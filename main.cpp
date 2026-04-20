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
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QScreen>
#include <QTextStream>

namespace {

/// Loads a custom font from :/fonts/ and sets it as the default
/// application font. Falls back to Qt's default if the font fails
/// to load (e.g. resource missing).
void installApplicationFont(QApplication& app)
{
    const QStringList candidates = {
        ":/fonts/Quicksand-VariableFont_wght.ttf",
        ":/fonts/Nunito-VariableFont.ttf",
    };
    for (const QString& path : candidates) {
        int id = QFontDatabase::addApplicationFont(path);
        if (id < 0) continue;
        const QStringList families = QFontDatabase::applicationFontFamilies(id);
        if (families.isEmpty()) continue;
        QFont font(families.first());
        font.setPointSize(14);
        app.setFont(font);
        return;
    }
}

/// Loads :/styles.qss and applies it as the application-wide stylesheet.
void installApplicationStylesheet(QApplication& app)
{
    QFile f(":/styles.qss");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&f);
    app.setStyleSheet(in.readAll());
}

} // namespace

int main(int argc, char *argv[])
{
    // Force Qt's DPI scale factor to floor to the nearest integer,
    // BEFORE constructing QApplication. Without this, Windows display
    // scaling above 100% (e.g. 125%, 150%) shrinks Qt's logical screen
    // size (a 1920×1200 display at 125% becomes 1536×960 logical),
    // which clips the pixel-perfect 1920×1080 layouts. Floor collapses
    // 1.25 → 1.0 and 1.5 → 1.0, so Qt treats the full physical pixel
    // area as available. Must be called before any QGuiApplication or
    // QApplication instance exists.
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::Floor);

    QApplication a(argc, argv);

    installApplicationFont(a);
    installApplicationStylesheet(a);

    SpriteCache::init();

    MainWindow w;

    // Pick a display mode based on logical screen size:
    //   – Clearly larger than 1920×1080 (e.g. 1440p, 4K at low DPI):
    //     show as a fixed 1920×1080 centered window.
    //   – Anything that fits tightly (exactly 1080p, 1920×1200, or any
    //     scaled display where the logical area ≤ 1920×1200):
    //     go fullscreen so the taskbar is hidden.
    const QRect screenRect = QGuiApplication::primaryScreen()->geometry();
    const bool largeEnoughForWindowed =
        screenRect.width() > 1920 && screenRect.height() > 1200;

    if (largeEnoughForWindowed) {
        w.setFixedSize(1920, 1080);
        const QRect avail = QGuiApplication::primaryScreen()->availableGeometry();
        w.move(avail.center() - QPoint(1920 / 2, 1080 / 2));
        w.show();
    } else {
        w.showFullScreen();
    }

    return a.exec();
}
