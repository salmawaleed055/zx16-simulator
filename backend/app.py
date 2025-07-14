from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
import subprocess
import threading
import queue
import os
import tempfile
import time
import json
import base64
import struct
from datetime import datetime

app = Flask(__name__)
process_id_counter = 0
CORS(app)

class Z16SimulatorManager:
    def __init__(self):
        self.active_sessions = {}
        self.session_lock = threading.Lock()

    def create_session(self, bin_file_path, interactive=True):
        """Create a new simulation session"""
        session_id = str(int(time.time() * 1000))  # Use timestamp as ID

        with self.session_lock:
            self.active_sessions[session_id] = {
                'process': None,
                'output': [],
                'queue': queue.Queue(),
                'is_running': False,
                'bin_file': bin_file_path,
                'interactive': interactive,
                'graphics_data': None,
                'last_graphics_update': None
            }

        return session_id

    def start_simulation(self, session_id):
        """Start the Z16 simulator process"""
        if session_id not in self.active_sessions:
            return False, "Session not found"

        session = self.active_sessions[session_id]

        # Build command - add headless flag to prevent SFML window
        cmd = ['./z16sim']
        if session['interactive']:
            cmd.append('-i')
        cmd.append(session['bin_file'])

        # Set environment variable to run headless (no SFML window)
        env = os.environ.copy()
        env['DISPLAY'] = ':99'  # Use virtual display

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

            # Start reader thread
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
        """Read simulator output in separate thread"""
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

                    # Check for graphics memory updates
                    if "Graphics memory updated" in line:
                        self._capture_graphics_state(session_id)

                    # Signal when simulator is ready for next step
                    if "READY_FOR_STEP" in line:
                        q.put("READY")
                    elif "Simulation halted" in line or "Program execution completed" in line:
                        q.put("HALTED")
                        session['is_running'] = False

            except Exception as e:
                print(f"Error reading simulator output: {e}")
                break

    def _capture_graphics_state(self, session_id):
        """Capture current graphics state by reading simulator memory"""
        session = self.active_sessions[session_id]

        # For now, we'll create a mock graphics state
        # In a real implementation, you'd read from the simulator's memory
        graphics_data = {
            'timestamp': datetime.now().isoformat(),
            'width': 320,  # Adjust based on your graphics resolution
            'height': 240,
            'tile_width': 8,
            'tile_height': 8,
            'palette': self._get_default_palette(),
            'framebuffer': self._extract_framebuffer(session_id),
            'needs_update': True
        }

        session['graphics_data'] = graphics_data
        session['last_graphics_update'] = time.time()

    def _get_default_palette(self):
        """Get default color palette"""
        return [
            '#000000', '#FFFFFF', '#FF0000', '#00FF00',
            '#0000FF', '#FFFF00', '#FF00FF', '#00FFFF',
            '#800000', '#008000', '#000080', '#808000',
            '#800080', '#008080', '#808080', '#C0C0C0'
        ]

    def _extract_framebuffer(self, session_id):
        """Extract framebuffer data from simulator"""
        # This is a mock implementation
        # In reality, you'd need to modify your C++ simulator to output graphics data
        width, height = 320, 240

        # Create a simple test pattern
        framebuffer = []
        for y in range(height):
            row = []
            for x in range(width):
                # Simple gradient pattern
                color_index = ((x + y) // 20) % 16
                row.append(color_index)
            framebuffer.append(row)

        return framebuffer

    def get_graphics_data(self, session_id):
        """Get current graphics state"""
        if session_id not in self.active_sessions:
            return None

        session = self.active_sessions[session_id]
        return session.get('graphics_data')

    def step_simulation(self, session_id):
        """Execute one simulation step"""
        if session_id not in self.active_sessions:
            return False, "Session not found"

        session = self.active_sessions[session_id]

        if not session['process'] or session['process'].poll() is not None:
            return False, "No simulation running"

        try:
            session['process'].stdin.write("\n")
            session['process'].stdin.flush()

            # Wait for response
            try:
                result = session['queue'].get(timeout=5)

                # Capture graphics state after each step
                self._capture_graphics_state(session_id)

                if result == "HALTED":
                    session['is_running'] = False
                    return True, "Simulation halted"
                else:
                    return True, "Step completed"
            except queue.Empty:
                return False, "Timeout waiting for simulator response"

        except Exception as e:
            return False, f"Error stepping simulation: {e}"

# Global simulator manager
simulator_manager = Z16SimulatorManager()

@app.route('/simulate', methods=['POST'])
def simulate():
    """Handle binary file upload and simulation"""
    try:
        if 'binfile' not in request.files:
            return "❌ No file provided", 400

        file = request.files['binfile']

        if file.filename == '':
            return "❌ No file selected", 400

        # Save uploaded file
        timestamp = str(int(time.time()))
        filename = f"{timestamp}_{file.filename}"
        filepath = os.path.join('uploads', filename)

        os.makedirs('uploads', exist_ok=True)
        file.save(filepath)

        # Create simulation session
        session_id = simulator_manager.create_session(filepath, interactive=False)

        # Start simulation
        success, message = simulator_manager.start_simulation(session_id)

        if not success:
            return f"❌ {message}", 500

        # Wait for simulation to complete
        max_wait_time = 30
        start_time = time.time()

        while (time.time() - start_time) < max_wait_time:
            session = simulator_manager.active_sessions[session_id]

            if session['process'] and session['process'].poll() is not None:
                break

            if not session['is_running']:
                break

            time.sleep(0.1)

        # Get output
        output_lines = simulator_manager.get_full_output(session_id)

        # Clean up
        try:
            os.remove(filepath)
        except:
            pass

        return '\n'.join(output_lines), 200, {'Content-Type': 'text/plain'}

    except Exception as e:
        return f"❌ Error: {str(e)}", 500

@app.route('/simulate-interactive', methods=['POST'])
def simulate_interactive():
    """Start interactive simulation"""
    try:
        if 'binfile' not in request.files:
            return jsonify({"error": "No file provided"}), 400

        file = request.files['binfile']

        if file.filename == '':
            return jsonify({"error": "No file selected"}), 400

        # Save uploaded file
        timestamp = str(int(time.time()))
        filename = f"{timestamp}_{file.filename}"
        filepath = os.path.join('uploads', filename)

        os.makedirs('uploads', exist_ok=True)
        file.save(filepath)

        # Create interactive simulation session
        session_id = simulator_manager.create_session(filepath, interactive=True)

        # Start simulation
        success, message = simulator_manager.start_simulation(session_id)

        if not success:
            return jsonify({"error": message}), 500

        # Wait for initial READY signal
        session = simulator_manager.active_sessions[session_id]
        try:
            session['queue'].get(timeout=10)
        except queue.Empty:
            return jsonify({"error": "Timeout waiting for simulator to start"}), 500

        # Initial graphics capture
        simulator_manager._capture_graphics_state(session_id)

        return jsonify({
            "session_id": session_id,
            "status": "started",
            "message": "Interactive simulation ready",
            "graphics_enabled": True
        })

    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/step/<session_id>', methods=['POST'])
def step(session_id):
    """Execute one simulation step"""
    success, message = simulator_manager.step_simulation(session_id)

    if not success:
        return jsonify({"error": message}), 400

    output = simulator_manager.get_full_output(session_id)
    graphics_data = simulator_manager.get_graphics_data(session_id)

    return jsonify({
        "status": "success" if success else "error",
        "message": message,
        "output": output,
        "graphics": graphics_data
    })

@app.route('/graphics/<session_id>', methods=['GET'])
def get_graphics(session_id):
    """Get current graphics state"""
    graphics_data = simulator_manager.get_graphics_data(session_id)

    if not graphics_data:
        return jsonify({"error": "No graphics data available"}), 404

    return jsonify(graphics_data)

if __name__ == '__main__':
    print("Starting Z16 Simulator Backend with Graphics Support...")
    print("Frontend should connect to: http://localhost:5001")
    app.run(host='0.0.0.0', port=5001, debug=True)
