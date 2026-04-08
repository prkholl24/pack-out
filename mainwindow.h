/**
 * @file mainwindow.h
 * @brief Contains headers for mainwindow.cpp
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Box2D/Box2D.h"
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief The main application window.
 *
 * This class handles the rendering of the simulation, user input,
 * and manages the Box2D physics world.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for MainWindow
     *
     * Initializes the UI, creates the Box2D world, and starts the timer.
     *
     * @param parent widget
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor for MainWindow
     */
    ~MainWindow();

protected:
    /**
     * @brief QPainter uses this to know how to paint the scene
     *
     * Translates Box2D objects into rectangles that Qt can paint.
     *
     * @param event – The paint event created by Qt
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Overrides the mouse press event to interact with physics objects.
     *
     * If the user clicks a spawner (say a banana spawner)
     *                -> create a new banana to render and attach it to the users mouse.
     * If the user clicks an existing item -> attach it to the users mouse.
     *
     * @param event – mouse event created by Qt
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Overrides the mouse move event to update interaction state.
     *
     * Checks if the user is holding an item and allows them to drag it around if they are.
     *
     * @param event – mouse event created by Qt
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief Overrides the mouse release event to end interaction.
     *
     * If the user is holding an object -> release it.
     *
     * @param event – mouse event created by Qt
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    /**
     * @brief Updates the physics world and repaints.
     *
     * Used by the timer to trigger an update to the view.
     */
    void updatePhysics();

private:
    /**
     * @brief Contains all of the Qt widgets and buttons.
     */
    Ui::MainWindow *ui;

    /**
     * @brief The Box2D physics world.
     *
     * Manages all physical bodies and handles the simulation steps.
     */
    b2World world;

    /**
     * @brief Timer for the game loop
     *
     * Continuously emits `udpatePhysics()`
     */
    QTimer* timer;

    /**
     * @brief Used for mouse-based interaction with Box2D shapes.
     *
     * Allows the user to create a spring-y tension between
     * the mouse and other objects.
     */
    b2MouseJoint* mouseJoint = nullptr;

    /**
     * @brief Converts Qt screen position to Box2D
     * @param point – The position according to Qt
     *
     * Qt – Maps the TOP-LEFT of the screen the logical 0,0
     *      As you move RIGHT, you increase the X value
     *      As you move DOWN, you increse the Y value
     *
     * Box2D – Maps the BOTTOM-LEFT as the logical 0,0
     *      As you move RIGHT, you increase the X value
     *      As you move UP, you increase the Y value.
     *
     * @return The corresponding position in the Box2D world.
     */
    b2Vec2 screenToWorld(const QPoint& point);
};
#endif // MAINWINDOW_H
