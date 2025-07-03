const express = require('express');
const multer = require('multer');
const cors = require('cors');
const { execFile } = require('child_process');
const path = require('path');
const fs = require('fs');

const app = express();
const upload = multer({ dest: 'uploads/' });

// Middleware
app.use(cors());
app.use(express.json());

// Ensure uploads directory exists
const uploadsDir = path.join(__dirname, 'uploads');
if (!fs.existsSync(uploadsDir)) {
    fs.mkdirSync(uploadsDir, { recursive: true });
}

// Routes
app.get('/health', (req, res) => {
    const simulatorPath = path.join(__dirname, 'zx16_simulator.exe');
    console.log('Health check requested');
    res.json({
        status: 'ok',
        timestamp: new Date().toISOString(),
        cwd: __dirname,
        simulatorExists: fs.existsSync(simulatorPath),
        simulatorPath: simulatorPath
    });
});

app.get('/test', (req, res) => {
    console.log('Test endpoint hit');
    res.json({ message: 'Server is working perfectly!' });
});

app.post('/simulate', upload.single('binfile'), (req, res) => {
    console.log('=== SIMULATE ENDPOINT HIT ===');
    console.log('req.file:', req.file);

    if (!req.file) {
        console.log('No file uploaded');
        return res.status(400).json({ error: 'No file uploaded' });
    }

    const uploadedFilePath = req.file.path;
    const simulatorPath = path.join(__dirname, 'zx16_simulator.exe');

    console.log('Uploaded file path:', uploadedFilePath);
    console.log('Simulator path:', simulatorPath);

    // Check if simulator exists
    if (!fs.existsSync(simulatorPath)) {
        console.log('Simulator not found');
        // Clean up uploaded file
        fs.unlink(uploadedFilePath, (unlinkErr) => {
            if (unlinkErr) console.error('Failed to delete uploaded file:', unlinkErr);
        });
        return res.status(500).json({ error: 'Simulator executable not found' });
    }

    console.log('Starting simulator execution...');

    execFile(simulatorPath, [uploadedFilePath], {
        timeout: 30000,
        maxBuffer: 1024 * 1024
    }, (error, stdout, stderr) => {
        console.log('Simulator execution completed');

        // Always clean up uploaded file
        fs.unlink(uploadedFilePath, (unlinkErr) => {
            if (unlinkErr) console.error('Failed to delete uploaded file:', unlinkErr);
        });

        if (error) {
            console.error('Simulator error:', error);
            console.error('stderr:', stderr);
            console.error('stdout:', stdout);

            return res.status(500).json({
                error: `Simulator error: ${stderr || error.message || 'Unknown error'}`,
                debug: {
                    stdout: stdout || '',
                    stderr: stderr || '',
                    errorCode: error.code,
                    errorSignal: error.signal
                }
            });
        }

        console.log('Simulator output:', stdout);
        res.json({
            success: true,
            output: stdout || 'No output received',
            stderr: stderr || ''
        });
    });
});

// Global error handler
app.use((error, req, res, next) => {
    console.error('Global error handler:', error);
    res.status(500).json({
        error: 'Internal server error',
        message: error.message
    });
});

const PORT = process.env.PORT || 5000;
app.listen(PORT, () => {
    console.log(`✅ ZX16 simulator backend running at http://localhost:${PORT}`);
    console.log(`📁 Working directory: ${__dirname}`);
    console.log(`🔧 Simulator path: ${path.join(__dirname, 'zx16_simulator.exe')}`);
    console.log(`📂 Uploads directory: ${uploadsDir}`);
    console.log(`🌐 Available endpoints:`);
    console.log(`   GET  /health   - Health check`);
    console.log(`   GET  /test     - Test endpoint`);
    console.log(`   POST /simulate - Run simulation`);
});
app.get('/health', (req, res) => {
    res.send('Server is healthy');
});

app.get('/test', (req, res) => {
    res.send('Test endpoint works');
});
