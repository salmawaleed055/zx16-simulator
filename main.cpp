#include "z16sim.h"
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>

int main(int argc, char **argv) {
    z16sim sim;
    bool debug = false;

    printf("If you only want to see the output without the instructions, press 1, otherwise; press 0\n");
    char option;
    scanf(" %c", &option);
    while (option != '0' && option != '1') {
        printf("Invalid option, press 0 or 1\n");
        scanf(" %c", &option);
    }

    if (option == '0') {
        printf("Do you want to debug? Press 1 if you want to see the register changes at each instruction, otherwise; press 0\n");
        char debug_option;
        scanf(" %c", &debug_option);
        while (debug_option != '0' && debug_option != '1') {
            printf("Invalid option, press 0 or 1\n");
            scanf(" %c", &debug_option);
        }
        if (debug_option == '1') {
            debug = true;
        }
    }

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <machine_code_file>\n", argv[0]);
        return 1;
    }

    sim.reset();
    sim.loadMemoryFromFile(argv[1]);

    while (sim.cycle()) {
        if (debug) {
            printf("Registers: ");
            for (int i = 0; i < z16sim::NUM_REGS; ++i) {
                printf("%s=0x%04X ", z16sim::regNames[i], sim.getReg(i));
            }
            printf("\n");
        }
    }

    return 0;
}
