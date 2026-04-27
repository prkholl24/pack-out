/**
 * @file scaledwindow.cpp
 * @brief Implements the resolution-scaling wrapper.
 */
#include "scaledwindow.h"
#include "mainwindow.h"

ScaledWindow::ScaledWindow(MainWindow* game, QWidget* parent)
    : QGraphicsView(parent)
    , gameWindow(game)
{
    setScene(&scene);

    // Lock the game canvas to 1920x1080; all existing code stays unchanged.
    gameWindow->setFixedSize(1920, 1080);
    scene.addWidget(gameWindow);

    // Black letterbox bars when the aspect ratio doesn't match.
    setBackgroundBrush(Qt::black);

    // Remove any scroll bars or frame.
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);

    // Smooth scaling for text and sprites.
    setRenderHint(QPainter::SmoothPixmapTransform);
}

void ScaledWindow::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(0, 0, 1920, 1080, Qt::KeepAspectRatio);
}

void ScaledWindow::keyPressEvent(QKeyEvent* event)
{
    // Let Escape close/exit fullscreen rather than being swallowed by the view.
    if (event->key() == Qt::Key_Escape) {
        close();
        return;
    }
    QGraphicsView::keyPressEvent(event);
}
