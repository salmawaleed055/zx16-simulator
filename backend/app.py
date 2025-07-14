from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess
import threading
import queue
import os
import time

app = Flask(__name__)
CORS(app)

class Z16SimulatorManager:
    def __init__(self):
        self.active_sessions = {}
        self.session_lock = threading.Lock()

    def create_session(self, bin_file_path, interactive=True):
        session_id = str(int(time.time() * 1000))
        with self.session_lock:
            self.active_sessions[session_id] = {
                'process': None,
                'output': [],
                'queue': queue.Queue(),
                'is_running': False,
                'bin_file': bin_file_path,
                'interactive': interactive,
                'step_count': 0
            }
        return session_id

    def start_simulation(self, session_id):
        if session_id not in self.active_sessions:
            return False, "Session not found"
        session = self.active_sessions[session_id]
        cmd = ['/Users/mac/Documents/GitHub/zx16-simulator/backend/z16sim.exe']
        if session['interactive']:
            cmd.append('-i')
        cmd.append(session['bin_file'])
        env = os.environ.copy()
        try:
            session['process'] = subprocess.Popen(
                cmd,
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                bufsize=1,
                universal_newlines=True,
                env=env
            )
            threading.Thread(
                target=self._sim_reader_thread,
                args=(session_id,),
                daemon=True
            ).start()
            session['is_running'] = True
            return True, "Simulation started"
        except Exception as e:
            return False, f"Error starting simulator: {e}"

    def _sim_reader_thread(self, session_id):
        session = self.active_sessions[session_id]
        process = session['process']
        output_list = session['output']
        q = session['queue']
        while True:
            try:
                line = process.stdout.readline()
                if not line:
                    break
                line = line.strip()
                if line:
                    output_list.append(line)
                    if "READY_FOR_STEP" in line:
                        q.put("READY")
                    elif "Simulation halted" in line or "Program execution completed" in line:
                        q.put("HALTED")
                        session['is_running'] = False
            except Exception as e:
                print(f"Error reading simulator output: {e}")
                break

    def step_simulation(self, session_id):
        if session_id not in self.active_sessions:
            return False, "Session not found"
        session = self.active_sessions[session_id]
        if not session['process'] or session['process'].poll() is not None:
            return False, "No simulation running"
        try:
            session['process'].stdin.write("\n")
            session['process'].stdin.flush()
            try:
                result = session['queue'].get(timeout=5)
                session['step_count'] += 1
                if result == "HALTED":
                    session['is_running'] = False
                    return True, "Simulation halted"
                else:
                    return True, "Step completed"
            except queue.Empty:
                return False, "Timeout waiting for simulator response"
        except Exception as e:
            return False, f"Error stepping simulation: {e}"

    def get_full_output(self, session_id):
        if session_id not in self.active_sessions:
            return []
        return self.active_sessions[session_id]['output']

    def end_simulation(self, session_id):
        session = self.active_sessions.get(session_id)
        if session and session['process']:
            session['process'].terminate()
            session['is_running'] = False
            return True
        return False

    def get_step_count(self, session_id):
        session = self.active_sessions.get(session_id)
        if session:
            return session.get('step_count', 0)
        return 0

simulator_manager = Z16SimulatorManager()

@app.route('/simulate', methods=['POST'])
def simulate():
    try:
        if 'binfile' not in request.files:
            return jsonify({"error": "No file provided"}), 400
        file = request.files['binfile']
        if file.filename == '':
            return jsonify({"error": "No file selected"}), 400

        # Save the uploaded file
        timestamp = str(int(time.time()))
        filename = f"{timestamp}_{file.filename}"
        filepath = os.path.join('uploads', filename)
        os.makedirs('uploads', exist_ok=True)
        file.save(filepath)

        mode = request.form.get('mode', 'full')

        if mode == 'step':
            # Step-by-step mode
            session_id = simulator_manager.create_session(filepath, interactive=True)
            success, message = simulator_manager.start_simulation(session_id)
            if not success:
                return jsonify({"error": message}), 500
            return jsonify({
                "process_id": session_id,
                "initial_output": "\n".join(simulator_manager.get_full_output(session_id))
            })
        else:
            # Full simulation mode
            session_id = simulator_manager.create_session(filepath, interactive=False)
            success, message = simulator_manager.start_simulation(session_id)
            if not success:
                return jsonify({"error": message}), 500

            # Wait for simulation to complete
            max_wait_time = 60  # Increase if needed for larger programs
            start_time = time.time()
            while (time.time() - start_time) < max_wait_time:
                session = simulator_manager.active_sessions[session_id]
                if session['process'] and session['process'].poll() is not None:
                    break  # Process has finished
                if not session['is_running']:
                    break  # Simulation halted
                time.sleep(0.1)

            # Get the full output
            output_lines = simulator_manager.get_full_output(session_id)

            # Clean up the uploaded file
            try:
                os.remove(filepath)
            except Exception as e:
                print(f"Cleanup error: {e}")

            # Return as plain text if successful
            if output_lines:
                return '\n'.join(output_lines), 200, {'Content-Type': 'text/plain'}
            else:
                return jsonify({"error": "No output from simulator"}), 500

    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/health', methods=['GET'])
def health():
    # Optional: Health check endpoint for frontend
    cpp_source = os.path.exists('z16sim.cpp')
    executable_path = os.path.abspath('./z16sim')
    executable_found = os.path.exists(executable_path)
    compiler_available = any(
        os.access(os.path.join(path, 'g++'), os.X_OK)
        for path in os.environ["PATH"].split(os.pathsep)
    )
    return jsonify({
        "active_simulations": len(simulator_manager.active_sessions),
        "cpp_source_found": cpp_source,
        "compiler_available": compiler_available,
        "executable_found": executable_found,
        "executable_path": executable_path if executable_found else None
    })

@app.route('/compile', methods=['POST'])
def compile_simulator():
    # Optional: Compile endpoint for frontend
    if not os.path.exists('z16sim.cpp'):
        return jsonify({"error": "z16sim.cpp not found"}), 400
    try:
        result = subprocess.run(
            ['g++', '-std=c++17', '-O2', '-o', 'z16sim', 'z16sim.cpp', 'z16sim.h'],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        if result.returncode != 0:
            return jsonify({"error": result.stderr}), 500
        return jsonify({"message": "Compilation successful!"})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == '__main__':
    print("Frontend should connect to: http://localhost:5001")
    app.run(host='0.0.0.0', port=5001, debug=True)
