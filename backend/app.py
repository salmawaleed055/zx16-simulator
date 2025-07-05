from flask import Flask, request, jsonify, Response
from flask_cors import CORS
import subprocess
import os
import tempfile
import platform
import threading
import time
import select
import sys
import signal
import atexit

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

# Dictionary to store active simulation processes for step-by-step mode
active_simulations = {}
process_id_counter = 0
process_id_lock = threading.Lock()

def cleanup_processes():
    """Clean up all active processes on server shutdown"""
    for process_id, sim_info in active_simulations.items():
        try:
            process = sim_info['process']
            if process.poll() is None:
                process.terminate()
                try:
                    process.wait(timeout=2)
                except subprocess.TimeoutExpired:
                    process.kill()
            if os.path.exists(sim_info['temp_path']):
                os.unlink(sim_info['temp_path'])
        except:
            pass

# Register cleanup function
atexit.register(cleanup_processes)

def compile_simulator():
    """Compile the z16sim.cpp file if it exists and executable doesn't exist"""
    cpp_file = 'z16sim.cpp'

    if platform.system() == 'Windows':
        executable = 'z16sim.exe'
        compile_cmd = ['g++', '-o', executable, cpp_file]
    else:
        executable = 'z16sim'
        compile_cmd = ['g++', '-o', executable, cpp_file]

    # Check if source file exists
    if not os.path.exists(cpp_file):
        return False, f"Source file {cpp_file} not found"

    # Check if executable already exists and is newer than source
    if os.path.exists(executable):
        cpp_mtime = os.path.getmtime(cpp_file)
        exe_mtime = os.path.getmtime(executable)
        if exe_mtime > cpp_mtime:
            return True, f"Executable {executable} is up to date"

    try:
        print(f"Compiling {cpp_file}...")
        result = subprocess.run(compile_cmd, capture_output=True, text=True, timeout=30)

        if result.returncode == 0:
            # Make executable on Unix-like systems
            if platform.system() != 'Windows':
                os.chmod(executable, 0o755)
            return True, f"Successfully compiled {executable}"
        else:
            return False, f"Compilation failed: {result.stderr}"

    except subprocess.TimeoutExpired:
        return False, "Compilation timed out"
    except FileNotFoundError:
        return False, "g++ compiler not found. Please install a C++ compiler."
    except Exception as e:
        return False, f"Compilation error: {str(e)}"

def check_executable():
    """Check if the simulator executable exists, compile if needed"""
    if platform.system() == 'Windows':
        executable = 'z16sim.exe'
    else:
        executable = 'z16sim'

    # First check if executable exists
    if os.path.exists(executable):
        return executable, "Executable found"

    # Try to compile if source exists
    success, message = compile_simulator()
    if success and os.path.exists(executable):
        return executable, message

    return None, message

def read_until_ready(process, timeout=10):
    """Read from process stdout until we see 'READY_FOR_STEP' or timeout"""
    output_lines = []
    start_time = time.time()

    while time.time() - start_time < timeout:
        if process.poll() is not None:
            # Process has terminated
            break

        try:
            if platform.system() != 'Windows':
                # Unix-like systems
                ready, _, _ = select.select([process.stdout], [], [], 0.1)
                if ready:
                    line = process.stdout.readline()
                    if line:
                        line = line.strip()
                        output_lines.append(line)
                        if line == "READY_FOR_STEP":
                            break
            else:
                # Windows - use a different approach
                line = process.stdout.readline()
                if line:
                    line = line.strip()
                    output_lines.append(line)
                    if line == "READY_FOR_STEP":
                        break
                else:
                    time.sleep(0.1)
        except Exception as e:
            print(f"Error reading from process: {e}")
            break

    return output_lines

@app.route('/compile', methods=['POST'])
def compile_endpoint():
    """Endpoint to manually trigger compilation"""
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

        # Check if executable exists, compile if needed
        executable, message = check_executable()
        if not executable:
            return jsonify({'error': f'Simulator not available: {message}'}), 500

        # Get the simulation mode from the request (default to 'full')
        simulation_mode = request.form.get('mode', 'full')

        with tempfile.NamedTemporaryFile(delete=False, suffix='.bin') as temp_file:
            file.save(temp_file.name)
            temp_path = temp_file.name

        try:
            if simulation_mode == 'full':
                # --- Full Simulation Mode ---
                print(f"Running full simulation: {executable} {temp_path}")
                result = subprocess.run(
                    [f'./{executable}', temp_path],
                    capture_output=True,
                    text=True,
                    timeout=60,
                    cwd=os.getcwd()
                )
                os.unlink(temp_path)

                if result.returncode == 0:
                    return result.stdout
                else:
                    error_msg = result.stderr if result.stderr else "Unknown simulation error"
                    return jsonify({'error': f'Simulation failed: {error_msg}'}), 500

            elif simulation_mode == 'step':
                # --- Step-by-Step Simulation Mode ---
                with process_id_lock:
                    current_process_id = process_id_counter
                    process_id_counter += 1

                print(f"Starting step-by-step simulation: {executable} -i {temp_path}")

                # Start the process in interactive mode
                process = subprocess.Popen(
                    [f'./{executable}', '-i', temp_path],
                    stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True,
                    bufsize=0,  # Unbuffered
                    universal_newlines=True,
                    cwd=os.getcwd()
                )

                active_simulations[current_process_id] = {
                    'process': process,
                    'temp_path': temp_path,
                    'step_count': 0,
                    'created_at': time.time()
                }

                # Read initial output until we see "READY_FOR_STEP" or process terminates
                initial_output = read_until_ready(process)

                # Check if process terminated during startup
                if process.poll() is not None:
                    # Process terminated during startup
                    stderr_output = process.stderr.read() if process.stderr else ""
                    os.unlink(temp_path)
                    del active_simulations[current_process_id]
                    return jsonify({
                        'error': f'Simulation process terminated during startup. Error: {stderr_output}'
                    }), 500

                # Filter out the "READY_FOR_STEP" marker from the output
                filtered_output = [line for line in initial_output if line != "READY_FOR_STEP"]
                initial_output_str = "\n".join(filtered_output)

                return jsonify({
                    'status': 'Simulation started in step-by-step mode',
                    'process_id': current_process_id,
                    'initial_output': initial_output_str
                }), 200

            else:
                os.unlink(temp_path)
                return jsonify({'error': 'Invalid simulation mode specified'}), 400

        except subprocess.TimeoutExpired:
            os.unlink(temp_path)
            return jsonify({'error': 'Simulation timed out'}), 500
        except FileNotFoundError:
            os.unlink(temp_path)
            return jsonify({'error': f'Simulator executable not found: {executable}'}), 500
        except Exception as e:
            os.unlink(temp_path)
            return jsonify({'error': f'Error running simulator: {str(e)}'}), 500

    except Exception as e:
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
        return jsonify({'error': 'Simulation process not found or has ended'}), 404

    process = simulation_info['process']
    temp_path = simulation_info['temp_path']

    try:
        if process.poll() is not None:
            # Process has already terminated
            try:
                os.unlink(temp_path)
            except:
                pass
            del active_simulations[process_id]
            return jsonify({
                'status': 'Simulation finished',
                'output': 'Simulation ended.',
                'terminated': True
            }), 200

        # Send command to the process (usually just Enter for next step)
        print(f"Sending command to process {process_id}: {repr(command)}")
        process.stdin.write(command + '\n')
        process.stdin.flush()

        # Read output until we see "READY_FOR_STEP" or process terminates
        output_lines = read_until_ready(process)

        # Filter out the "READY_FOR_STEP" marker
        filtered_output = [line for line in output_lines if line != "READY_FOR_STEP"]
        current_output = "\n".join(filtered_output)

        simulation_info['step_count'] += 1

        if process.poll() is not None:
            # Process terminated after this step
            try:
                os.unlink(temp_path)
            except:
                pass
            del active_simulations[process_id]
            return jsonify({
                'status': 'Simulation step executed',
                'output': current_output,
                'terminated': True,
                'return_code': process.returncode,
                'step_count': simulation_info['step_count']
            }), 200
        else:
            return jsonify({
                'status': 'Simulation step executed',
                'output': current_output,
                'terminated': False,
                'step_count': simulation_info['step_count']
            }), 200

    except Exception as e:
        # Clean up in case of an error during stepping
        print(f"Error during step simulation: {e}")
        if process_id in active_simulations:
            try:
                process = active_simulations[process_id]['process']
                temp_path = active_simulations[process_id]['temp_path']
                if process.poll() is None:
                    process.terminate()
                    try:
                        process.wait(timeout=2)
                    except subprocess.TimeoutExpired:
                        process.kill()
                os.unlink(temp_path)
                del active_simulations[process_id]
            except:
                pass
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
            # Send 'q' to quit gracefully
            print(f"Sending 'q' to terminate process {process_id}")
            process.stdin.write('q\n')
            process.stdin.flush()

            # Wait a bit for graceful termination
            try:
                process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                process.terminate()
                try:
                    process.wait(timeout=2)
                except subprocess.TimeoutExpired:
                    process.kill()

        try:
            os.unlink(temp_path)
        except:
            pass
        del active_simulations[process_id]
        return jsonify({'status': 'Simulation ended successfully'}), 200

    except Exception as e:
        print(f"Error ending simulation: {e}")
        # Ensure cleanup even if termination fails
        if process_id in active_simulations:
            try:
                process = active_simulations[process_id]['process']
                temp_path = active_simulations[process_id]['temp_path']
                if process.poll() is None:
                    try:
                        process.terminate()
                        process.wait(timeout=2)
                    except:
                        process.kill()
                try:
                    os.unlink(temp_path)
                except:
                    pass
                del active_simulations[process_id]
            except:
                pass
        return jsonify({'error': f'Error ending simulation: {str(e)}'}), 500

@app.route('/health', methods=['GET'])
def health_check():
    executable, message = check_executable()
    cpp_exists = os.path.exists('z16sim.cpp')

    return jsonify({
        'status': 'Backend is running',
        'message': 'ZX16 Simulator Backend',
        'executable_found': executable is not None,
        'executable_path': executable,
        'executable_message': message,
        'cpp_source_found': cpp_exists,
        'active_simulations': len(active_simulations),
        'compiler_available': subprocess.run(['g++', '--version'], capture_output=True).returncode == 0
    })

@app.route('/status', methods=['GET'])
def get_status():
    """Get current backend status and active simulations"""
    executable, message = check_executable()
    return jsonify({
        'active_simulations': len(active_simulations),
        'process_ids': list(active_simulations.keys()),
        'executable_found': executable is not None,
        'executable_message': message,
        'cpp_source_found': os.path.exists('z16sim.cpp')
    })

@app.route('/files', methods=['GET'])
def list_files():
    """List relevant files in the current directory"""
    files = []
    for file in os.listdir('.'):
        if file.endswith(('.cpp', '.exe', '.bin')) or file == 'z16sim':
            stat = os.stat(file)
            files.append({
                'name': file,
                'size': stat.st_size,
                'modified': stat.st_mtime
            })
    return jsonify({'files': files})

if __name__ == '__main__':
    print("🚀 Starting ZX16 Simulator Backend...")
    print("📁 Looking for z16sim.cpp source file...")

    # Check current directory contents
    cpp_exists = os.path.exists('z16sim.cpp')
    executable, message = check_executable()

    if cpp_exists:
        print(f"✅ Found z16sim.cpp")
        if executable:
            print(f"✅ {message}")
        else:
            print(f"⚠️  {message}")
            print("🔨 Will attempt to compile when needed")
    else:
        print("❌ z16sim.cpp not found in current directory")
        print("📂 Please ensure z16sim.cpp is in the same directory as this script")

    # Check for compiler
    try:
        result = subprocess.run(['g++', '--version'], capture_output=True, timeout=5)
        if result.returncode == 0:
            print("✅ g++ compiler found")
        else:
            print("❌ g++ compiler not working properly")
    except:
        print("❌ g++ compiler not found - compilation will not be possible")

    print("🌐 Backend will run on http://localhost:5001")
    app.run(debug=True, host='0.0.0.0', port=5001)