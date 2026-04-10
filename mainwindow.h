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
#include <memory>

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

signals:
    /**
     * @brief Emitted once the player activates the "Cinch & Go"
     * button and the button body has fully exited the screen.
     *
     * The controller/model layer can subscribe to this signal to
     * transition out of the packing scene and start the trip.
     */
    void levelHasStarted();

    /**
     * @brief Emitted when an item body enters the pack's interior.
     *
     * Fires exactly once per item per entry, even for multi-fixture
     * items (the banana has two fixtures, the bottle two) — a contact
     * counter inside the listener de-duplicates per body.
     *
     * @param itemName - Short human-readable name, e.g. "BANANA".
     */
    void itemAddedToBag(const QString& itemName);

    /**
     * @brief Emitted when an item body leaves the pack's interior,
     * or when an item inside the pack is destroyed (e.g. dragged out
     * and off-screen).
     *
     * @param itemName - Short human-readable name, e.g. "BANANA".
     */
    void itemRemovedFromBag(const QString& itemName);

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
     * @brief Scale factor between the Box2D world (meters) and Qt pixels.
     *
     * Box2D is tuned for objects in the 0.1–10 meter range. Treating
     * 1 world unit as 1 pixel pushes bodies far outside that band,
     * so we scale: 1 meter == PIXELS_PER_METER pixels.
     */
    static constexpr float PIXELS_PER_METER = 50.0f;

    /**
     * @brief World dimensions in meters.
     *
     * An 800x600 window at 50 px/m gives a 16x12 m world — items
     * sized ~0.3–1 m and a pack ~3 m wide all sit in Box2D's
     * numerically-healthy range.
     */
    static constexpr float WORLD_WIDTH_M = 16.0f;
    static constexpr float WORLD_HEIGHT_M = 12.0f;

    /**
     * @brief Fixed window dimensions in pixels.
     *
     * The window size is locked via setFixedSize() so the
     * screen <-> world math never has to account for resize.
     */
    static constexpr int WINDOW_WIDTH_PX = 800;
    static constexpr int WINDOW_HEIGHT_PX = 600;

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
     * @brief Invisible static body used as bodyA for the mouse joint.
     *
     * b2MouseJoint requires two bodies. bodyB is the grabbed item;
     * bodyA is this anchor, which never moves.
     */
    b2Body* mouseJointAnchor = nullptr;

    /**
     * @brief The "Cinch & Go" button body.
     *
     * Tracked so updatePhysics() can watch it leave the screen
     * after activation and emit levelHasStarted().
     */
    b2Body* cinchAndGoButton = nullptr;

    /**
     * @brief Guards against emitting levelHasStarted() more than once.
     */
    bool levelStarted = false;

    /**
     * @brief Listener that nulls out mouseJoint if its body is destroyed.
     *
     * Box2D auto-destroys joints attached to a destroyed body but leaves
     * our mouseJoint pointer dangling. This listener intercepts the
     * SayGoodbye(b2Joint*) callback and clears the pointer.
     */
    class DestructionListener;
    std::unique_ptr<DestructionListener> destructionListener;

    /**
     * @brief Listener that fires itemAddedToBag / itemRemovedFromBag
     * when item bodies enter or leave the pack sensor fixture.
     *
     * Maintains an internal contact count per body so multi-fixture
     * items (banana, bottle) only emit one signal per true entry.
     */
    class ContactListener;
    std::unique_ptr<ContactListener> contactListener;

    /**
     * @brief Static body carrying a single sensor fixture that fills
     * the pack's interior volume. Collisions with this fixture are
     * non-physical but still fire contact callbacks.
     */
    b2Body* packSensor = nullptr;

    /**
     * @name Scene construction helpers
     *
     * Each one builds a portion of the initial scene by creating
     * static Box2D bodies with tagged user data.
     */
    /// @{
    void createShelf();
    void createBackpack();
    void createSpawners();
    void createCinchAndGoButton();
    void createMouseAnchor();
    /// @}

    /**
     * @name Item spawn helpers
     *
     * Create a dynamic body with the right fixtures and physics
     * properties for each item type. Position is in world meters.
     */
    /// @{
    b2Body* spawnBanana(const b2Vec2& position);
    b2Body* spawnPotato(const b2Vec2& position);
    b2Body* spawnBottle(const b2Vec2& position);
    /// @}

    /**
     * @brief Returns the first body whose fixtures contain the given point.
     *
     * Iterates the world's body list in forward order. For the small
     * number of bodies in this scene (< 20) this is simpler and faster
     * to write than b2World::QueryAABB.
     *
     * @param worldPos - Point in Box2D world coordinates.
     * @return The hit body, or nullptr if no fixture contains the point.
     */
    b2Body* findBodyAt(const b2Vec2& worldPos);

    /**
     * @brief Creates the mouse joint that glues a body to the cursor.
     */
    void beginDragging(b2Body* body, const b2Vec2& worldPos);

    /**
     * @brief Destroys the current mouse joint, if any.
     */
    void endDragging();

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
