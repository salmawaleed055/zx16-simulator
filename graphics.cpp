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
    screenNeedsUpdate = true;
    window.setFramerateLimit(60);
    
    // Create texture for screen
    screenTexture.create(320, 240);
    screenSprite.setTexture(screenTexture);

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
    
    graphicsInitialized = true;

    std::cout << "SFML Graphics initialized: 320x240 display" << std::endl;
}

void Graphics::onGraphicsMemoryWrite(uint16_t addr, uint8_t value) {
    if (!graphicsMemoryAccessed) {
        graphicsMemoryAccessed = true;
        initGraphics();
    }
    
    // Any write to graphics memory triggers screen update
    if ((addr >= TILE_MAP_START && addr <= TILE_MAP_END) ||
        (addr >= TILE_DATA_START && addr <= TILE_DATA_END) ||
        (addr >= PALETTE_START && addr <= PALETTE_END)) {
        
        screenNeedsUpdate = true;
        std::cout << "Graphics memory updated at 0x" << std::hex << addr 
                  << " with value 0x" << (int)value << std::dec << std::endl;
    }
}

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
    
    return sf::Color(r, g, b);
}



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