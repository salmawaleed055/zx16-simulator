from flask import Flask, request, jsonify, Response
from flask_cors import CORS
import subprocess
import os
import tempfile
import platform
import threading
import time
# import select # <--- This import should be removed if not used, or only used conditionally
import sys
import signal
import atexit
import shutil

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

active_simulations = {}
process_id_counter = 0
process_id_lock = threading.Lock()

def cleanup_processes():
    """Clean up all active processes on server shutdown"""
    for process_id, sim_info in list(active_simulations.items()):
        try:
            process = sim_info['process']
            if process.poll() is None:
                print(f"Cleaning up lingering process {process_id}...")
                process.terminate()
                try:
                    process.wait(timeout=2)
                except subprocess.TimeoutExpired:
                    process.kill()
            if os.path.exists(sim_info['temp_path']):
                os.unlink(sim_info['temp_path'])
            del active_simulations[process_id]
        except Exception as e:
            print(f"Error during cleanup for process {process_id}: {e}")

atexit.register(cleanup_processes)

def check_compiler():
    """Check if g++ compiler is available"""
    try:
        # Use shell=True for windows to help find g++.exe in PATH if not directly found
        result = subprocess.run(['g++', '--version'], capture_output=True, timeout=5, shell=platform.system() == 'Windows')
        return result.returncode == 0
    except (FileNotFoundError, subprocess.CalledProcessError):
        return False
    except Exception as e:
        print(f"Error checking compiler: {e}")
        return False

def compile_simulator():
    """Compile the z16sim.cpp file if it exists and executable doesn't exist"""
    cpp_file = 'z16sim.cpp'

    if platform.system() == 'Windows':
        executable = 'z16sim.exe'
        compile_cmd = ['g++', '-o', executable, cpp_file]
    else:
        executable = 'z16sim'
        compile_cmd = ['g++', '-o', executable, cpp_file]

    if not os.path.exists(cpp_file):
        return False, f"Source file {cpp_file} not found"

    if not check_compiler():
        return False, "g++ compiler not found. Please install a C++ compiler."

    if os.path.exists(executable):
        try:
            cpp_mtime = os.path.getmtime(cpp_file)
            exe_mtime = os.path.getmtime(executable)
            if exe_mtime > cpp_mtime:
                return True, f"Executable {executable} is up to date"
        except Exception as e:
            print(f"Warning: Could not get mtime for {executable} or {cpp_file}: {e}")
            pass

    try:
        print(f"Compiling {cpp_file}...")
        # Use shell=True on Windows for robustness if g++ path isn't straightforward
        result = subprocess.run(compile_cmd, capture_output=True, text=True, timeout=30, shell=platform.system() == 'Windows')

        if result.returncode == 0:
            if platform.system() != 'Windows':
                os.chmod(executable, 0o755)
            print(f"Compilation successful: {executable}")
            return True, f"Successfully compiled {executable}"
        else:
            print(f"Compilation failed: {result.stderr}")
            return False, f"Compilation failed: {result.stderr}"

    except subprocess.TimeoutExpired:
        print("Compilation timed out")
        return False, "Compilation timed out"
    except FileNotFoundError:
        print("g++ compiler not found during compilation attempt.")
        return False, "g++ compiler not found. Please install a C++ compiler."
    except Exception as e:
        print(f"Compilation error: {str(e)}")
        return False, f"Compilation error: {str(e)}"

def check_executable():
    """Check if the simulator executable exists and return its absolute path."""
    if platform.system() == 'Windows':
        executable_name = 'z16sim.exe'
    else:
        executable_name = 'z16sim'

    # Check current directory first
    if os.path.exists(executable_name):
        abs_path = os.path.abspath(executable_name)
        if platform.system() != 'Windows':
            if os.access(abs_path, os.X_OK):
                return abs_path, "Executable found and ready"
            else:
                return None, "Executable found but not executable (permissions issue). Try `chmod +x z16sim`."
        else:
            return abs_path, "Executable found and ready"

    # Also search PATH
    path_executable = shutil.which(executable_name)
    if path_executable:
        if platform.system() != 'Windows':
            if os.access(path_executable, os.X_OK):
                return os.path.abspath(path_executable), "Executable found in PATH and ready"
            else:
                return None, "Executable found in PATH but not executable (permissions issue)."
        else:
            return os.path.abspath(path_executable), "Executable found in PATH and ready"

    return None, "Executable not found"

def read_until_ready(process, timeout=10):
    """
    Read from process stdout until we see "READY_FOR_STEP", EOF, or timeout.
    Returns collected output lines and a boolean indicating if "READY_FOR_STEP" was found.

    This function relies on the subprocess printing line-by-line and flushing its stdout.
    """
    output_buffer = []
    start_time = time.time()
    ready_marker_found = False

    while time.time() - start_time < timeout:
        if process.poll() is not None:
            # Process has terminated, read any remaining output
            try:
                # Read all remaining output if process exited
                remaining_output = process.stdout.read()
                if remaining_output:
                    # Split by lines and append
                    for line in remaining_output.strip().splitlines():
                        output_buffer.append(line)
            except ValueError: # stdin/stdout/stderr are closed
                pass
            break

        try:
            line = process.stdout.readline() # This will block until a newline or EOF
            if line:
                line = line.strip()
                if line == "READY_FOR_STEP":
                    ready_marker_found = True
                    break
                output_buffer.append(line)
            else: # EOF reached, process might have ended
                # Give a brief moment for process to fully terminate
                time.sleep(0.01)
                if process.poll() is not None: # Confirm termination
                    break
                else: # No line, but process still alive, wait a bit
                    time.sleep(0.05)
        except Exception as e:
            # This can catch BrokenPipeError, ValueError if stream is closed, etc.
            print(f"Error reading from process stdout (might be closed): {e}")
            break

        # Small sleep to prevent busy-waiting if readline() isn't producing data rapidly
        time.sleep(0.01)

        # After the loop, attempt to read any final remaining data that might have buffered
    # This is a bit tricky with line-buffered, but important if process terminates
    # right after the marker.
    try:
        # Read any remaining data that's immediately available without blocking
        # This loop will quickly drain anything left after the main loop finishes.
        while True:
            # On Unix-like systems, we can use select for non-blocking read
            if platform.system() != 'Windows':
                import select # Import select here to ensure it's only used where applicable
                rlist, _, _ = select.select([process.stdout], [], [], 0.001) # Very short timeout
                if rlist:
                    line = process.stdout.readline()
                    if line:
                        line = line.strip()
                        if line == "READY_FOR_STEP":
                            ready_marker_found = True
                        else:
                            output_buffer.append(line)
                    else: # EOF
                        break
                else: # No more data for now
                    break
            else: # On Windows, just try readline one more time and break if empty
                line = process.stdout.readline()
                if line:
                    line = line.strip()
                    if line == "READY_FOR_STEP":
                        ready_marker_found = True
                    else:
                        output_buffer.append(line)
                else:
                    break # EOF or no more data
                time.sleep(0.001) # Small pause to yield control

    except Exception as e:
        print(f"Error during final read_until_ready drain: {e}")

    return output_buffer, ready_marker_found

@app.route('/compile', methods=['POST'])
def compile_endpoint():
    try:
        success, message = compile_simulator()
        if success:
            return jsonify({'status': 'success', 'message': message}), 200
        else:
            return jsonify({'status': 'error', 'message': message}), 500
    except Exception as e:
        return jsonify({'status': 'error', 'message': f'Compilation error: {str(e)}'}), 500

@app.route('/simulate', methods=['POST'])
def simulate():
    global process_id_counter

    try:
        if 'binfile' not in request.files:
            return jsonify({'error': 'No file uploaded'}), 400

        file = request.files['binfile']

        if file.filename == '':
            return jsonify({'error': 'No file selected'}), 400

        executable_path, message = check_executable()
        if not executable_path:
            return jsonify({'error': f'Simulator not available: {message}'}), 500

        simulation_mode = request.form.get('mode', 'full')

        with tempfile.NamedTemporaryFile(delete=False, suffix='.bin') as temp_file:
            file.save(temp_file.name)
            temp_path = temp_file.name

        try:
            if simulation_mode == 'full':
                print(f"Running full simulation: {executable_path} {temp_path}")

                result = subprocess.run(
                    [executable_path, temp_path],
                    capture_output=True,
                    text=True,
                    timeout=120, # Increased timeout
                    cwd=os.path.dirname(executable_path) or os.getcwd()
                )

                try:
                    os.unlink(temp_path)
                except Exception as e:
                    print(f"Warning: Could not delete temp file {temp_path}: {e}")

                if result.returncode == 0:
                    return Response(result.stdout, mimetype='text/plain')
                else:
                    error_msg = result.stderr if result.stderr else "Unknown simulation error"
                    print(f"Full simulation failed: {error_msg}")
                    return jsonify({'error': f'Simulation failed: {error_msg}'}), 500

            elif simulation_mode == 'step':
                with process_id_lock:
                    current_process_id = process_id_counter
                    process_id_counter += 1

                print(f"Starting step-by-step simulation: {executable_path} -i {temp_path}")

                process = subprocess.Popen(
                    [executable_path, '-i', temp_path],
                    stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True,
                    bufsize=1,  # Line-buffered
                    universal_newlines=True,
                    cwd=os.path.dirname(executable_path) or os.getcwd()
                )

                active_simulations[current_process_id] = {
                    'process': process,
                    'temp_path': temp_path,
                    'step_count': 0,
                    'created_at': time.time()
                }

                # Read initial output until "READY_FOR_STEP" is found or process terminates
                initial_output_lines, ready_marker_found = read_until_ready(process)

                if process.poll() is not None:
                    # Process terminated during startup
                    stderr_output = ""
                    try:
                        stderr_output = process.stderr.read()
                    except:
                        pass

                    try:
                        os.unlink(temp_path)
                    except Exception as e:
                        print(f"Warning: Could not delete temp file {temp_path} during step startup cleanup: {e}")

                    if current_process_id in active_simulations:
                        del active_simulations[current_process_id]

                    print(f"Step simulation process {current_process_id} terminated during startup. Return code: {process.returncode}. Stderr: {stderr_output}")
                    return jsonify({
                        'error': f'Simulation process terminated during startup. Return code: {process.returncode}. Output: {"\\n".join(initial_output_lines)}. Error: {stderr_output}'
                    }), 500

                if not ready_marker_found:
                    print(f"Process {current_process_id} did not signal READY_FOR_STEP. Initial output: {initial_output_lines}")
                    # Attempt to gracefully terminate and clean up
                    try:
                        process.stdin.write('q\n')
                        process.stdin.flush()
                        process.wait(timeout=2)
                    except Exception as e:
                        print(f"Error during graceful shutdown attempt for {current_process_id}: {e}")
                        process.kill()

                    try:
                        os.unlink(temp_path)
                    except Exception as e:
                        print(f"Warning: Could not delete temp file {temp_path} after non-ready startup: {e}")

                    if current_process_id in active_simulations:
                        del active_simulations[current_process_id]

                    return jsonify({
                        'error': f'Simulator did not signal readiness after initial output. Initial output: {"\\n".join(initial_output_lines)}. Ensure your C++ simulator prints "READY_FOR_STEP" and flushes stdout.'
                    }), 500

                initial_output_str = "\n".join([line for line in initial_output_lines if line != "READY_FOR_STEP"])

                return jsonify({
                    'status': 'Simulation started in step-by-step mode',
                    'process_id': current_process_id,
                    'initial_output': initial_output_str
                }), 200

            else:
                try:
                    os.unlink(temp_path)
                except Exception as e:
                    print(f"Warning: Could not delete temp file {temp_path} for invalid mode: {e}")
                return jsonify({'error': 'Invalid simulation mode specified'}), 400

        except subprocess.TimeoutExpired as e:
            if 'temp_path' in locals() and os.path.exists(temp_path):
                try:
                    os.unlink(temp_path)
                except Exception as ex:
                    print(f"Warning: Could not delete temp file {temp_path} after TimeoutExpired: {ex}")
            print(f"Simulation startup timed out: {e}")
            return jsonify({'error': f'Simulation startup timed out: {str(e)}'}), 500
        except FileNotFoundError:
            if 'temp_path' in locals() and os.path.exists(temp_path):
                try:
                    os.unlink(temp_path)
                except Exception as e:
                    print(f"Warning: Could not delete temp file {temp_path} after FileNotFoundError: {e}")
            print(f"Simulator executable not found: {executable_path}")
            return jsonify({'error': f'Simulator executable not found: {executable_path}'}), 500
        except Exception as e:
            if 'temp_path' in locals() and os.path.exists(temp_path):
                try:
                    os.unlink(temp_path)
                except Exception as ex:
                    print(f"Warning: Could not delete temp file {temp_path} after general exception: {ex}")
            print(f"Error running simulator: {str(e)}")
            return jsonify({'error': f'Error running simulator: {str(e)}'}), 500

    except Exception as e:
        print(f"Top-level server error: {str(e)}")
        return jsonify({'error': f'Server error: {str(e)}'}), 500

@app.route('/step_simulation', methods=['POST'])
def step_simulation():
    data = request.get_json()
    process_id = data.get('process_id')
    command = data.get('command', '\n')

    if process_id is None:
        return jsonify({'error': 'process_id is required'}), 400

    simulation_info = active_simulations.get(process_id)
    if not simulation_info:
        # Check if it terminated recently and clean up temp_path if still exists
        if 'temp_path' in data:
            temp_path_from_client = data['temp_path']
            if os.path.exists(temp_path_from_client):
                try:
                    os.unlink(temp_path_from_client)
                    print(f"Cleaned up stray temp file: {temp_path_from_client}")
                except Exception as e:
                    print(f"Warning: Failed to clean up stray temp file {temp_path_from_client}: {e}")

        return jsonify({'error': 'Simulation process not found or has ended. Please start a new simulation.'}), 404

    process = simulation_info['process']
    temp_path = simulation_info['temp_path']

    try:
        # Check if process has already terminated before sending command
        if process.poll() is not None:
            print(f"Process {process_id} already terminated before step request.")
            try:
                os.unlink(temp_path)
            except Exception as e:
                print(f"Warning: Could not delete temp file {temp_path} during pre-step cleanup: {e}")
            if process_id in active_simulations:
                del active_simulations[process_id]
            return jsonify({
                'status': 'Simulation finished',
                'output': 'Simulation ended unexpectedly.',
                'terminated': True,
                'step_count': simulation_info['step_count']
            }), 200

        print(f"Sending command to process {process_id}: {repr(command.strip())}")
        process.stdin.write(command) # `command` already includes `\n` from frontend
        process.stdin.flush()

        # Read output until we see "READY_FOR_STEP" or process terminates
        output_lines, ready_marker_found = read_until_ready(process, timeout=10) # Adjust timeout if steps are slow

        filtered_output = [line for line in output_lines if line != "READY_FOR_STEP"]
        current_output = "\n".join(filtered_output)

        simulation_info['step_count'] += 1

        # Check if process terminated *after* this step
        if process.poll() is not None:
            print(f"Process {process_id} terminated after step. Return code: {process.returncode}")
            try:
                os.unlink(temp_path)
            except Exception as e:
                print(f"Warning: Could not delete temp file {temp_path} during post-step cleanup: {e}")
            if process_id in active_simulations:
                del active_simulations[process_id]

            stderr_output = ""
            try:
                stderr_output = process.stderr.read()
            except:
                pass
            if stderr_output.strip():
                current_output += f"\n\n[Simulator Stderr]:\n{stderr_output.strip()}"

            return jsonify({
                'status': 'Simulation step executed',
                'output': current_output,
                'terminated': True,
                'return_code': process.returncode,
                'step_count': simulation_info['step_count']
            }), 200
        else:
            if not ready_marker_found:
                print(f"Warning: Process {process_id} did not send READY_FOR_STEP after step. Current output: {current_output}")
                # Force termination to prevent hanging
                process.terminate()
                try:
                    process.wait(timeout=2)
                except subprocess.TimeoutExpired:
                    process.kill()
                try:
                    os.unlink(temp_path)
                except Exception as e:
                    print(f"Warning: Could not delete temp file {temp_path} during hang cleanup: {e}")
                if process_id in active_simulations:
                    del active_simulations[process_id]
                return jsonify({
                    'error': f'Simulator did not signal readiness after step {simulation_info["step_count"]}. Possible hang. Process terminated. Last output: {current_output}',
                    'output': current_output,
                    'terminated': True,
                    'step_count': simulation_info['step_count']
                }), 500

            return jsonify({
                'status': 'Simulation step executed',
                'output': current_output,
                'terminated': False,
                'step_count': simulation_info['step_count']
            }), 200

    except Exception as e:
        print(f"Error during step simulation for process {process_id}: {e}")
        if process_id in active_simulations:
            sim_info_to_clean = active_simulations[process_id]
            try:
                proc_to_clean = sim_info_to_clean['process']
                tmp_path_to_clean = sim_info_to_clean['temp_path']
                if proc_to_clean.poll() is None:
                    try:
                        proc_to_clean.terminate()
                        proc_to_clean.wait(timeout=2)
                    except subprocess.TimeoutExpired:
                        proc_to_clean.kill()
                if os.path.exists(tmp_path_to_clean):
                    os.unlink(tmp_path_to_clean)
                del active_simulations[process_id]
            except Exception as clean_err:
                print(f"Error during cleanup after step error for {process_id}: {clean_err}")
        return jsonify({'error': f'Error during step simulation: {str(e)}'}), 500

@app.route('/end_simulation', methods=['POST'])
def end_simulation():
    data = request.get_json()
    process_id = data.get('process_id')

    if process_id is None:
        return jsonify({'error': 'process_id is required'}), 400

    simulation_info = active_simulations.get(process_id)
    if not simulation_info:
        return jsonify({'status': 'Simulation already ended or not found'}), 200

    process = simulation_info['process']
    temp_path = simulation_info['temp_path']

    try:
        if process.poll() is None:
            print(f"Sending 'q' to terminate process {process_id}")
            try:
                process.stdin.write('q\n')
                process.stdin.flush()
            except BrokenPipeError:
                print(f"Stdin pipe for process {process_id} already closed, likely terminated.")
            except Exception as e:
                print(f"Error writing 'q' to process {process_id}: {e}")
                pass

            try:
                process.wait(timeout=3)
                print(f"Process {process_id} terminated gracefully.")
            except subprocess.TimeoutExpired:
                print(f"Process {process_id} did not terminate gracefully, killing it.")
                process.terminate()
                try:
                    process.wait(timeout=2)
                except subprocess.TimeoutExpired:
                    process.kill()
                except Exception as e:
                    print(f"Error killing process {process_id}: {e}")
            except Exception as e:
                print(f"Error waiting for process {process_id} termination: {e}")

        try:
            if os.path.exists(temp_path):
                os.unlink(temp_path)
                print(f"Cleaned up temp file: {temp_path}")
        except Exception as e:
            print(f"Warning: Could not delete temp file {temp_path} during end_simulation: {e}")

        if process_id in active_simulations:
            del active_simulations[process_id]
            print(f"Removed process {process_id} from active simulations.")

        return jsonify({'status': 'Simulation ended successfully'}), 200

    except Exception as e:
        print(f"Error ending simulation {process_id}: {e}")
        if process_id in active_simulations:
            sim_info_to_clean = active_simulations[process_id]
            try:
                proc_to_clean = sim_info_to_clean['process']
                tmp_path_to_clean = sim_info_to_clean['temp_path']
                if proc_to_clean.poll() is None:
                    try:
                        proc_to_clean.terminate()
                        proc_to_clean.wait(timeout=2)
                    except:
                        proc_to_clean.kill()
                if os.path.exists(tmp_path_to_clean):
                    os.unlink(tmp_path_to_clean)
                del active_simulations[process_id]
            except Exception as clean_err:
                print(f"Error during cleanup after end_simulation error for {process_id}: {clean_err}")
        return jsonify({'error': f'Error ending simulation: {str(e)}'}), 500

@app.route('/health', methods=['GET'])
def health_check():
    executable_path, exec_message = check_executable()
    cpp_exists = os.path.exists('z16sim.cpp')
    compiler_available = check_compiler()

    return jsonify({
        'status': 'Backend is running',
        'message': 'ZX16 Simulator Backend',
        'executable_found': executable_path is not None,
        'executable_path': executable_path if executable_path else 'Not found',
        'executable_message': exec_message,
        'cpp_source_found': cpp_exists,
        'active_simulations': len(active_simulations),
        'compiler_available': compiler_available
    })

@app.route('/status', methods=['GET'])
def get_status():
    executable_path, message = check_executable()
    return jsonify({
        'active_simulations': len(active_simulations),
        'process_ids': list(active_simulations.keys()),
        'executable_found': executable_path is not None,
        'executable_message': message,
        'cpp_source_found': os.path.exists('z16sim.cpp'),
        'compiler_available': check_compiler()
    })

@app.route('/files', methods=['GET'])
def list_files():
    files = []
    for file_name in os.listdir('.'):
        if file_name.endswith(('.cpp', '.exe', '.bin')) or file_name == 'z16sim':
            try:
                stat = os.stat(file_name)
                files.append({
                    'name': file_name,
                    'size': stat.st_size,
                    'modified': stat.st_mtime
                })
            except Exception as e:
                print(f"Error stating file {file_name}: {e}")
                pass
    return jsonify({'files': files})

if __name__ == '__main__':
    print("🚀 Starting ZX16 Simulator Backend...")
    print("📁 Looking for z16sim.cpp source file...")

    cpp_exists = os.path.exists('z16sim.cpp')
    executable_path, message = check_executable()

    if cpp_exists:
        print(f"✅ Found z16sim.cpp")
        if executable_path:
            print(f"✅ {message}")
        else:
            print(f"⚠️  {message}")
            print("🔨 Will attempt to compile when needed")
    else:
        print("❌ z16sim.cpp not found in current directory")
        print("📂 Please ensure z16sim.cpp is in the same directory as this script")

    if check_compiler():
        print("✅ g++ compiler found")
    else:
        print("❌ g++ compiler not found - compilation will not be possible")

    print("🌐 Backend will run on http://localhost:5001")
    # Set use_reloader to False to avoid double execution on some systems
    app.run(debug=True, host='0.0.0.0', port=5001, use_reloader=False)