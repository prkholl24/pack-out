/**
 * @file backpack.cpp
 * @brief Implementation of the BackpackScene widget.
 *
 * This file is the heaviest user of Box2D in the project. Reading it
 * top-to-bottom is a reasonable tutorial for integrating Box2D with a
 * Qt widget — it covers world creation, body/fixture setup, sensors,
 * mouse joints, contact listeners, destruction listeners, and the
 * render loop.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/9/2026
 */
#include "backpack.h"
#include "ui_backpack.h"

#include <QColor>
#include <QFont>
#include <QPen>
#include <QPolygonF>
#include <QRectF>

#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>

// =========================================================================
//  Anonymous namespace — file-private helpers and body-tagging system
// =========================================================================

namespace {

/**
 * @enum BodyKind
 * @brief Identifies every type of Box2D body in the scene.
 *
 * Box2D bodies carry a single `void* userData` pointer that the
 * engine ignores but the application can use for any purpose.  We
 * cast BodyKind enum values to `void*` and store them with
 * `body->SetUserData(tagFor(kind))`.  Later, `kindOf(body)` casts
 * back.  This is the standard Box2D body-identification pattern.
 *
 * The underlying type is `std::uintptr_t` so that the enum ↔ void*
 * round-trip is well-defined (C++11 §5.2.10/5).
 */
enum class BodyKind : std::uintptr_t {
    None = 0,
    MouseAnchor,        ///< Invisible static body for the mouse joint's bodyA.
    WorldBound,         ///< Unused after walls were removed; kept for future use.
    Shelf,              ///< Static shelf platform on the left side.
    SpawnerBanana,      ///< Static spawner button for bananas.
    SpawnerPotato,      ///< Static spawner button for potatoes.
    SpawnerBottle,      ///< Static spawner button for water bottles.
    PackWall,           ///< One of the three static U-shaped pack walls.
    PackSensor,         ///< Sensor covering the pack's interior volume.
    CinchAndGoLedge,    ///< Static ledge the Cinch & Go button rests on.
    CinchAndGoIdle,     ///< Cinch & Go button before activation (static).
    CinchAndGoActivated,///< Cinch & Go button after activation (dynamic).
    ItemBanana,         ///< Dynamic banana item.
    ItemPotato,         ///< Dynamic potato item.
    ItemBottle,         ///< Dynamic water-bottle item.
};

/// @brief Returns true if @p kind represents a packable item body.
inline bool isItemKind(BodyKind kind) {
    return kind == BodyKind::ItemBanana
        || kind == BodyKind::ItemPotato
        || kind == BodyKind::ItemBottle;
}

/// @brief Maps an item's BodyKind to a short display name (e.g. "BANANA").
inline QString itemNameOf(BodyKind kind) {
    switch (kind) {
        case BodyKind::ItemBanana: return QStringLiteral("BANANA");
        case BodyKind::ItemPotato: return QStringLiteral("POTATO");
        case BodyKind::ItemBottle: return QStringLiteral("BOTTLE");
        default:                   return QString();
    }
}

/// @brief Packs a BodyKind into the void* that b2Body::SetUserData expects.
inline void* tagFor(BodyKind k) {
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(k));
}

/// @brief Unpacks a BodyKind from b2Body::GetUserData.
inline BodyKind kindOf(const b2Body* b) {
    return static_cast<BodyKind>(reinterpret_cast<std::uintptr_t>(b->GetUserData()));
}

/**
 * @brief Returns the fill color to render a body with based on its kind.
 *
 * Bodies with alpha = 0 (MouseAnchor, PackSensor) are skipped
 * entirely by paintEvent — they exist in Box2D but are invisible.
 */
QColor colorFor(BodyKind kind) {
    switch (kind) {
        case BodyKind::ItemBanana:          return QColor(255, 225, 60);
        case BodyKind::ItemPotato:          return QColor(150, 100, 55);
        case BodyKind::ItemBottle:          return QColor(130, 200, 230);

        case BodyKind::SpawnerBanana:       return QColor(220, 200, 100);
        case BodyKind::SpawnerPotato:       return QColor(140, 105, 70);
        case BodyKind::SpawnerBottle:       return QColor(130, 175, 200);

        case BodyKind::Shelf:               return QColor(120, 85, 45);
        case BodyKind::CinchAndGoLedge:     return QColor(120, 85, 45);

        case BodyKind::PackWall:            return QColor(75, 75, 85);
        case BodyKind::WorldBound:          return QColor(55, 55, 55);

        case BodyKind::CinchAndGoIdle:      return QColor(80, 180, 90);
        case BodyKind::CinchAndGoActivated: return QColor(50, 130, 60);

        case BodyKind::MouseAnchor:         return QColor(0, 0, 0, 0);
        case BodyKind::PackSensor:          return QColor(0, 0, 0, 0);

        case BodyKind::None:
        default:                            return QColor(Qt::blue);
    }
}

/**
 * @brief Returns the label text drawn above a body, or "" for bodies
 *        that should not carry a label.
 */
QString labelFor(BodyKind kind) {
    switch (kind) {
        case BodyKind::SpawnerBanana:  return QStringLiteral("BANANA");
        case BodyKind::SpawnerPotato:  return QStringLiteral("POTATO");
        case BodyKind::SpawnerBottle:  return QStringLiteral("BOTTLE");
        case BodyKind::CinchAndGoIdle: return QStringLiteral("CINCH & GO");
        default:                       return QString();
    }
}

}  // namespace

// =========================================================================
//  Box2D listener classes (nested in BackpackScene)
// =========================================================================

/**
 * @class BackpackScene::DestructionListener
 * @brief Clears BackpackScene::mouseJoint when Box2D destroys it.
 *
 * ## Why this exists
 *
 * When you call `world.DestroyBody(body)`, Box2D automatically
 * destroys every joint attached to that body.  However, it does NOT
 * null any external pointers you may hold to those joints — our
 * `mouseJoint` member would silently become dangling.
 *
 * b2DestructionListener::SayGoodbye(b2Joint*) is called just before
 * the joint is freed.  By comparing the pointer to our mouseJoint
 * we can null it in time.
 */
class BackpackScene::DestructionListener : public b2DestructionListener {
public:
    BackpackScene* owner = nullptr;

    void SayGoodbye(b2Joint* joint) override {
        if (owner && joint == owner->mouseJoint) {
            owner->mouseJoint = nullptr;
        }
    }
    void SayGoodbye(b2Fixture*) override {}
};

/**
 * @class BackpackScene::ContactListener
 * @brief Turns pack-sensor overlap events into bag-contents signals.
 *
 * ## How Box2D contacts work
 *
 * When two fixtures begin overlapping, Box2D calls BeginContact().
 * When they stop overlapping, it calls EndContact().  For *sensor*
 * fixtures (isSensor = true) the overlap is detected but no collision
 * force is applied — the bodies pass right through each other.
 *
 * ## Multi-fixture deduplication
 *
 * Items like the banana (2 fixtures) and the bottle (2 fixtures)
 * generate one BeginContact per fixture that overlaps the sensor.
 * Without deduplication, dropping a banana into the pack would emit
 * two "added" signals.
 *
 * We solve this by maintaining `inBagCount[body]`.  BeginContact
 * increments it; EndContact decrements it.  The "added" signal fires
 * only on the 0 → 1 transition, and the "removed" signal fires only
 * on the 1 → 0 transition.
 *
 * ## Destruction edge case
 *
 * When a body is destroyed, Box2D destroys its contacts first and
 * calls EndContact for each one.  So an item destroyed while still
 * inside the pack will emit a trailing "removed" signal — no manual
 * cleanup needed.
 *
 * ## Thread safety
 *
 * BeginContact / EndContact are called synchronously inside
 * world.Step().  Slots connected with Qt::DirectConnection (the
 * default) run in the same call stack.  Those slots must NOT mutate
 * the Box2D world (no CreateBody, DestroyBody, etc.).  If a future
 * subscriber needs to do that, connect with Qt::QueuedConnection.
 */
class BackpackScene::ContactListener : public b2ContactListener {
public:
    BackpackScene* owner = nullptr;

    void BeginContact(b2Contact* contact) override {
        b2Body* itemBody = itemBodyFromContact(contact);
        if (!itemBody) {
            return;
        }
        int& count = inBagCount[itemBody];
        if (count == 0 && owner) {
            emit owner->itemAddedToBag(itemNameOf(kindOf(itemBody)));
        }
        ++count;
    }

    void EndContact(b2Contact* contact) override {
        b2Body* itemBody = itemBodyFromContact(contact);
        if (!itemBody) {
            return;
        }
        auto it = inBagCount.find(itemBody);
        if (it == inBagCount.end()) {
            return;
        }
        if (--it->second <= 0) {
            if (owner) {
                emit owner->itemRemovedFromBag(itemNameOf(kindOf(itemBody)));
            }
            inBagCount.erase(it);
        }
    }

private:
    /**
     * @brief If one fixture is the pack sensor and the other belongs
     *        to an item, return the item's body.  Otherwise nullptr.
     */
    b2Body* itemBodyFromContact(b2Contact* contact) const {
        b2Body* a = contact->GetFixtureA()->GetBody();
        b2Body* b = contact->GetFixtureB()->GetBody();
        const BodyKind ka = kindOf(a);
        const BodyKind kb = kindOf(b);
        if (ka == BodyKind::PackSensor && isItemKind(kb)) return b;
        if (kb == BodyKind::PackSensor && isItemKind(ka)) return a;
        return nullptr;
    }

    /// Per-body count of how many of its fixtures currently touch the sensor.
    std::unordered_map<b2Body*, int> inBagCount;
};

// =========================================================================
//  Constructor / destructor
// =========================================================================

BackpackScene::BackpackScene(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BackpackScene)
    // Create the Box2D world with downward gravity of 10 m/s^2.
    // b2Vec2(x, y) — positive Y is UP in Box2D, so (0, -10) pulls
    // bodies toward the bottom of the scene.
    , world(b2Vec2(0.0f, -10.0f))
    , timer(new QTimer(this))
{
    ui->setupUi(this);

    // --- Box2D listener registration ---
    //
    // Listeners are callback objects that Box2D invokes during its
    // internal operations.  They must outlive the world, so we store
    // them as unique_ptrs and detach them in the destructor.

    destructionListener = std::make_unique<DestructionListener>();
    destructionListener->owner = this;
    world.SetDestructionListener(destructionListener.get());

    contactListener = std::make_unique<ContactListener>();
    contactListener->owner = this;
    world.SetContactListener(contactListener.get());

    // Temporary: log bag add/remove events to stdout for debugging.
    // These connections will be replaced by real model-layer subscribers.
    connect(this, &BackpackScene::itemAddedToBag, this, [](const QString& name) {
        std::cout << "Added to bag: " << name.toStdString() << std::endl;
    });
    connect(this, &BackpackScene::itemRemovedFromBag, this, [](const QString& name) {
        std::cout << "Removed from bag: " << name.toStdString() << std::endl;
    });

    // --- Scene construction ---
    //
    // Each helper creates a group of static Box2D bodies that form
    // the scene geometry.  Order doesn't matter physics-wise, but
    // the mouse anchor must exist before any mouse-joint code runs.
    createMouseAnchor();
    createShelf();
    createBackpack();
    createCinchAndGoButton();
    createSpawners();

    // --- Game loop ---
    //
    // The timer fires at REFRESH_RATE Hz.  Each tick calls
    // updatePhysics(), which advances the Box2D simulation by one
    // time step and schedules a repaint.
    connect(timer, &QTimer::timeout, this, &BackpackScene::updatePhysics);
    timer->start(1000 / REFRESH_RATE);
}

BackpackScene::~BackpackScene()
{
    // Detach listeners BEFORE the unique_ptrs are destroyed.
    // ~b2World does not currently invoke listener callbacks, but
    // this is defensive against future Box2D versions that might.
    world.SetContactListener(nullptr);
    world.SetDestructionListener(nullptr);
    delete ui;
}

// =========================================================================
//  Rendering
// =========================================================================

void BackpackScene::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // A "cosmetic" pen keeps its width at 1 device pixel regardless
    // of any QPainter transforms (translate, rotate, scale).  Without
    // this, the scale(PIXELS_PER_METER) below would fatten the
    // outline to 50 px.
    QPen outlinePen(Qt::black);
    outlinePen.setCosmetic(true);

    QFont labelFont = painter.font();
    labelFont.setBold(true);
    labelFont.setPointSize(12);

    // Iterate every body in the Box2D world.  GetBodyList() returns
    // the head of a singly-linked list; GetNext() walks it.
    for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
        BodyKind kind = kindOf(body);
        QColor fill = colorFor(kind);

        // Invisible bodies (mouse anchor, pack sensor) have alpha 0.
        if (fill.alpha() == 0) {
            continue;
        }

        // Convert the body's center position from Box2D meters to Qt pixels.
        QPointF screenPos = worldToScreen(body->GetPosition());

        // --- Transform stack ---
        //
        // We want to draw the body's fixture vertices (which are in
        // local meters, Y-up) as screen pixels.  The transform order:
        //
        //   1. translate() — move the origin to the body's screen center.
        //   2. rotate()    — rotate by the body's angle (negated because
        //                    Qt rotates clockwise while Box2D uses CCW).
        //   3. scale()     — convert meters → pixels.
        //
        // Inside the loop below, the vertices are emitted as
        // QPointF(v.x, -v.y) — the negation flips Box2D's Y-up into
        // Qt's Y-down in local space.
        painter.save();
        painter.translate(screenPos);
        painter.rotate(-body->GetAngle() * 180.0f / b2_pi);
        painter.scale(PIXELS_PER_METER, PIXELS_PER_METER);
        painter.setBrush(fill);
        painter.setPen(outlinePen);

        // A body may have multiple fixtures (e.g. the water bottle
        // has a body rectangle and a cap rectangle).  Each fixture
        // is drawn separately; the shared fill color makes them
        // look like one object.
        for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
            // Safety: skip non-polygon shapes.  All shapes in this
            // scene are b2PolygonShape, but this guard future-proofs
            // against adding edge or circle shapes.
            if (fixture->GetShape()->GetType() != b2Shape::e_polygon) {
                continue;
            }
            auto* poly = static_cast<b2PolygonShape*>(fixture->GetShape());
            QPolygonF qPoly;
            for (int32 i = 0; i < poly->GetVertexCount(); ++i) {
                b2Vec2 v = poly->GetVertex(i);
                // Y-flip: Box2D vertex Y is up, Qt drawing Y is down.
                qPoly << QPointF(v.x, -v.y);
            }
            painter.drawPolygon(qPoly);
        }

        // Undo the transform so labels are drawn in unscaled,
        // unrotated screen space — they always face the player.
        painter.restore();

        // Labels float above the body against the dark window
        // background where white bold text is easy to read.
        QString label = labelFor(kind);
        if (!label.isEmpty()) {
            painter.setFont(labelFont);
            painter.setPen(Qt::white);
            QRectF labelRect(screenPos.x() - 80.0, screenPos.y() - 42.0, 160.0, 22.0);
            painter.drawText(labelRect, Qt::AlignCenter, label);
        }
    }
}

// =========================================================================
//  Coordinate conversion
// =========================================================================

/**
 * Qt's origin is at the top-left (Y increases downward).
 * Box2D's origin is at the bottom-left (Y increases upward).
 *
 *     Qt screen (pixels)          Box2D world (meters)
 *     +---------> X               ^ Y
 *     |                           |
 *     |  (0,0) top-left           |  (0, 12) top-left
 *     v Y                         +---------> X
 *                                (0, 0) bottom-left
 *
 * screenToWorld flips Y and divides by PIXELS_PER_METER.
 * worldToScreen multiplies by PIXELS_PER_METER and flips Y.
 */

b2Vec2 BackpackScene::screenToWorld(const QPoint& point)
{
    return b2Vec2(point.x() / PIXELS_PER_METER,
                  (WINDOW_HEIGHT_PX - point.y()) / PIXELS_PER_METER);
}

QPointF BackpackScene::worldToScreen(const b2Vec2& point)
{
    return QPointF(point.x * PIXELS_PER_METER,
                   WINDOW_HEIGHT_PX - point.y * PIXELS_PER_METER);
}

// =========================================================================
//  Mouse interaction
// =========================================================================

void BackpackScene::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    // Convert the Qt click position to Box2D world coordinates so
    // we can compare it against body fixture positions.
    const b2Vec2 worldPos = screenToWorld(event->pos());

    // Hit-test: find the body (if any) that owns a fixture containing
    // the click point.
    b2Body* hit = findBodyAt(worldPos);
    if (!hit) {
        return;
    }

    // Branch on the body's identity tag.
    switch (kindOf(hit)) {

        // --- Spawner buttons ---
        // Clicking a spawner creates a new item body just above the
        // spawner and immediately attaches the mouse joint so the
        // player can drag the item in one continuous gesture.
        case BodyKind::SpawnerBanana: {
            b2Vec2 spawnPos = hit->GetPosition();
            spawnPos.y += 0.5f;
            b2Body* item = spawnBanana(spawnPos);
            beginDragging(item, worldPos);
            break;
        }
        case BodyKind::SpawnerPotato: {
            b2Vec2 spawnPos = hit->GetPosition();
            spawnPos.y += 0.5f;
            b2Body* item = spawnPotato(spawnPos);
            beginDragging(item, worldPos);
            break;
        }
        case BodyKind::SpawnerBottle: {
            b2Vec2 spawnPos = hit->GetPosition();
            spawnPos.y += 0.5f;
            b2Body* item = spawnBottle(spawnPos);
            beginDragging(item, worldPos);
            break;
        }

        // --- Cinch & Go button ---
        // The button starts as a static body.  On click we flip it
        // to dynamic (Box2D recalculates its mass from fixture
        // density), retag it, and give it an upward velocity.
        // It then flies off the top of the screen, gets destroyed by
        // the exit sweep in updatePhysics(), and emits levelHasStarted().
        case BodyKind::CinchAndGoIdle: {
            hit->SetType(b2_dynamicBody);
            hit->SetAwake(true);  // static bodies sleep by default
            hit->SetLinearVelocity(b2Vec2(0.0f, 15.0f));
            hit->SetUserData(tagFor(BodyKind::CinchAndGoActivated));
            break;
        }

        // --- Existing items (and the flying button) ---
        // Attach the mouse joint so the player can rearrange them.
        case BodyKind::ItemBanana:
        case BodyKind::ItemPotato:
        case BodyKind::ItemBottle:
        case BodyKind::CinchAndGoActivated:
            beginDragging(hit, worldPos);
            break;

        // Ignore clicks on walls, shelf, ledge, sensor, anchor.
        default:
            break;
    }
}

void BackpackScene::mouseMoveEvent(QMouseEvent *event)
{
    // While the player holds the mouse button, the mouse joint's
    // target follows the cursor.  The joint's spring (frequency +
    // damping) pulls the held body toward the target.
    if (mouseJoint) {
        mouseJoint->SetTarget(screenToWorld(event->pos()));
    }
}

void BackpackScene::mouseReleaseEvent(QMouseEvent *event)
{
    // Releasing the button destroys the mouse joint, leaving the
    // item to freefall under gravity and friction.
    if (event->button() == Qt::LeftButton) {
        endDragging();
    }
}

// =========================================================================
//  Physics step + destroy-on-exit sweep
// =========================================================================

void BackpackScene::updatePhysics()
{
    // --- Advance the simulation ---
    //
    // world.Step(timeStep, velocityIterations, positionIterations)
    //
    // - timeStep: how much simulated time to advance (1/60 s).
    // - velocityIterations (8): how many times Box2D refines
    //   velocity constraints per step. Higher = more accurate
    //   stacking and collision response.  8 is the library default.
    // - positionIterations (3): how many times Box2D corrects
    //   positional overlap.  3 is usually enough for stable piles.
    world.Step(1.0f / static_cast<float>(REFRESH_RATE) , 8, 3);

    // --- Destroy-on-exit sweep ---
    //
    // Instead of walling items in with boundary bodies, we let them
    // fall off the edge and destroy them once they're far enough
    // off-screen that the player can't see the pop.
    //
    // We CANNOT call DestroyBody inside the GetBodyList iteration
    // because DestroyBody mutates the linked list (invalidating
    // the ->GetNext() chain).  So we collect first, destroy second.
    constexpr float EXIT_MARGIN = 2.0f;  // meters past the screen edge

    std::vector<b2Body*> toDestroy;
    for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
        // Only dynamic bodies can leave the scene.  Static bodies
        // (walls, shelf, sensor) are pinned in place.
        if (body->GetType() != b2_dynamicBody) {
            continue;
        }
        const BodyKind kind = kindOf(body);
        const bool destroyable =
               kind == BodyKind::ItemBanana
            || kind == BodyKind::ItemPotato
            || kind == BodyKind::ItemBottle
            || kind == BodyKind::CinchAndGoActivated;
        if (!destroyable) {
            continue;
        }

        const b2Vec2 pos = body->GetPosition();
        const bool offScreen = pos.x < -EXIT_MARGIN
                               || pos.x > WORLD_WIDTH_M + EXIT_MARGIN
                               || pos.y < -EXIT_MARGIN
                               || pos.y > WORLD_HEIGHT_M + EXIT_MARGIN;
        if (offScreen) {
            toDestroy.push_back(body);
        }
    }

    // Now safe to destroy — we're outside the iteration.
    // DestroyBody also destroys all fixtures and joints on the body.
    // The DestructionListener will null mouseJoint if the dragged
    // body happens to be the one leaving; the ContactListener will
    // fire EndContact for any remaining sensor overlaps.
    for (b2Body* body : toDestroy) {
        const BodyKind kind = kindOf(body);

        if (kind == BodyKind::CinchAndGoActivated && !levelStarted) {
            levelStarted = true;
            emit levelHasStarted();
        }
        if (body == cinchAndGoButton) {
            cinchAndGoButton = nullptr;
        }

        world.DestroyBody(body);
    }

    // Schedule a repaint so the updated body positions are rendered.
    update();
}

// =========================================================================
//  Scene construction
// =========================================================================
//
// Each function below creates one or more static bodies that form the
// permanent scene geometry.  The general recipe for every body:
//
//   1. b2BodyDef  — set the body type and initial position.
//   2. world.CreateBody(&def) — add the body to the world.
//   3. b2PolygonShape — define the shape (SetAsBox for rectangles,
//      Set(vertices, count) for arbitrary convex polygons).
//   4. b2FixtureDef — attach the shape plus material properties
//      (density, friction, restitution, sensor flag, collision filter).
//   5. body->CreateFixture(&fixDef) — finalize the fixture.
//   6. body->SetUserData(tagFor(...)) — tag the body for identification.
//
// SetAsBox(halfWidth, halfHeight) takes *half*-extents, not full
// dimensions.  A call like SetAsBox(2.0, 0.1) creates a 4 m wide
// by 0.2 m tall rectangle centered on the body's position.

void BackpackScene::createMouseAnchor()
{
    // The mouse joint needs two bodies: bodyA (the anchor) and bodyB
    // (the item being dragged).  The anchor is a static body with no
    // fixtures — it never collides or renders.
    b2BodyDef def;
    def.type = b2_staticBody;
    def.position.Set(0.0f, 0.0f);
    mouseJointAnchor = world.CreateBody(&def);
    mouseJointAnchor->SetUserData(tagFor(BodyKind::MouseAnchor));
}

void BackpackScene::createShelf()
{
    // Static body: type = b2_staticBody means it never moves, has
    // infinite mass, and is unaffected by gravity or collisions.
    b2BodyDef def;
    def.type = b2_staticBody;
    def.position.Set(2.5f, 4.0f);  // center of the shelf in world meters
    b2Body* body = world.CreateBody(&def);

    // SetAsBox(halfW, halfH) — a 4.0 m wide by 0.2 m tall rectangle.
    b2PolygonShape box;
    box.SetAsBox(2.0f, 0.1f);

    // A fixture = shape + material.  Density is irrelevant for
    // static bodies (they have infinite mass).  Friction (0.6) keeps
    // spawned items from sliding off the shelf.
    b2FixtureDef fix;
    fix.shape = &box;
    fix.friction = 0.6f;
    body->CreateFixture(&fix);

    body->SetUserData(tagFor(BodyKind::Shelf));
}

void BackpackScene::createBackpack()
{
    // --- Pack walls (three static bodies forming a U) ---

    auto makePackWall = [this](float cx, float cy, float halfW, float halfH) {
        b2BodyDef def;
        def.type = b2_staticBody;
        def.position.Set(cx, cy);
        b2Body* body = world.CreateBody(&def);

        b2PolygonShape box;
        box.SetAsBox(halfW, halfH);

        b2FixtureDef fix;
        fix.shape = &box;
        fix.friction = 0.6f;   // high friction helps items stack
        body->CreateFixture(&fix);

        body->SetUserData(tagFor(BodyKind::PackWall));
    };

    // Left wall, right wall, and bottom.  The top is open so items
    // can be dropped in (and so the Cinch & Go button can escape).
    //
    // Interior: x ∈ [11.1, 12.9] (1.8 m wide), y ∈ [1.1, 5.5].
    makePackWall(11.0f, 3.25f, 0.1f, 2.25f);  // left  (0.2 m thick, 4.5 m tall)
    makePackWall(13.0f, 3.25f, 0.1f, 2.25f);  // right
    makePackWall(12.0f, 1.0f,  1.1f, 0.1f);   // bottom (2.2 m wide, 0.2 m thick)

    // --- Pack sensor ---
    //
    // A "sensor" is a fixture with isSensor = true.  It participates
    // in contact detection (BeginContact / EndContact fire) but does
    // NOT generate collision forces — bodies pass right through it.
    //
    // This sensor fills the pack's interior volume.  The
    // ContactListener watches for item fixtures entering/leaving and
    // emits itemAddedToBag / itemRemovedFromBag.
    b2BodyDef sensorDef;
    sensorDef.type = b2_staticBody;
    sensorDef.position.Set(12.0f, 3.3f);  // centered in the U
    packSensor = world.CreateBody(&sensorDef);

    b2PolygonShape sensorBox;
    // halfW = 0.9 → interior width 1.8 m (matches the gap between walls).
    // halfH = 2.2 → interior height 4.4 m (above the bottom slab,
    //               up to the wall tops).
    sensorBox.SetAsBox(0.9f, 2.2f);

    b2FixtureDef sensorFix;
    sensorFix.shape = &sensorBox;
    sensorFix.isSensor = true;  // detect overlap, don't collide
    packSensor->CreateFixture(&sensorFix);

    packSensor->SetUserData(tagFor(BodyKind::PackSensor));
}

void BackpackScene::createSpawners()
{
    auto makeSpawner = [this](float cx, float cy, BodyKind kind) {
        b2BodyDef def;
        def.type = b2_staticBody;
        def.position.Set(cx, cy);
        b2Body* body = world.CreateBody(&def);

        b2PolygonShape box;
        box.SetAsBox(0.40f, 0.20f);

        b2FixtureDef fix;
        fix.shape = &box;

        // --- Collision filtering ---
        //
        // Every fixture has a b2Filter with categoryBits and maskBits.
        // A fixture A collides with fixture B only if:
        //     (A.maskBits & B.categoryBits) != 0
        //  && (B.maskBits & A.categoryBits) != 0
        //
        // By setting maskBits = 0 on the spawner, NOTHING can collide
        // with it.  This prevents the newly-spawned item (created at
        // the spawner's position) from interpenetrating the spawner
        // and getting launched by the collision response.
        //
        // The spawner is still visible and hit-testable (TestPoint
        // doesn't use the filter) — it just never exerts forces.
        fix.filter.maskBits = 0;
        body->CreateFixture(&fix);

        body->SetUserData(tagFor(kind));
    };

    // Spread along the 4 m shelf with 1.5 m gaps so the 12 pt labels
    // drawn above each spawner don't overlap.
    makeSpawner(1.00f, 4.30f, BodyKind::SpawnerBanana);
    makeSpawner(2.50f, 4.30f, BodyKind::SpawnerPotato);
    makeSpawner(4.00f, 4.30f, BodyKind::SpawnerBottle);
}

void BackpackScene::createCinchAndGoButton()
{
    // Small static ledge for the button to rest on.
    {
        b2BodyDef def;
        def.type = b2_staticBody;
        def.position.Set(2.5f, 1.1f);
        b2Body* body = world.CreateBody(&def);

        b2PolygonShape box;
        box.SetAsBox(0.7f, 0.1f);

        b2FixtureDef fix;
        fix.shape = &box;
        fix.friction = 0.5f;
        body->CreateFixture(&fix);

        body->SetUserData(tagFor(BodyKind::CinchAndGoLedge));
    }

    // The button itself.  It starts as a static body so it sits on
    // the ledge without falling.  When the player clicks it,
    // mousePressEvent flips it to b2_dynamicBody.
    //
    // IMPORTANT: we set density = 1.0 even though static bodies
    // ignore density.  When SetType(b2_dynamicBody) is called later,
    // Box2D recalculates mass from the fixture's density and area.
    // If density were 0, Box2D would fall back to mass = 1.0 (see
    // b2Body.h line 74) which behaves oddly.  Setting density here
    // ensures the mass calculation is based on real geometry.
    b2BodyDef def;
    def.type = b2_staticBody;
    def.position.Set(2.5f, 1.45f);
    def.linearDamping = 0.0f;
    def.angularDamping = 0.5f;
    cinchAndGoButton = world.CreateBody(&def);

    b2PolygonShape box;
    box.SetAsBox(0.5f, 0.25f);

    b2FixtureDef fix;
    fix.shape = &box;
    fix.density = 1.0f;   // see note above
    fix.friction = 0.5f;
    cinchAndGoButton->CreateFixture(&fix);

    cinchAndGoButton->SetUserData(tagFor(BodyKind::CinchAndGoIdle));
}

// =========================================================================
//  Item spawn helpers
// =========================================================================
//
// Each helper creates a dynamic body with one or more convex polygon
// fixtures.  Key Box2D constraints:
//
// - b2PolygonShape vertices MUST be convex and wound counter-
//   clockwise (CCW).  Box2D silently rejects invalid polygons.
//
// - b2_maxPolygonVertices is 8.  Stay at ≤ 6 for safety margin.
//
// - A single body can carry multiple fixtures.  The banana uses two
//   quads to approximate a non-convex crescent; the bottle uses a
//   body rectangle and a cap rectangle.  Box2D treats the union as
//   one rigid object for physics and mass calculation.
//
// - density (kg/m^2) * fixture area (m^2) = contribution to body
//   mass (kg).  A heavier body needs more mouse-joint force to drag.
//
// - linearDamping slows translational velocity each step (0 = no
//   damping, 1.0 = heavy damping).  Helps items settle in the pack
//   instead of bouncing endlessly.
//
// - angularDamping does the same for rotational velocity.

b2Body* BackpackScene::spawnBanana(const b2Vec2& position)
{
    b2BodyDef def;
    def.type = b2_dynamicBody;  // affected by gravity and collisions
    def.position = position;
    def.linearDamping = 0.5f;
    def.angularDamping = 1.0f;
    b2Body* body = world.CreateBody(&def);

    // A real banana crescent is non-convex (concave on one side).
    // Box2D requires convex polygons, so we split the shape into two
    // convex quads that meet along the vertical midline.  The result
    // is ~1.4 m long, tapers at both ends, and bows downward in the
    // middle — a recognizable banana shape.
    b2Vec2 leftHalf[4] = {
        b2Vec2(-0.70f,  0.05f),   // left tip, bottom
        b2Vec2( 0.00f, -0.13f),   // midline, bottom (the dip)
        b2Vec2( 0.00f,  0.07f),   // midline, top
        b2Vec2(-0.70f,  0.10f),   // left tip, top
    };
    b2Vec2 rightHalf[4] = {
        b2Vec2( 0.00f, -0.13f),   // midline, bottom
        b2Vec2( 0.70f,  0.05f),   // right tip, bottom
        b2Vec2( 0.70f,  0.10f),   // right tip, top
        b2Vec2( 0.00f,  0.07f),   // midline, top
    };

    b2PolygonShape leftShape;
    leftShape.Set(leftHalf, 4);   // Set() validates convexity + winding
    b2FixtureDef leftFix;
    leftFix.shape = &leftShape;
    leftFix.density = 0.5f;       // light — it's a banana
    leftFix.friction = 0.5f;
    body->CreateFixture(&leftFix);

    b2PolygonShape rightShape;
    rightShape.Set(rightHalf, 4);
    b2FixtureDef rightFix;
    rightFix.shape = &rightShape;
    rightFix.density = 0.5f;
    rightFix.friction = 0.5f;
    body->CreateFixture(&rightFix);

    body->SetUserData(tagFor(BodyKind::ItemBanana));
    return body;
}

b2Body* BackpackScene::spawnPotato(const b2Vec2& position)
{
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = position;
    def.linearDamping = 0.5f;
    def.angularDamping = 1.0f;
    b2Body* body = world.CreateBody(&def);

    // Single 6-vertex convex blob, slightly lopsided for a natural
    // potato look.  All vertices are wound CCW.
    b2Vec2 verts[6] = {
        b2Vec2(-0.33f, -0.08f),
        b2Vec2(-0.15f, -0.26f),
        b2Vec2( 0.21f, -0.23f),
        b2Vec2( 0.36f,  0.03f),
        b2Vec2( 0.12f,  0.26f),
        b2Vec2(-0.24f,  0.21f),
    };
    b2PolygonShape shape;
    shape.Set(verts, 6);

    b2FixtureDef fix;
    fix.shape = &shape;
    fix.density = 1.5f;   // heavier than the banana — potatoes are dense
    fix.friction = 0.5f;
    body->CreateFixture(&fix);

    body->SetUserData(tagFor(BodyKind::ItemPotato));
    return body;
}

b2Body* BackpackScene::spawnBottle(const b2Vec2& position)
{
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = position;
    def.linearDamping = 0.5f;
    def.angularDamping = 1.0f;
    b2Body* body = world.CreateBody(&def);

    // Two fixtures on one body: a tall rectangle for the body and a
    // narrower rectangle offset upward for the cap.
    //
    // SetAsBox(halfW, halfH) creates a centered box.
    // SetAsBox(halfW, halfH, center, angle) creates an offset box.

    b2PolygonShape bodyRect;
    bodyRect.SetAsBox(0.17f, 0.50f);  // 0.34 m wide, 1.00 m tall
    b2FixtureDef bodyFix;
    bodyFix.shape = &bodyRect;
    bodyFix.density = 1.0f;
    bodyFix.friction = 0.5f;
    body->CreateFixture(&bodyFix);

    b2PolygonShape capRect;
    // Offset the cap 0.57 m above the body center so it sits on top
    // of the main rectangle (main top is at y = +0.50, cap bottom
    // is at 0.57 - 0.07 = 0.50 — flush).
    capRect.SetAsBox(0.10f, 0.07f, b2Vec2(0.0f, 0.57f), 0.0f);
    b2FixtureDef capFix;
    capFix.shape = &capRect;
    capFix.density = 1.0f;
    capFix.friction = 0.5f;
    body->CreateFixture(&capFix);

    body->SetUserData(tagFor(BodyKind::ItemBottle));
    return body;
}

// =========================================================================
//  Picking and dragging
// =========================================================================

b2Body* BackpackScene::findBodyAt(const b2Vec2& worldPos)
{
    // Linear scan of all bodies/fixtures.  b2Fixture::TestPoint
    // returns true if worldPos is inside the fixture's shape.
    //
    // For scenes with hundreds of bodies you'd use
    // b2World::QueryAABB with a callback instead — it queries the
    // broadphase tree and is O(log N) in the body count.  For our
    // ~20 bodies the linear scan is simpler and just as fast.
    for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
            if (f->TestPoint(worldPos)) {
                return body;
            }
        }
    }
    return nullptr;
}

void BackpackScene::beginDragging(b2Body* body, const b2Vec2& worldPos)
{
    if (!body || !mouseJointAnchor) {
        return;
    }
    // If already dragging something, release it first.
    if (mouseJoint) {
        endDragging();
    }
    // Mouse joints only work on dynamic bodies.
    if (body->GetType() != b2_dynamicBody) {
        return;
    }

    // --- b2MouseJointDef ---
    //
    // bodyA:       static anchor (never moves).
    // bodyB:       the item being dragged.
    // target:      the world point the spring pulls bodyB toward
    //              (updated each frame in mouseMoveEvent).
    // maxForce:    maximum force the spring can exert (Newtons).
    //              Set to 1000 * mass so even heavy items respond
    //              quickly.  NOTE: the default maxForce is 0, which
    //              makes the joint silently do nothing — a classic
    //              first-time Box2D trap.
    // frequencyHz: spring oscillation frequency.  5 Hz gives a
    //              responsive but not twitchy feel.
    // dampingRatio: 0.7 is critically-damped — the item reaches the
    //              target quickly without overshooting.
    b2MouseJointDef def;
    def.bodyA = mouseJointAnchor;
    def.bodyB = body;
    def.target = worldPos;
    def.maxForce = 1000.0f * body->GetMass();
    def.frequencyHz = 5.0f;
    def.dampingRatio = 0.7f;

    mouseJoint = static_cast<b2MouseJoint*>(world.CreateJoint(&def));

    // Wake the body.  Box2D puts idle bodies to sleep to save CPU.
    // A sleeping body ignores forces/joints until woken.
    body->SetAwake(true);
}

void BackpackScene::endDragging()
{
    if (mouseJoint) {
        // DestroyJoint removes the joint from the world and frees it.
        // After this, the body is subject only to gravity and friction.
        world.DestroyJoint(mouseJoint);
        mouseJoint = nullptr;
    }
}
