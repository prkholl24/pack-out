/**
 * @file scaledwindow.h
 * @brief Resolution-independent wrapper that scales the 1920x1080 game canvas
 *        to fit any display size using QGraphicsView.
 */
#ifndef SCALEDWINDOW_H
#define SCALEDWINDOW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QResizeEvent>
#include <QKeyEvent>

class MainWindow;

/**
 * @class ScaledWindow
 * @brief Hosts the fixed-size MainWindow inside a QGraphicsView that
 *        scales it to fit any screen resolution, with black letterboxing.
 */
class ScaledWindow : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ScaledWindow(MainWindow* game, QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    QGraphicsScene  scene;
    MainWindow*     gameWindow;
};

#endif // SCALEDWINDOW_H
