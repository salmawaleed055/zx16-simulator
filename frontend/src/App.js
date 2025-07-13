import React, { useState, useEffect, useCallback } from 'react';

// Main App component for the ZX16 Simulator Interface
function App() {
  // State variables for simulation output and file handling
  const [simOutput, setSimOutput] = useState('');
  const [selectedFile, setSelectedFile] = useState(null);
  const [fileName, setFileName] = useState('');

  // State for UI feedback and loading status
  const [loading, setLoading] = useState(false);
  const [userMessage, setUserMessage] = useState(null); // For custom alerts/messages
  const [messageType, setMessageType] = useState('info'); // 'info', 'success', 'error', 'warning'

  // State for backend health and information
  const [backendStatus, setBackendStatus] = useState('checking'); // 'checking', 'ready', 'needs-compile', 'no-compiler', 'no-source', 'offline', 'error'
  const [backendInfo, setBackendInfo] = useState(null);

  // State for step-by-step simulation
  const [processId, setProcessId] = useState(null); // ID of the active simulation process
  const [isStepping, setIsStepping] = useState(false); // True if in step-by-step mode
  const [hasSimulationEnded, setHasSimulationEnded] = useState(false); // True if simulation has completed or terminated
  const [stepCount, setStepCount] = useState(0); // Current step count in step-by-step mode

  /**
   * Displays a temporary message to the user.
   * @param {string} message - The message to display.
   * @param {string} type - The type of message ('info', 'success', 'error', 'warning').
   * @param {number} duration - How long the message should be visible in milliseconds.
   */
  const showUserMessage = (message, type = 'info', duration = 5000) => {
    setUserMessage(message);
    setMessageType(type);
    if (duration > 0) {
      setTimeout(() => {
        setUserMessage(null);
        setMessageType('info');
      }, duration);
    }
  };

  /**
   * Checks the health and status of the Flask backend.
   * Updates backendStatus and backendInfo states accordingly.
   * Wrapped in useCallback to satisfy useEffect dependency.
   */
  const checkBackendHealth = useCallback(async () => {
    try {
      const backendUrl = 'http://localhost:5001/health';
      console.log(`Attempting to fetch backend health from: ${backendUrl}`); // Added for debugging
      const response = await fetch(backendUrl);
      if (response.ok) {
        const data = await response.json();
        setBackendInfo(data);

        if (data.executable_found) {
          setBackendStatus('ready');
          showUserMessage('Backend is ready and simulator executable found.', 'success', 3000);
        } else if (data.cpp_source_found && data.compiler_available) {
          setBackendStatus('needs-compile');
          showUserMessage('Backend online. Simulator source found, but needs compilation.', 'warning', 5000);
        } else if (data.cpp_source_found && !data.compiler_available) {
          setBackendStatus('no-compiler');
          showUserMessage('Backend online. Simulator source found, but g++ compiler is missing.', 'error', 0); // Persistent error
        } else {
          setBackendStatus('no-source');
          showUserMessage('Backend online. No z16sim.cpp source file found.', 'error', 0); // Persistent error
        }
      } else {
        setBackendStatus('error');
        showUserMessage(`Backend returned an error: ${response.status} ${response.statusText}`, 'error', 0);
      }
    } catch (error) {
      setBackendStatus('offline');
      showUserMessage('Backend is offline. Please ensure the Flask server is running.', 'error', 0); // Persistent error
      console.error('Backend health check error:', error);
    }
  }, []);

  // Effect hook to check backend health when the component mounts
  useEffect(() => {
    checkBackendHealth();
  }, [checkBackendHealth]);

  /**
   * Triggers the compilation of the z16sim.cpp on the backend.
   */
  const handleCompile = async () => {
    setLoading(true);
    showUserMessage("🔨 Compiling simulator...", 'info', 0); // Persistent during compilation

    try {
      const response = await fetch('http://localhost:5001/compile', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || `Compilation failed: ${response.statusText}`);
      }

      const result = await response.json();
      showUserMessage(`✅ ${result.message}`, 'success');

      // Refresh backend status after compilation to reflect the new executable status
      setTimeout(() => {
        checkBackendHealth();
      }, 1000);

    } catch (error) {
      console.error('Compilation error:', error);
      showUserMessage(`❌ Compilation failed: ${error.message}`, 'error', 0);
    } finally {
      setLoading(false);
    }
  };

  /**
   * Handles the selection of a new binary file.
   * @param {Event} e - The file input change event.
   */
  const handleFileChange = (e) => {
    const file = e.target.files[0];
    if (!file) return;

    // Validate file extension to ensure it's a .bin file
    if (!file.name.toLowerCase().endsWith('.bin')) {
      showUserMessage('Please select a .bin file.', 'warning');
      setSelectedFile(null);
      setFileName('');
      return;
    }

    setSelectedFile(file);
    setFileName(file.name);
    setSimOutput(''); // Clear previous simulation output
    showUserMessage(`📄 Selected file: ${file.name}`, 'info');

    // Reset step-by-step simulation state when a new file is selected
    setProcessId(null);
    setIsStepping(false);
    setHasSimulationEnded(false);
    setStepCount(0);
  };

  /**
   * Initiates a simulation (full or step-by-step) on the backend.
   * @param {'full' | 'step'} mode - The simulation mode.
   */
  const handleRunSimulation = async (mode) => {
    if (!selectedFile) {
      showUserMessage("Please upload a .bin file first.", 'warning');
      return;
    }

    if (backendStatus !== 'ready') {
      showUserMessage("Backend is not ready. Please ensure the simulator executable is available or compile it.", 'warning');
      return;
    }

    // Prepare form data to send the file and mode to the backend
    const formData = new FormData();
    formData.append('binfile', selectedFile);
    formData.append('mode', mode);

    setLoading(true);
    setSimOutput(''); // Clear previous output
    showUserMessage(`⏳ Starting ${mode} simulation...`, 'info', 0); // Persistent during simulation start
    setHasSimulationEnded(false);
    setStepCount(0);

    try {
      const response = await fetch('http://localhost:5001/simulate', {
        method: 'POST',
        body: formData,
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.error || `Server error: ${response.statusText}`);
      }

      if (mode === 'full') {
        const result = await response.text(); // Full simulation returns plain text
        setSimOutput(result);
        showUserMessage("✅ Full simulation completed.", 'success');
        setHasSimulationEnded(true);
      } else if (mode === 'step') {
        const result = await response.json(); // Step-by-step returns JSON
        if (result.error) {
          throw new Error(result.error);
        }
        setProcessId(result.process_id);
        setIsStepping(true);
        // Display initial output and process ID
        setSimOutput(`🚶 Step-by-step simulation started (Process ID: ${result.process_id})\n\n${result.initial_output}`);
        showUserMessage("🎯 Ready for step-by-step execution. Click 'Next Step' to proceed.", 'info');
      }

    } catch (error) {
      console.error('Simulation error:', error);
      setSimOutput(`❌ Error: ${error.message}`);
      showUserMessage(`❌ Simulation failed: ${error.message}`, 'error');
      // Reset states on error
      setProcessId(null);
      setIsStepping(false);
      setHasSimulationEnded(true);
    } finally {
      setLoading(false);
    }
  };

  /**
   * Executes the next step in a step-by-step simulation.
   */
  const handleNextStep = async () => {
    if (!processId) return; // Ensure a process ID exists

    setLoading(true);
    showUserMessage("⏳ Executing next step...", 'info', 0); // Persistent during step

    try {
      const response = await fetch('http://localhost:5001/step_simulation', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          process_id: processId,
          command: '\n' // Send newline to signal the simulator to execute the next instruction
        }),
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.error || `Server error: ${response.statusText}`);
      }

      const result = await response.json();

      // Append new output to the existing simulation output
      // Add a separator for clarity between steps
      if (result.output && result.output.trim()) {
        setSimOutput(prevOutput =>
            prevOutput + `\n\n--- Step ${result.step_count} ---\n${result.output}`
        );
      } else {
        setSimOutput(prevOutput =>
            prevOutput + `\n\n--- Step ${result.step_count} --- (No new output)`
        );
      }

      setStepCount(result.step_count);

      if (result.terminated) {
        // If the simulation terminated after this step
        setSimOutput(prevOutput =>
            prevOutput + `\n\n🏁 Simulation completed after ${result.step_count} steps.`
        );
        showUserMessage(`✅ Simulation completed in ${result.step_count} steps.`, 'success');
        // Reset step-by-step states
        setProcessId(null);
        setIsStepping(false);
        setHasSimulationEnded(true);
      } else {
        showUserMessage(`🎯 Step ${result.step_count} completed. Ready for next step.`, 'info');
      }

    } catch (error) {
      console.error('Step execution error:', error);
      setSimOutput(prevOutput =>
          prevOutput + `\n\n❌ Error during step execution: ${error.message}`
      );
      showUserMessage(`❌ Step execution failed: ${error.message}`, 'error');
      // Reset step-by-step states on error
      setProcessId(null);
      setIsStepping(false);
      setHasSimulationEnded(true);
    } finally {
      setLoading(false);
    }
  };

  /**
   * Stops an active step-by-step simulation.
   */
  const handleStopSimulation = async () => {
    if (!processId) return; // Only stop if there's an active process

    setLoading(true);
    showUserMessage("⏳ Stopping simulation...", 'info', 0); // Persistent during stop

    try {
      const response = await fetch('http://localhost:5001/end_simulation', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ process_id: processId }),
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.error || `Server error: ${response.statusText}`);
      }

      const result = await response.json();
      setSimOutput(prevOutput =>
          prevOutput + `\n\n⏹️ ${result.status || "Simulation stopped by user"}`
      );
      showUserMessage(`⏹️ Simulation stopped after ${stepCount} steps.`, 'info');
      // Reset step-by-step states
      setProcessId(null);
      setIsStepping(false);
      setHasSimulationEnded(true);

    } catch (error) {
      console.error('Stop simulation error:', error);
      setSimOutput(prevOutput =>
          prevOutput + `\n\n❌ Error stopping simulation: ${error.message}`
      );
      showUserMessage(`❌ Error stopping simulation: ${error.message}`, 'error');
    } finally {
      setLoading(false);
    }
  };

  /**
   * Clears all simulation output and resets the application state.
   * If a step-by-step simulation is active, it attempts to stop it first.
   */
  const handleClear = async () => {
    // If there's an active step-by-step simulation, stop it gracefully
    if (isStepping && processId) {
      await handleStopSimulation();
    }

    setSimOutput('');
    setSelectedFile(null);
    setFileName('');
    showUserMessage('All cleared. Ready for new simulation.', 'info');
    setProcessId(null);
    setIsStepping(false);
    setHasSimulationEnded(false);
    setStepCount(0);
  };

  /**
   * Helper function to get a display string for backend status.
   */
  const getBackendStatusDisplay = () => {
    switch (backendStatus) {
      case 'ready':
        return '✅ Backend ready - Simulator executable found';
      case 'needs-compile':
        return '🔨 Backend online - Source found, needs compilation';
      case 'no-compiler':
        return '❌ Backend online - Source found but no g++ compiler';
      case 'no-source':
        return '❌ Backend online - No z16sim.cpp source file found';
      case 'offline':
        return '❌ Backend offline';
      case 'error':
        return '❌ Backend error';
      default:
        return '🔄 Checking backend...';
    }
  };

  /**
   * Helper function to display detailed backend information.
   */
  const getBackendDetails = () => {
    if (!backendInfo) return null;

    return (
        <div className="text-xs text-gray-600 mt-1">
          <div>Active simulations: {backendInfo.active_simulations}</div>
          <div>Source file: {backendInfo.cpp_source_found ? '✅ Found' : '❌ Not Found'}</div>
          <div>Compiler: {backendInfo.compiler_available ? '✅ Available' : '❌ Not Available'}</div>
          {backendInfo.executable_path && (
              <div>Executable: {backendInfo.executable_path}</div>
          )}
        </div>
    );
  };

  /**
   * Determines the background color for the user message based on its type.
   */
  const getMessageBackgroundColor = (type) => {
    switch (type) {
      case 'success': return 'bg-green-100 border-green-400 text-green-700';
      case 'error': return 'bg-red-100 border-red-400 text-red-700';
      case 'warning': return 'bg-yellow-100 border-yellow-400 text-yellow-700';
      case 'info':
      default: return 'bg-blue-100 border-blue-400 text-blue-700';
    }
  };

  return (
      <div className="min-h-screen bg-gray-100 flex items-center justify-center p-4 font-sans antialiased">
        <div className="w-full max-w-3xl bg-white shadow-lg rounded-xl p-6 md:p-8 border border-gray-200">
          <h2 className="text-3xl font-bold text-center text-gray-800 mb-6 flex items-center justify-center">
            <span className="mr-2">⚙️</span> ZX16 Simulator Interface
          </h2>

          {/* Backend Status Section */}
          <div className={`mb-6 p-4 rounded-lg border ${
              backendStatus === 'ready' ? 'bg-green-50 border-green-200' :
                  backendStatus === 'needs-compile' ? 'bg-yellow-50 border-yellow-200' :
                      'bg-red-50 border-red-200'
          }`}>
            <div className="font-semibold text-gray-800 mb-2">
              {getBackendStatusDisplay()}
            </div>
            {getBackendDetails()}

            <div className="mt-4 flex flex-wrap gap-2">
              {backendStatus === 'offline' && (
                  <button
                      onClick={checkBackendHealth}
                      className="px-4 py-2 text-sm bg-blue-600 text-white rounded-md hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-blue-500 focus:ring-offset-2 transition duration-200 ease-in-out shadow-sm"
                  >
                    🔄 Retry Connection
                  </button>
              )}

              {backendStatus === 'needs-compile' && (
                  <button
                      onClick={handleCompile}
                      disabled={loading}
                      className={`px-4 py-2 text-sm bg-orange-500 text-white rounded-md hover:bg-orange-600 focus:outline-none focus:ring-2 focus:ring-orange-400 focus:ring-offset-2 transition duration-200 ease-in-out shadow-sm ${loading ? 'opacity-60 cursor-not-allowed' : ''}`}
                  >
                    {loading ? '🔨 Compiling...' : '🔨 Compile Simulator'}
                  </button>
              )}
            </div>
          </div>

          {/* Custom User Message Display */}
          {userMessage && (
              <div className={`mb-6 p-4 rounded-lg border ${getMessageBackgroundColor(messageType)} font-medium text-sm transition-all duration-300 ease-in-out`}>
                {userMessage}
              </div>
          )}

          {/* File Upload Section */}
          <div className="mb-6">
            <label htmlFor="file-upload" className="block text-sm font-medium text-gray-700 mb-2">
              Upload ZX16 Binary File (.bin)
            </label>
            <input
                id="file-upload"
                type="file"
                accept=".bin"
                onChange={handleFileChange}
                disabled={backendStatus !== 'ready' || loading}
                className="block w-full text-sm text-gray-900 border border-gray-300 rounded-lg cursor-pointer bg-gray-50 file:mr-4 file:py-2 file:px-4 file:rounded-md file:border-0 file:text-sm file:font-semibold file:bg-blue-50 file:text-blue-700 hover:file:bg-blue-100"
            />
            {fileName && (
                <p className="mt-2 text-sm text-gray-600 italic">
                  📄 Selected: <span className="font-semibold text-gray-800">{fileName}</span>
                </p>
            )}
          </div>

          {/* Action Buttons Group */}
          <div className="flex flex-wrap gap-3 mb-6">
            {/* Show simulation start buttons when not stepping */}
            {!isStepping && (
                <>
                  <button
                      onClick={() => handleRunSimulation('full')}
                      disabled={loading || !selectedFile || backendStatus !== 'ready'}
                      className={`flex-1 px-6 py-3 bg-green-600 text-white font-semibold rounded-lg shadow-md hover:bg-green-700 focus:outline-none focus:ring-2 focus:ring-green-500 focus:ring-offset-2 transition duration-200 ease-in-out ${
                          (loading || !selectedFile || backendStatus !== 'ready') ? 'opacity-60 cursor-not-allowed' : ''
                      }`}
                  >
                    ▶️ {loading && !isStepping ? 'Running Full...' : 'Run Full Simulation'}
                  </button>
                  <button
                      onClick={() => handleRunSimulation('step')}
                      disabled={loading || !selectedFile || backendStatus !== 'ready'}
                      className={`flex-1 px-6 py-3 bg-blue-600 text-white font-semibold rounded-lg shadow-md hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-blue-500 focus:ring-offset-2 transition duration-200 ease-in-out ${
                          (loading || !selectedFile || backendStatus !== 'ready') ? 'opacity-60 cursor-not-allowed' : ''
                      }`}
                  >
                    🚶 {loading && !isStepping ? 'Starting Step...' : 'Run Step-by-Step'}
                  </button>
                </>
            )}

            {/* Show step control buttons when stepping */}
            {isStepping && !hasSimulationEnded && (
                <>
                  <button
                      onClick={handleNextStep}
                      disabled={loading}
                      className={`flex-1 px-6 py-3 bg-teal-600 text-white font-semibold rounded-lg shadow-md hover:bg-teal-700 focus:outline-none focus:ring-2 focus:ring-teal-500 focus:ring-offset-2 transition duration-200 ease-in-out ${loading ? 'opacity-60 cursor-not-allowed' : ''}`}
                  >
                    ➡️ {loading ? 'Stepping...' : `Next Step (${stepCount + 1})`}
                  </button>
                  <button
                      onClick={handleStopSimulation}
                      disabled={loading}
                      className={`flex-1 px-6 py-3 bg-red-600 text-white font-semibold rounded-lg shadow-md hover:bg-red-700 focus:outline-none focus:ring-2 focus:ring-red-500 focus:ring-offset-2 transition duration-200 ease-in-out ${loading ? 'opacity-60 cursor-not-allowed' : ''}`}
                  >
                    ⏹️ Stop Simulation
                  </button>
                </>
            )}

            <button
                onClick={handleClear}
                disabled={loading}
                className={`flex-1 px-6 py-3 bg-gray-600 text-white font-semibold rounded-lg shadow-md hover:bg-gray-700 focus:outline-none focus:ring-2 focus:ring-gray-500 focus:ring-offset-2 transition duration-200 ease-in-out ${loading ? 'opacity-60 cursor-not-allowed' : ''}`}
            >
              🧹 Clear
            </button>
          </div>

          {/* Simulation Output Display */}
          {simOutput && (
              <div>
                <h3 className="text-xl font-semibold text-gray-800 mb-3">📋 Simulation Output:</h3>
                <pre className="whitespace-pre-wrap break-words max-h-96 overflow-y-auto border border-gray-300 p-4 bg-gray-50 text-sm font-mono rounded-lg shadow-inner leading-relaxed">
                  {simOutput}
                </pre>
              </div>
          )}
        </div>
      </div>
  );
}

// Export the App component as default
export default App;