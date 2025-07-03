// #ifndef Z16SIM_H
// #define Z16SIM_H

// #include <cstdint>
// #include <string>
// #include <unordered_map>
// #include <vector>

// class z16sim {
// private:
//     // Constants
//     static const int MEM_SIZE = 65536;
//     static const int NUM_REGS = 8;
//     static const int RA_REG = 1; // ra register index

//     // Simulator state
//     uint16_t regs[NUM_REGS];
//     uint16_t pc;
//     unsigned char memory[MEM_SIZE];
//     bool debug;

//     // Register name mappings
//     static const char* regNames[NUM_REGS];
//     std::unordered_map<std::string, int> regMap;

//     // Helper methods
//     void initializeRegisterMap();
//     int getRegisterIndex(const std::string& regName);

//     bool updatePC(uint16_t new_pc, const char* instruction_name);

// public:
//     z16sim();
//     void dumpRegisters() const;
//     void loadMemoryFromFile(const char* filename); // For binary files
//     bool cycle();
//     int executeInstruction(uint16_t inst);
//     void reset();
//     void disassemble(uint16_t inst, uint16_t current_pc, char *buf, size_t bufSize);
//     uint16_t getPC() const { return pc; }
//     void setDebug(bool d) { debug = d; }


// };

// #endif // Z16SIM_H


#ifndef Z16SIM_H
#define Z16SIM_H

//#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <cstdint>
#define TILE_ROWS 15
#define TILE_COLS 20

class z16sim {
public:
    static const int MEM_SIZE = 65536;
    static const int NUM_REGS = 8;

    z16sim();
    void loadMemoryFromFile(const char* filename);
    bool cycle();
    int executeInstruction(uint16_t inst);
    void dumpRegisters() const;
    void reset();
    uint16_t getPC() const { return pc; }
    std::pair<char, char> getTileColor(uint8_t paletteByte);
    // Graphics methods
    void initGraphics();
    void updateGraphicsMemory(uint16_t addr, uint8_t value);
    void renderScreen();
    void renderTile(int tileIndex, int screenX, int screenY);
    char pixelToChar(uint8_t color);
   // sf::Color paletteToColor(uint8_t colorIndex);
    void cleanup();
    bool handleEvents();
    bool needsGraphics() const { return graphicsMemoryAccessed; }


    // Public for main loop access
   // sf::RenderWindow window;
   // sf::Sprite screenSprite;

private:
    unsigned char memory[MEM_SIZE];
    uint16_t regs[NUM_REGS];
    uint16_t pc;
    bool debug;
    static const char* regNames[NUM_REGS];
    std::unordered_map<std::string, int> regMap;

    // Graphics members
    //sf::Texture screenTexture;

    //sf::Uint8 frameBuffer[320 * 240 * 4]; // RGBA pixels
    uint8_t tileMap[300];                  // 20x15 tiles
    uint8_t tileData[16][128];             // 16 tiles, 128 bytes each
    uint8_t colorPalette[16];              // 16 colors
    bool screenNeedsUpdate;
    bool graphicsInitialized;
    bool graphicsMemoryAccessed;

    void initializeRegisterMap();
    int getRegisterIndex(const std::string& regName);
    bool updatePC(uint16_t new_pc, const char* instruction_name);
    static void disassemble(uint16_t inst, uint16_t current_pc, char *buf, size_t bufSize);
};

#endif

