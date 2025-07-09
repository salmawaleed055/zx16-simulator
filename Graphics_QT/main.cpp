#include "z16sim.h"
#include <iostream>
#include <iomanip>
#include <QApplication>
#include <QLabel>
#include <QImage>
#include <QPixmap>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    constexpr int SCREEN_WIDTH = 320;
    constexpr int SCREEN_HEIGHT = 240;

    // Create simulator instance
    z16sim sim;

    // Load binary program (Pong)
    sim.loadMemoryFromFile("D:/QT/Z16/Test_02.bin");

    // Initialize palette (before rendering)
    for (int i = 0; i < 16; ++i) {
        sim.memory[0xFA00 + i] = (i << 1);  // bright enough RGB values
    }

    // Optionally simulate a few cycles before rendering (for pong to draw)
    for (int i = 0; i < 10000; ++i) {
        if (!sim.cycle()) break;
    }

    // Render using simulator's internal memory
    QImage screenImage = sim.renderTiledScreen();

    // Display the rendered image
    QLabel label;
    label.setPixmap(QPixmap::fromImage(screenImage));
    label.setFixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    label.show();

    return app.exec();
}
