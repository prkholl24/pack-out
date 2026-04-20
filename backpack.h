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
 * and a 1.8 m-wide pack sit comfortably in the library's sweet spot.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison, Claude Opus 4.6 (1M context)
 * @date 4/9/2026
 */
#ifndef BACKPACKSCENE_H
#define BACKPACKSCENE_H

#include <QColor>
#include <QRect>
#include <QWidget>
#include "Box2D/Box2D.h"
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QStringList>
#include <memory>
#include <unordered_map>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class BackpackScene;
}
QT_END_NAMESPACE

struct LevelTheme;

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
     * pack, tabs, spawners, Cinch & Go button), and starts a 60 Hz
     * timer that drives the physics step + repaint loop.
     *
     * @param availableItems  Registry item names available for the level.
     * @param parent  Owner widget (typically the QStackedWidget
     *                inside MainWindow).
     */
    BackpackScene(const QStringList& availableItems, QWidget *parent = nullptr);

    /**
     * @brief Tears down the scene.
     *
     * Detaches Box2D listeners before member destruction to prevent
     * dangling-pointer callbacks, then deletes the UI form.
     */
    ~BackpackScene();

    /**
     * @brief Updates the displayed weight and volume.
     * @param weight  Total weight in lbs.
     * @param volume  Total volume in liters.
     */
    void updateStats(double weight, double volume);

    /**
     * @brief Applies the per-level theme colors to the scene.
     *
     * Stores primary/secondary colors for use in paintEvent (background
     * tint, Cinch & Go button, themed border). Does not change layout.
     */
    void applyTheme(const LevelTheme& theme);

    /**
     * @brief Screen region reserved for the future Survival Guide overlay.
     *
     * No physics bodies or interactive UI are placed in this rectangle so
     * that `feature/guide`'s SurvivalScrapbook widget can be dropped in
     * without fighting this scene's layout.
     */
    static constexpr QRect kGuideReservedRect{0, 0, 550, 340};

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
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void updatePhysics();

private:
    // -----------------------------------------------------------------
    //  Constants
    // -----------------------------------------------------------------
    const int REFRESH_RATE = 60;
    static constexpr float PIXELS_PER_METER = 90.0f;
    static constexpr int   WINDOW_WIDTH_PX   = 1920;
    static constexpr int   WINDOW_HEIGHT_PX  = 1080;
    static constexpr float WORLD_WIDTH_M     = WINDOW_WIDTH_PX  / PIXELS_PER_METER; // ~21.33 m
    static constexpr float WORLD_HEIGHT_M    = WINDOW_HEIGHT_PX / PIXELS_PER_METER; // 12 m

    // -----------------------------------------------------------------
    //  Theme state (applied by MainWindow on level change)
    // -----------------------------------------------------------------
    QColor themePrimary   = QColor("#3E7A4F");
    QColor themeSecondary = QColor("#C8D9B8");
    QColor themeOnPrim    = QColor("#FFFFFF");
    QColor themeOnSurface = QColor("#1E1E1E");

    // -----------------------------------------------------------------
    //  Backpack status
    // -----------------------------------------------------------------
    double currentWeight = 0.0;
    double currentVolume = 0.0;

    // -----------------------------------------------------------------
    //  Core state
    // -----------------------------------------------------------------
    Ui::BackpackScene* ui;
    b2World world;
    QTimer* timer;

    // -----------------------------------------------------------------
    //  Mouse interaction
    // -----------------------------------------------------------------
    b2MouseJoint* mouseJoint = nullptr;
    b2Body* mouseJointAnchor = nullptr;

    // -----------------------------------------------------------------
    //  Scene-specific tracked bodies
    // -----------------------------------------------------------------
    b2Body* cinchAndGoButton = nullptr;
    bool levelStarted = false;

    // -----------------------------------------------------------------
    //  Body → name mapping
    // -----------------------------------------------------------------
    /// Maps spawner and item bodies to their registry name (e.g. "BANANA").
    std::unordered_map<b2Body*, QString> bodyNames;

    // -----------------------------------------------------------------
    //  Available items and category state
    // -----------------------------------------------------------------
    QStringList allAvailableItems;
    QStringList foodItems;
    QStringList drinkItems;
    QStringList gearItems;
    QString activeTab = "food";

    /// Current spawner bodies (destroyed/recreated on tab switch).
    std::vector<b2Body*> spawnerBodies;

    // -----------------------------------------------------------------
    //  Hover tooltip state
    // -----------------------------------------------------------------
    b2Body* hoveredBody = nullptr;

    // -----------------------------------------------------------------
    //  Box2D listeners
    // -----------------------------------------------------------------
    class DestructionListener;
    std::unique_ptr<DestructionListener> destructionListener;

    class ContactListener;
    std::unique_ptr<ContactListener> contactListener;

    b2Body* packSensor = nullptr;

    // -----------------------------------------------------------------
    //  Scene construction helpers
    // -----------------------------------------------------------------
    void createBackpack();
    void createTabs();
    void createCinchAndGoButton();
    void createMouseAnchor();

    // -----------------------------------------------------------------
    //  Dynamic spawner management
    // -----------------------------------------------------------------
    /// Switches the shelf to show items in the given category.
    void showCategory(const QString& category);
    /// Lays out spawner bodies for a list of item names.
    void layoutSpawners(const QStringList& items);

    // -----------------------------------------------------------------
    //  Item spawn (data-driven from shape in items.json)
    // -----------------------------------------------------------------
    /// Spawns a dynamic item body using the shape data from ItemRegistry.
    b2Body* spawnItem(const b2Vec2& position, const QString& itemName);

    // -----------------------------------------------------------------
    //  Picking and dragging
    // -----------------------------------------------------------------
    b2Body* findBodyAt(const b2Vec2& worldPos);
    void beginDragging(b2Body* body, const b2Vec2& worldPos);
    void endDragging();

    // -----------------------------------------------------------------
    //  Coordinate conversion
    // -----------------------------------------------------------------
    b2Vec2 screenToWorld(const QPoint& point);
    QPointF worldToScreen(const b2Vec2& point);
};

#endif // BACKPACKSCENE_H
