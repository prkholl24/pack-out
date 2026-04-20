/**
 * @file main.cpp
 * @brief Application entry point for Pack-Out.
 *
 * Creates the Qt event loop, constructs the MainWindow (which owns
 * the scene stack and all game scenes), shows it, and runs until the
 * user closes the window. All game logic lives inside the scene
 * widgets — this file is intentionally minimal.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 */
#include "mainwindow.h"
#include "spritecache.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // QApplication initializes the Qt framework (event handling,
    // platform integration, fonts, styles). Must be created before
    // any QWidget.
    QApplication a(argc, argv);

    // Load sprite images for all items from Qt resources (:/sprites/).
    SpriteCache::init();

    // MainWindow is the top-level window. Its constructor builds
    // a QStackedWidget and populates it with game scenes.
    MainWindow w;
    w.show();

    // Enters the Qt event loop. Everything beyond this point —
    // painting, timers, mouse events — is driven by the loop.
    // exec() returns when the last window is closed.
    return a.exec();
}
