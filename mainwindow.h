/**
 * @file mainwindow.h
 * @brief Contains the MainWindow class and handles rendering + user interaction.
 *
 * Defines the MainWindow class, which is responsible for:
 * - setting up the UI
 * - managing the Box2D physics world
 * - handling mouse input
 * - rendering objects to the screen
 *
 * This connects the physics simulation with the Qt interface so the user
 * can interact with items in a visual way.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 * Reviewed by Lindsey Reese 4/8/2026
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
     * @param event - The paint event created by Qt
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Overrides the mouse press event to interact with physics objects.
     *
     * If the user clicks a spawner (say a banana spawner)
     * -> create a new banana to render and attach it to the user's mouse.
     * If the user clicks an existing item
     * -> attach it to the user's mouse.
     *
     * @param event mouse event created by Qt
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Overrides the mouse move event to update interaction state.
     *
     * Checks if the user is holding an item and allows them to drag it around if they are.
     *
     * @param event mouse event created by Qt
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief Overrides the mouse release event to end interaction.
     *
     * If the user is holding an object -> release it.
     *
     * @param event mouse event created by Qt
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
     * @brief REFRESH_RATE determines how often the screen gets updated.
     */
    const int REFRESH_RATE = 60;

    /**
     * @brief Contains all of the Qt widgets and buttons.
     */
    Ui::MainWindow* ui;

    /**
     * @brief The Box2D physics world.
     *
     * Manages all physical bodies and handles the simulation steps.
     */
    b2World world;

    /**
     * @brief Timer for the game loop.
     *
     * Continuously calls `updatePhysics()`.
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
     * @brief Converts a Qt screen position to a Box2D world position
     *
     * Qt - Maps the TOP-LEFT of the screen as (0,0)
     * - Moving RIGHT increases the X value
     * - Moving DOWN increases the Y value
     *
     * Box2D - Maps the BOTTOM-LEFT as (0,0)
     * - Moving RIGHT increases the X value
     * - Moving UP increases the Y value
     *
     * @param point - The position according to Qt
     * @return The corresponding position in the Box2D world
     */
    b2Vec2 screenToWorld(const QPoint& point);

    /**
     * @brief Converts a Box2D world position to a Qt screen position
     *
     * Qt - Maps the TOP-LEFT of the screen as (0,0)
     * - Moving RIGHT increases the X value
     * - Moving DOWN increases the Y value
     *
     * Box2D - Maps the BOTTOM-LEFT as (0,0)
     * - Moving RIGHT increases the X value
     * - Moving UP increases the Y value
     *
     * @param point – The position according to Box2D
     * @return The corresponding position in the Qt screen.
     */
    QPointF worldToScreen(const b2Vec2& point);
};
#endif // MAINWINDOW_H
