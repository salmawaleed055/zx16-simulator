from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess
import os
import tempfile
import platform

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

@app.route('/simulate', methods=['POST'])
def simulate():
    try:
        # Check if file was uploaded
        if 'binfile' not in request.files:
            return jsonify({'error': 'No file uploaded'}), 400

        file = request.files['binfile']

        # Check if file has a name
        if file.filename == '':
            return jsonify({'error': 'No file selected'}), 400

        # Create a temporary file to store the uploaded .bin file
        with tempfile.NamedTemporaryFile(delete=False, suffix='.bin') as temp_file:
            file.save(temp_file.name)
            temp_path = temp_file.name

        try:
            # Determine the executable name based on OS
            if platform.system() == 'Windows':
                executable = './z16sim.exe'
            else:
                executable = './z16sim'

            # Run the C++ simulator
            result = subprocess.run(
                [executable, temp_path],
                capture_output=True,
                text=True,
                timeout=30  # 30 second timeout
            )

            # Clean up the temporary file
            os.unlink(temp_path)

            if result.returncode == 0:
                # Success - return the output
                return result.stdout
            else:
                # Error in simulation
                error_msg = result.stderr if result.stderr else "Unknown simulation error"
                return jsonify({'error': f'Simulation failed: {error_msg}'}), 500

        except subprocess.TimeoutExpired:
            os.unlink(temp_path)
            return jsonify({'error': 'Simulation timed out'}), 500
        except FileNotFoundError:
            os.unlink(temp_path)
            return jsonify({'error': 'Simulator executable not found. Make sure z16sim is compiled.'}), 500
        except Exception as e:
            os.unlink(temp_path)
            return jsonify({'error': f'Error running simulator: {str(e)}'}), 500

    except Exception as e:
        return jsonify({'error': f'Server error: {str(e)}'}), 500

@app.route('/health', methods=['GET'])
def health_check():
    return jsonify({'status': 'Backend is running', 'message': 'ZX16 Simulator Backend'})

if __name__ == '__main__':
    print("🚀 Starting ZX16 Simulator Backend...")
    print("📁 Make sure z16sim executable is in the same directory")
    print("🌐 Backend will run on http://localhost:5001")
    app.run(debug=True, host='0.0.0.0', port=5001)