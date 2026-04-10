/**
 * @file backpack.h
 * @brief Contains the BackpackScene widget — the packing-phase view.
 *
 * BackpackScene is a self-contained QWidget that runs an entire
 * packing mini-game using the Box2D 2-D rigid-body physics engine.
 *
 * ## Box2D crash course (for readers new to the library)
 *
 * Box2D organizes physics around three core concepts:
 *
 * - **b2World** — the simulation container. Holds every body, joint,
 *   and contact. Calling world.Step() advances the simulation by one
 *   time increment.
 *
 * - **b2Body** — a rigid object in the world. Has a position, angle,
 *   velocity, and type:
 *     - `b2_staticBody`  — never moves (walls, ground, shelves).
 *     - `b2_dynamicBody` — affected by forces and gravity (items).
 *     - `b2_kinematicBody` — moves at a set velocity, not used here.
 *
 * - **b2Fixture** — attaches a *shape* (polygon, circle, edge) and
 *   *material* (density, friction, restitution) to a body.  A single
 *   body can have multiple fixtures (e.g. a water bottle = body
 *   rectangle + cap rectangle).  Fixture density determines the
 *   body's mass via `mass = density * area`.
 *
 * This file also uses:
 * - **b2MouseJoint** — a spring that pulls a body toward the cursor.
 * - **Sensors** (`isSensor = true`) — fixtures that detect overlap
 *   without causing physical collisions.
 * - **Contact/Destruction listeners** — callbacks Box2D invokes
 *   when contacts begin/end or when joints/fixtures are destroyed.
 *
 * ## Coordinate systems
 *
 * Box2D uses a Y-up coordinate system (origin at the bottom-left),
 * while Qt paints with Y-down (origin at the top-left).  The helpers
 * screenToWorld() / worldToScreen() convert between the two, and
 * paintEvent() applies a local Y-flip when drawing fixture vertices.
 *
 * The constant PIXELS_PER_METER (50) bridges the scale gap: Box2D
 * is tuned for objects in the 0.1 – 10 m range, so a 0.7 m banana
 * and a 1.8 m-wide pack sit comfortably in the engine's sweet spot.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison, Claude Opus 4.6 (1M context)
 * @date 4/9/2026
 */
#ifndef BACKPACKSCENE_H
#define BACKPACKSCENE_H

#include <QWidget>
#include "Box2D/Box2D.h"
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class BackpackScene;
}
QT_END_NAMESPACE

/**
 * @class BackpackScene
 * @brief The packing-phase scene widget.
 *
 * Owns a Box2D world containing a shelf with spawners, a U-shaped
 * backpack, a "Cinch & Go" button, and dynamically-spawned item
 * bodies. The player drags items from the shelf into the pack, then
 * activates the button to start the level.
 *
 * Intended to be hosted inside MainWindow's QStackedWidget — it does
 * not set its own size or hide chrome (that's MainWindow's job).
 */
class BackpackScene : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the packing scene.
     *
     * Sets up the Box2D world with gravity, registers destruction and
     * contact listeners, builds the static scene geometry (shelf,
     * pack, spawners, Cinch & Go button), and starts a 60 Hz timer
     * that drives the physics step + repaint loop.
     *
     * @param parent  Owner widget (typically the QStackedWidget
     *                inside MainWindow).
     */
    BackpackScene(QWidget *parent = nullptr);

    /**
     * @brief Tears down the scene.
     *
     * Detaches Box2D listeners before member destruction to prevent
     * dangling-pointer callbacks, then deletes the UI form.
     */
    ~BackpackScene();

signals:
    /**
     * @brief Emitted once the Cinch & Go button exits the screen.
     *
     * The model/controller layer should subscribe to this to
     * transition out of the packing scene and start the trip.
     */
    void levelHasStarted();

    /**
     * @brief Emitted when an item body enters the pack's interior.
     *
     * De-duplicated per body: multi-fixture items (banana = 2
     * fixtures, bottle = 2) still emit exactly one "added" event.
     *
     * @param itemName  Human-readable name, e.g. "BANANA".
     */
    void itemAddedToBag(const QString& itemName);

    /**
     * @brief Emitted when an item body leaves the pack's interior
     * (dragged out, or destroyed while still inside).
     *
     * @param itemName  Human-readable name, e.g. "BANANA".
     */
    void itemRemovedFromBag(const QString& itemName);

protected:
    /**
     * @brief Renders every Box2D body as a colored Qt polygon.
     *
     * For each body the painter is translated to the body's screen
     * position, rotated by the body's angle, and scaled by
     * PIXELS_PER_METER so that fixture vertices (in meters) map to
     * pixels. A local Y-flip (`QPointF(v.x, -v.y)`) converts from
     * Box2D's Y-up to Qt's Y-down.
     *
     * Labels (spawner names, "CINCH & GO") are drawn in unscaled
     * screen space after the transform is restored.
     *
     * @param event  The paint event issued by Qt's repaint system.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Handles clicks: spawning items, grabbing items,
     *        or activating the Cinch & Go button.
     *
     * Hit-tests the click position against every body's fixtures
     * (via findBodyAt), then branches on the body's BodyKind tag:
     * - Spawner  -> create a new item + immediately attach it to the
     *   mouse joint so the user can drag in one continuous gesture.
     * - Dynamic item -> attach the mouse joint for rearranging.
     * - Cinch & Go (idle) -> flip from static to dynamic, retag,
     *   and launch upward at 15 m/s.
     *
     * @param event  Mouse event from Qt.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Updates the mouse joint target so the held item follows
     *        the cursor with spring-like tension.
     *
     * @param event  Mouse event from Qt.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief Releases the held item by destroying the mouse joint.
     *
     * @param event  Mouse event from Qt.
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    /**
     * @brief Advances the physics world by one time step and repaints.
     *
     * Called 60 times per second by `timer`. After stepping, sweeps
     * all dynamic bodies and destroys any that have left the visible
     * screen area (the "destroy-on-exit" pattern that replaces
     * traditional world-boundary walls).
     */
    void updatePhysics();

private:
    // -----------------------------------------------------------------
    //  Constants
    // -----------------------------------------------------------------

    /**
     * @brief Target frames per second for the physics + render loop.
     */
    const int REFRESH_RATE = 60;

    /**
     * @brief Converts between Box2D meters and Qt pixels.
     *
     * Box2D is numerically stable for objects in the 0.1 – 10 m
     * range.  At 50 px/m a 0.7 m banana is 35 px on screen and the
     * 800 px-wide window spans 16 m of world space — both well
     * inside Box2D's comfort zone.
     */
    static constexpr float PIXELS_PER_METER = 50.0f;

    /** @brief World width derived from window width / scale. */
    static constexpr float WORLD_WIDTH_M = 16.0f;
    /** @brief World height derived from window height / scale. */
    static constexpr float WORLD_HEIGHT_M = 12.0f;
    /** @brief Locked window width in pixels. */
    static constexpr int WINDOW_WIDTH_PX = 800;
    /** @brief Locked window height in pixels. */
    static constexpr int WINDOW_HEIGHT_PX = 600;

    // -----------------------------------------------------------------
    //  Core state
    // -----------------------------------------------------------------

    /** @brief Auto-generated form (empty — reserved for future Designer use). */
    Ui::BackpackScene* ui;

    /**
     * @brief The Box2D physics world.
     *
     * A b2World owns every body, fixture, joint, and contact in the
     * simulation.  Constructed with downward gravity (0, -10) m/s^2.
     * Calling world.Step() moves bodies, resolves collisions, and
     * fires contact callbacks.
     */
    b2World world;

    /**
     * @brief Timer driving the game loop (physics step + repaint).
     *
     * Fires at REFRESH_RATE Hz.  Each tick calls updatePhysics().
     */
    QTimer* timer;

    // -----------------------------------------------------------------
    //  Mouse interaction
    // -----------------------------------------------------------------

    /**
     * @brief The active mouse joint, or nullptr if nothing is held.
     *
     * A b2MouseJoint is a soft spring connecting a body to a target
     * point (the cursor).  bodyA is a static anchor; bodyB is the
     * item being dragged.  maxForce, frequency, and damping control
     * how "snappy" the drag feels.
     */
    b2MouseJoint* mouseJoint = nullptr;

    /**
     * @brief Invisible zero-mass static body used as bodyA of the
     *        mouse joint.
     *
     * Every b2Joint needs two bodies. The anchor never moves —
     * the joint's target (set to the cursor position each frame)
     * pulls bodyB toward it.
     */
    b2Body* mouseJointAnchor = nullptr;

    // -----------------------------------------------------------------
    //  Scene-specific tracked bodies
    // -----------------------------------------------------------------

    /**
     * @brief The Cinch & Go button body (static while idle, dynamic
     *        after activation).  Nullptr after it exits the screen
     *        and is destroyed.
     */
    b2Body* cinchAndGoButton = nullptr;

    /** @brief One-shot guard so levelHasStarted() emits exactly once. */
    bool levelStarted = false;

    // -----------------------------------------------------------------
    //  Box2D listeners
    // -----------------------------------------------------------------

    /**
     * @brief Clears mouseJoint when Box2D auto-destroys it.
     *
     * When a body is destroyed, Box2D silently destroys every joint
     * attached to it — but our raw mouseJoint pointer would dangle.
     * The destruction listener's SayGoodbye(b2Joint*) nulls it.
     */
    class DestructionListener;
    std::unique_ptr<DestructionListener> destructionListener;

    /**
     * @brief Converts pack-sensor contacts into bag signals.
     *
     * A sensor fixture filling the pack's interior fires
     * BeginContact / EndContact when items enter or leave.
     * The listener de-duplicates per body (using a contact count)
     * and emits itemAddedToBag / itemRemovedFromBag.
     */
    class ContactListener;
    std::unique_ptr<ContactListener> contactListener;

    /**
     * @brief Static sensor body filling the pack's interior volume.
     *
     * Its single fixture has isSensor = true, so it detects overlap
     * with item bodies without physically blocking them.  Contact
     * callbacks from this fixture drive the bag-contents signals.
     */
    b2Body* packSensor = nullptr;

    // -----------------------------------------------------------------
    //  Scene construction helpers
    // -----------------------------------------------------------------

    /** @brief Creates the item shelf (static platform, left side). */
    void createShelf();
    /** @brief Creates the U-shaped pack walls + interior sensor. */
    void createBackpack();
    /** @brief Creates the three spawner buttons on the shelf. */
    void createSpawners();
    /** @brief Creates the Cinch & Go ledge + button. */
    void createCinchAndGoButton();
    /** @brief Creates the invisible mouse-joint anchor body. */
    void createMouseAnchor();

    // -----------------------------------------------------------------
    //  Item spawn helpers
    // -----------------------------------------------------------------

    /**
     * @brief Spawns a banana (two convex quads forming a crescent).
     * @param position  Center in world meters.
     * @return The new dynamic body, already tagged ItemBanana.
     */
    b2Body* spawnBanana(const b2Vec2& position);

    /**
     * @brief Spawns a potato (single 6-vertex convex blob).
     * @param position  Center in world meters.
     * @return The new dynamic body, already tagged ItemPotato.
     */
    b2Body* spawnPotato(const b2Vec2& position);

    /**
     * @brief Spawns a water bottle (body rect + cap rect).
     * @param position  Center in world meters.
     * @return The new dynamic body, already tagged ItemBottle.
     */
    b2Body* spawnBottle(const b2Vec2& position);

    // -----------------------------------------------------------------
    //  Picking and dragging
    // -----------------------------------------------------------------

    /**
     * @brief Returns the first body whose fixtures contain @p worldPos.
     *
     * Iterates the world body list and calls b2Fixture::TestPoint on
     * each fixture.  For the small body count in this scene (< 30)
     * this linear scan is simpler than b2World::QueryAABB.
     *
     * @param worldPos  Point in Box2D world coordinates (meters).
     * @return The hit body, or nullptr if no fixture contains the point.
     */
    b2Body* findBodyAt(const b2Vec2& worldPos);

    /**
     * @brief Attaches the mouse joint between the anchor and @p body.
     *
     * maxForce is set to `1000 * body->GetMass()` — high enough to
     * drag heavy items without lag, but still spring-like.  A
     * maxForce of 0 (Box2D's default) is a classic trap: the joint
     * silently does nothing.
     *
     * @param body      The dynamic body to drag.
     * @param worldPos  Initial cursor position (meters).
     */
    void beginDragging(b2Body* body, const b2Vec2& worldPos);

    /** @brief Destroys the current mouse joint, if any. */
    void endDragging();

    // -----------------------------------------------------------------
    //  Coordinate conversion
    // -----------------------------------------------------------------

    /**
     * @brief Converts a Qt screen position to Box2D world meters.
     *
     * Qt:    origin at top-left, Y increases downward.
     * Box2D: origin at bottom-left, Y increases upward.
     *
     * The conversion flips Y and scales by 1/PIXELS_PER_METER.
     *
     * @param point  Position in Qt widget-relative pixels.
     * @return Equivalent position in the Box2D world (meters).
     */
    b2Vec2 screenToWorld(const QPoint& point);

    /**
     * @brief Converts a Box2D world position to Qt screen pixels.
     *
     * Inverse of screenToWorld(): scales by PIXELS_PER_METER and
     * flips Y so that world-y = 0 lands at the bottom of the widget.
     *
     * @param point  Position in Box2D world meters.
     * @return Equivalent position in Qt widget-relative pixels.
     */
    QPointF worldToScreen(const b2Vec2& point);
};

#endif // BACKPACKSCENE_H
