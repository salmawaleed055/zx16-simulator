#include "graphics.h"

Graphics::Graphics(Memory* memory) {
    this->memory = memory;

    if (memory) {
        memory->registerMMIOCallback(0xF000, 0xFA0F, 
            [this](uint16_t addr, uint8_t value) {
                this->onGraphicsMemoryWrite(addr, value);
            });
    }
    
}

void Graphics::initGraphics() {
    std::cout << "HEREEEEE AT BEGINNING OF INITGRAPHICS" << std::endl;
    if (graphicsInitialized) return;

    // Create window
    window.create(sf::VideoMode(320, 240), "ZX16 Simulator");
<<<<<<< HEAD
    screenNeedsUpdate = true;
=======
>>>>>>> origin/Final_front_end
    window.setFramerateLimit(60);

    // Create texture for screen
    screenTexture.create(320, 240);
    screenSprite.setTexture(screenTexture);
<<<<<<< HEAD

    screen.create(320, 240, sf::Color::Black);
    
    // // Initialize default palette
    // colorPalette[0] = 0x00;   // Black
    // colorPalette[1] = 0x1C;   // Red
    // colorPalette[2] = 0xE0;   // Green
    // colorPalette[3] = 0xFC;   // Yellow
    // colorPalette[4] = 0x03;   // Blue
    // colorPalette[5] = 0x1F;   // Magenta
    // colorPalette[6] = 0xE3;  // Cyan
    // colorPalette[7] = 0xFF;   // White
    
=======
    //screenSprite.setScale(2.0f, 2.0f); // 2x scale for visibility

    // Initialize default palette
    colorPalette[0] = 0x00;   // Black
    colorPalette[1] = 0x1C;   // Red
    colorPalette[2] = 0xE0;   // Green
    colorPalette[3] = 0xFC;   // Yellow
    colorPalette[4] = 0x03;   // Blue
    colorPalette[5] = 0x1F;   // Magenta
    colorPalette[6] = 0xE3;  // Cyan
    colorPalette[7] = 0xFF;   // White

>>>>>>> origin/Final_front_end
    graphicsInitialized = true;

    std::cout << "SFML Graphics initialized: 320x240 display" << std::endl;
}

void Graphics::onGraphicsMemoryWrite(uint16_t addr, uint8_t value) {
    if (!graphicsMemoryAccessed) {
        graphicsMemoryAccessed = true;
        initGraphics();
    }
<<<<<<< HEAD
    
    // Any write to graphics memory triggers screen update
    if ((addr >= TILE_MAP_START && addr <= TILE_MAP_END) ||
        (addr >= TILE_DATA_START && addr <= TILE_DATA_END) ||
        (addr >= PALETTE_START && addr <= PALETTE_END)) {
        
=======
        if (addr >= 0xF000 && addr <= 0xF12B) {
        // if (addr >= 0x0000 && addr <= 0x012B) {
        // Tile map update
        tileMap[addr - 0xF000] = value;
        std::cout << "tile map updateee" << std::endl;
        // tileMap[addr - 0x0000] = value;
>>>>>>> origin/Final_front_end
        screenNeedsUpdate = true;
        std::cout << "Graphics memory updated at 0x" << std::hex << addr 
                  << " with value 0x" << (int)value << std::dec << std::endl;
    }
}

<<<<<<< HEAD
void Graphics::setNeedsUpdate() {
    screenNeedsUpdate= true;
}


void Graphics::update() {
    if (!graphicsInitialized)
        return;


    handleEvents();

    if (screenNeedsUpdate) {
        renderScreen();
        screenNeedsUpdate = false;
    }

    
}



sf::Color Graphics::paletteToColor(uint8_t paletteVal) {
    uint8_t r = ((paletteVal >> 5) & 0x7) * 36;  // 3 bits -> 0-255
    uint8_t g = ((paletteVal >> 2) & 0x7) * 36;  // 3 bits -> 0-255
    uint8_t b = (paletteVal & 0x3) * 85;         // 2 bits -> 0-255
    
=======

    else if (addr >= 0xF200 && addr <= 0xF9FF) {
    // else if (addr >= 0x012C && addr <= 0x092B) {
        // Tile data update
        std::cout << "tile data updateee" << std::endl;
        // int tileIndex = (addr - 0xF200) / 128;
         int byteOffset = (addr - 0xF200) % 128;
         int tileIndex = (addr - 0xF200) / 128;
        // int byteOffset = (addr - 0xF200) % 128;
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

>>>>>>> origin/Final_front_end
    return sf::Color(r, g, b);
}


<<<<<<< HEAD

void Graphics::renderScreen() {
    if (!memory) {
        std::cerr << "No Memory for Graphics Rendering!" << std::endl;
        return;
    }

    // screen.create(320, 240, sf::Color::Black);
    for (int y = 0; y < 240; y++) {
        for (int x = 0; x < 320; x++) {
            screen.setPixel(x, y, sf::Color::Black);
        }
    }
    
    sf::Color palette[16];
    for (int i = 0; i < 16; i++) {
        uint8_t paletteVal = memory->load(0xFA00 + i);
        palette[i] = paletteToColor(paletteVal);
    }


    // Render each tile
    for (int row = 0; row < 15; row++) {
        for (int col = 0; col < 20; col++) {
            uint16_t tileMapIndex = 0xF000 + row * 20 + col;
            uint8_t tileIndex;
            
            try {
                tileIndex = memory->load(tileMapIndex);
            }
            catch (...) {
                std::cerr << "Exception occurred while rendering tile" << std::endl;
                continue;
            }

            uint16_t tileStartAddr = 0xF200 + tileIndex * 128;

            // Render each pixel in tile
            for (int pixel_row = 0; pixel_row < 16; pixel_row++) {
                for (int pixel_col = 0; pixel_col < 16; pixel_col++) {

                    int pixelIndex = pixel_row * 16 + pixel_col;
                    int byteOffset = pixelIndex / 2; // 1 byte = 2 pixels

                    uint8_t packed;

                    try {
                        packed = memory->load(tileStartAddr + byteOffset);
                    }
                    catch (...) {
                        std::cerr << "Exception occurred while rendering tile" << std::endl;
                        continue;
                    }

                    uint8_t colorIndex;
                    if (pixelIndex % 2 == 0) // even pixel
                        colorIndex = packed & 0x0F; // low 4 bits
                    else
                        colorIndex = (packed >> 4) & 0x0F;

                    
                    sf::Color color = palette[colorIndex];

                    // get the coordinates on the screen
                    int screenY = row * 16 + pixel_row;
                    int screenX = col * 16 + pixel_col;


                    // set pixel on screen
                    if (screenX < 320 && screenY < 240) // width and height
                        screen.setPixel(screenX, screenY, color);
                }
            }
=======
void Graphics::renderTile(int tileIndex, int screenY, int screenX) {
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
    if (!screenNeedsUpdate) return;

    // Clear frame buffer
    memset(frameBuffer, 0, sizeof(frameBuffer));

    // Render each tile position
    for (int row = 0; row < 15; row++) {
        for (int col = 0; col < 20; col++) {
            int tileMapIndex = row * 20 + col;
            uint8_t tileIndex = tileMap[tileMapIndex];
            // Corrected parameter order: Y first, then X
            renderTile(tileIndex, row * 16, col * 16);
>>>>>>> origin/Final_front_end
        }
    }

    // Update SFML texture
    screenTexture.loadFromImage(screen);
    window.clear();
    window.draw(screenSprite);
    window.display();
    // screenNeedsUpdate = false;

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