#include "z16sim.h"
#include <iostream>
#include <vector>
#include <iomanip> // For std::hex, std::setw, std::setfill

// Main entry point for the Z16 simulator executable
int main(int argc, char* argv[])
    {
    // Check for correct command-line usage
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <machine_code_file_name.bin>" << std::endl;
        return 1; // Indicate error
    }

    z16sim simulator; // Create an instance of the simulator

    // Load the machine code binary from the specified file
    simulator.loadMemoryFromFile(argv[1]);

    // Run the simulation cycle by cycle
    // The cycle() method already prints the disassembly and execution trace
    while (simulator.cycle()) {
        // Continue simulation as long as cycle() returns true
        // (i.e., no termination ecall or error)
    }

    // Final register state (for debugging/verification if needed)
    std::cout << "\n--- Final State ---" << std::endl;
    for (int i = 0; i < z16sim::NUM_REGS; ++i) { // Use z16sim::NUM_REGS
        std::cout << z16sim::regNames[i] << ": 0x" // Use z16sim::regNames
                  << std::hex << std::setw(4) << std::setfill('0')
                  << simulator.getReg(i) << std::endl; // Use getReg()
    }
    std::cout << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0') << simulator.getPC() << std::endl;
    std::cout << "---------------------\n" << std::endl;

    std::cout << "Simulation finished." << std::endl;
    return 0; // Indicate success
}