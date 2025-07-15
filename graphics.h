#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <iostream>
#include <cstdio>
#include <cstring>
#include "Memory.h"

class Graphics {
public:

    Graphics(Memory* memory);

    Memory* memory;
    sf::Texture screenTexture;
    sf::Uint8 frameBuffer[320 * 240 * 4]; // RGBA pixels
    sf::Image screen;
    // uint8_t tileMap[300];                  // 20x15 tiles
    // uint8_t tileData[16][128];             // 16 tiles, 128 bytes each
    // uint8_t colorPalette[16];              // 16 colors
    bool screenNeedsUpdate = false;
    bool graphicsInitialized = false;
    bool graphicsMemoryAccessed = false;
    bool needUpdate = false;

    static const uint16_t TILE_MAP_START = 0xF000;
    static const uint16_t TILE_MAP_END = 0xF12B;
    static const uint16_t TILE_DATA_START = 0xF200;
    static const uint16_t TILE_DATA_END = 0xF9FF;
    static const uint16_t PALETTE_START = 0xFA00;
    static const uint16_t PALETTE_END = 0xFA0F;
    

    void initGraphics();
    void update();
    void updateGraphicsMemory(uint16_t addr, uint8_t value);
    void renderScreen();
    void renderTile(int tileIndex, int screenX, int screenY);
    sf::Color paletteToColor(uint8_t colorIndex);
    void cleanup();
    bool handleEvents();
    bool needsGraphics() const;
    sf::RenderWindow window;
    sf::Sprite screenSprite;
<<<<<<< HEAD
    void setNeedsUpdate();

    void onGraphicsMemoryWrite(uint16_t addr, uint8_t value);
    
=======

>>>>>>> origin/Final_front_end
};





#endif // GRAPHICS_H