const { execFile } = require('child_process');
const path = require('path');
const fs = require('fs');

// Test the simulator directly
const simulatorPath = path.join(__dirname, 'zx16_simulator.exe');
const testFilePath = path.join(__dirname, 'TC-ZX16-01_ADD.bin'); // Adjust path as needed

console.log('Testing ZX16 Simulator...');
console.log('Simulator path:', simulatorPath);
console.log('Test file path:', testFilePath);

// Check if files exist
console.log('Simulator exists:', fs.existsSync(simulatorPath));
console.log('Test file exists:', fs.existsSync(testFilePath));

if (!fs.existsSync(simulatorPath)) {
    console.error('❌ Simulator executable not found!');
    console.log('Make sure zx16_simulator.exe is in the same directory as this script');
    process.exit(1);
}

if (!fs.existsSync(testFilePath)) {
    console.error('❌ Test file not found!');
    console.log('Make sure TC-ZX16-01_ADD.bin is in the same directory as this script');
    console.log('Or update the testFilePath variable to point to your test file');
    process.exit(1);
}

// Try to run the simulator
console.log('\n🚀 Running simulator...');

const startTime = Date.now();

execFile(simulatorPath, [testFilePath], {
    timeout: 10000, // 10 second timeout
    maxBuffer: 1024 * 1024, // 1MB buffer
    cwd: __dirname
}, (error, stdout, stderr) => {
    const endTime = Date.now();
    const duration = endTime - startTime;

    console.log(`\n⏱️  Execution took ${duration}ms`);

    if (error) {
        console.error('❌ Error running simulator:');
        console.error('Error code:', error.code);
        console.error('Error signal:', error.signal);
        console.error('Error message:', error.message);

        if (stderr) {
            console.error('\n📢 Stderr output:');
            console.error(stderr);
        }

        if (stdout) {
            console.log('\n📄 Stdout output:');
            console.log(stdout);
        }

        process.exit(1);
    }

    console.log('✅ Simulator executed successfully!');

    if (stderr) {
        console.log('\n📢 Stderr output:');
        console.log(stderr);
    }

    if (stdout) {
        console.log('\n📄 Stdout output:');
        console.log(stdout);
    } else {
        console.log('\n⚠️  No stdout output received');
    }
});