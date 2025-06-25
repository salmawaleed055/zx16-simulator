#include "z16sim.h"
#include <iostream>

// -----------------------
// Main Simulation Loop
// -----------------------
int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <machine_code_file>\n", argv[0]);
        exit(1);
    }

    z16sim sim;

    sim.loadMemoryFromFile(argv[1]);

    while (sim.cycle()) {}

    return 0;
}

