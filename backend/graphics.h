#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <iostream>
#include <cstdio>
#include <cstring>

class Graphics {
public:

    Graphics(){};

    sf::Texture screenTexture;
    sf::Uint8 frameBuffer[320 * 240 * 4]; // RGBA pixels
    uint8_t tileMap[300];                  // 20x15 tiles
    uint8_t tileData[16][128];             // 16 tiles, 128 bytes each
    uint8_t colorPalette[16];              // 16 colors
    bool screenNeedsUpdate;
    bool graphicsInitialized;
    bool graphicsMemoryAccessed;

    void initGraphics();
    void updateGraphicsMemory(uint16_t addr, uint8_t value);
    void renderScreen();
    void renderTile(int tileIndex, int screenX, int screenY);
    sf::Color paletteToColor(uint8_t colorIndex);
    void cleanup();
    bool handleEvents();
    bool needsGraphics() const;
    sf::RenderWindow window;
    sf::Sprite screenSprite;

};





#endif // GRAPHICS_H