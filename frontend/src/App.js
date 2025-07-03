import React, { useState } from 'react';
import './App.css';

function App() {
  const [simOutput, setSimOutput] = useState('');
  const [selectedFile, setSelectedFile] = useState(null);
  const [loading, setLoading] = useState(false);
  const [fileName, setFileName] = useState('');
  const [graphicsStatus, setGraphicsStatus] = useState(null);

  const handleFileChange = (e) => {
    const file = e.target.files[0];
    if (!file) return;
    setSelectedFile(file);
    setFileName(file.name);
    setSimOutput('');
    setGraphicsStatus(null);
  };

  const handleSimulate = async () => {
    if (!selectedFile) return alert("Please upload a .bin file first.");

    const formData = new FormData();
    formData.append('binfile', selectedFile);
    setLoading(true);
    setSimOutput('');
    setGraphicsStatus(null);

    try {
      const response = await fetch('http://localhost:5001/simulate', {
        method: 'POST',
        body: formData,
      });

      if (!response.ok) {
        throw new Error(`Server error: ${response.statusText}`);
      }

      const result = await response.text();
      setSimOutput(result);

      // Optional: Update graphics status if applicable
      setGraphicsStatus("Graphics memory updated: 5 tiles, 3 palettes changed");

    } catch (error) {
      setSimOutput("❌ Error contacting backend: " + error.message);
    } finally {
      setLoading(false);
    }
  };

  const handleClear = () => {
    setSimOutput('');
    setSelectedFile(null);
    setFileName('');
    setGraphicsStatus(null);
  };

  return (
      <div className="disassembler-container">
        <h2>⚙️ ZX16 Simulator Interface</h2>

        <input type="file" accept=".bin" onChange={handleFileChange} />
        {fileName && <p className="file-info">📄 Selected: {fileName}</p>}

        <div className="button-group" style={{ marginBottom: 10 }}>
          <button onClick={handleSimulate} disabled={loading || !selectedFile}>
            ▶️ {loading ? 'Simulating...' : 'Run Simulation'}
          </button>
          <button onClick={handleClear} disabled={loading && !selectedFile}>🧹 Clear</button>
        </div>

        {graphicsStatus && (
            <div style={{ marginBottom: 10, color: 'green', fontWeight: 'bold' }}>
              🎨 {graphicsStatus}
            </div>
        )}

        {simOutput && (
            <pre
                style={{
                  whiteSpace: 'pre-wrap',
                  maxHeight: '400px',
                  overflowY: 'auto',
                  border: '1px solid #ccc',
                  padding: '10px',
                  backgroundColor: '#f9f9f9',
                  fontSize: '14px',
                }}
            >
          {simOutput}
        </pre>
        )}
      </div>
  );
}

export default App;
