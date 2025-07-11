#include "graphics.h"


void Graphics::initGraphics() {
    std::cout << "HEREEEEE AT BEGINNING OF INITGRAPHICS" << std::endl;
    if (graphicsInitialized) return;

    // Create window
    window.create(sf::VideoMode(640, 480), "ZX16 Simulator");
    window.setFramerateLimit(60);
    
    // Create texture for screen
    screenTexture.create(320, 240);
    screenSprite.setTexture(screenTexture);
    screenSprite.setScale(2.0f, 2.0f); // 2x scale for visibility
    
    // Initialize default palette
    colorPalette[0] = 0x00;   // Black
    colorPalette[1] = 0x1C;   // Red
    colorPalette[2] = 0xE0;   // Green
    colorPalette[3] = 0xFC;   // Yellow
    colorPalette[4] = 0x03;   // Blue
    colorPalette[5] = 0x1F;   // Magenta
    colorPalette[6] = 0xE3;  // Cyan
    colorPalette[7] = 0xFF;   // White
    
    graphicsInitialized = true;

    std::cout << "SFML Graphics initialized: 320x240 display (scaled 2x)" << std::endl;
}


void Graphics::updateGraphicsMemory(uint16_t addr, uint8_t value) {
    std::cout << "HEEEREE AT BEGINNING OF UPDATEGRAPHICSMEMORY" << std::endl;
    if (!graphicsMemoryAccessed) {
        graphicsMemoryAccessed = true;
        std::cout << "haallloooooo" << std::endl;
        initGraphics();
    }
        if (addr >= 0xF000 && addr <= 0xF12B) {
        // if (addr >= 0x0000 && addr <= 0x012B) {
        // Tile map update
        tileMap[addr - 0xF000] = value;
        std::cout << "tile map updateee" << std::endl;
        // tileMap[addr - 0x0000] = value; 
        screenNeedsUpdate = true;
    }


    else if (addr >= 0xF200 && addr <= 0xF9FF) {
    // else if (addr >= 0x012C && addr <= 0x092B) {
        // Tile data update
        std::cout << "tile data updateee" << std::endl;
        int tileIndex = (addr - 0xF200) / 128;
        int byteOffset = (addr - 0xF200) % 128;
        // int tileIndex = (addr - 0x012C) / 128;
        // int byteOffset = (addr - 0x012C) % 128;
        if (tileIndex < 16) {
            tileData[tileIndex][byteOffset] = value;
            screenNeedsUpdate = true;
        }
    }
    else if (addr >= 0xFA00 && addr <= 0xFA0F) {
    // else if (addr >= 0x092C && addr <= 0x093B) {
        // Color palette update
        std::cout << "color palette updateee" << std::endl;
        colorPalette[addr - 0xFA00] = value;
        // colorPalette[addr - 0x092C] = value;
        screenNeedsUpdate = true;
    }

    std::cout << "Writing value " << std::hex << (int)value
          << " to graphics address 0x" << addr << std::dec << std::endl;

    if (screenNeedsUpdate) renderScreen();
}


sf::Color Graphics::paletteToColor(uint8_t colorIndex) {
    if (colorIndex >= 16) colorIndex = 0;
    
    uint8_t colorByte = colorPalette[colorIndex];
    uint8_t r = ((colorByte >> 5) & 0x7) * 36;  // 3 bits -> 0-255
    uint8_t g = ((colorByte >> 2) & 0x7) * 36;  // 3 bits -> 0-255
    uint8_t b = (colorByte & 0x3) * 85;         // 2 bits -> 0-255
    
    return sf::Color(r, g, b);
}


void Graphics::renderTile(int tileIndex, int screenX, int screenY) {
    if (tileIndex >= 16) return;
    
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x += 2) {
            int byteIndex = y * 8 + x / 2;
            uint8_t pixelPair = tileData[tileIndex][byteIndex];
            
            uint8_t pixel0Color = pixelPair & 0x0F;
            uint8_t pixel1Color = (pixelPair >> 4) & 0x0F;
            
            sf::Color color0 = paletteToColor(pixel0Color);
            sf::Color color1 = paletteToColor(pixel1Color);
            
            // Set pixels in frame buffer
            int fbIndex0 = ((screenY + y) * 320 + (screenX + x)) * 4;
            int fbIndex1 = ((screenY + y) * 320 + (screenX + x + 1)) * 4;
            
            if (fbIndex0 < 320 * 240 * 4) {
                frameBuffer[fbIndex0] = color0.r;
                frameBuffer[fbIndex0 + 1] = color0.g;
                frameBuffer[fbIndex0 + 2] = color0.b;
                frameBuffer[fbIndex0 + 3] = 255;
            }
            
            if (fbIndex1 < 320 * 240 * 4) {
                frameBuffer[fbIndex1] = color1.r;
                frameBuffer[fbIndex1 + 1] = color1.g;
                frameBuffer[fbIndex1 + 2] = color1.b;
                frameBuffer[fbIndex1 + 3] = 255;
            }
        }
    }
}


void Graphics::renderScreen() {
    // std::cout << "IN RENDER SCREEN" << std::endl;
    if (!screenNeedsUpdate) return;
    
    // Clear frame buffer
    memset(frameBuffer, 0, sizeof(frameBuffer));
    
    // Render each tile position
    for (int row = 0; row < 15; row++) {
        for (int col = 0; col < 20; col++) {
            int tileMapIndex = row * 20 + col;
            uint8_t tileIndex = tileMap[tileMapIndex];
            renderTile(tileIndex, col * 16, row * 16);
        }
    }
    
    // Update SFML texture
    screenTexture.update(frameBuffer);
    screenNeedsUpdate = false;

    std::cout << "RENDERED SCREEN" << std::endl;
}


bool Graphics::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            return false;
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                return false;
            }
        }
    }
    return true;
}


void Graphics::cleanup() {
    if (window.isOpen()) {
        window.close();
    }
}


bool Graphics::needsGraphics() const {
    std::cout << "i am in needsGraphics" << std::endl;
    std::cout << "graphicsMemoryAccessed: " << graphicsMemoryAccessed << std::endl;
    return graphicsMemoryAccessed;
}