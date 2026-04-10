/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow class
 *
 * Implements the MainWindow class, which handles:
 * - rendering objects using Qt
 * - managing the Box2D physics world
 * - handling mouse input for interaction
 *
 * Connects the UI with the physics simulation so the user
 * can create, move, and interact with items on the screen.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 * Reviewed by Lindsey Reese 4/8/2026
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QColor>
#include <QFont>
#include <QMenuBar>
#include <QPen>
#include <QPolygonF>
#include <QRectF>
#include <QStatusBar>

#include <cstdint>

namespace {

/**
 * @brief Identity tag for every Box2D body in the scene.
 *
 * Stored as the body's user-data pointer by casting the enum value
 * to uintptr_t and then to void*. This round-trip is standard-compliant
 * since C++11 (see [expr.reinterpret.cast] p5).
 */
enum class BodyKind : std::uintptr_t {
    None = 0,
    MouseAnchor,
    WorldBound,
    Shelf,
    SpawnerBanana,
    SpawnerPotato,
    SpawnerBottle,
    PackWall,
    CinchAndGoLedge,
    CinchAndGoIdle,
    CinchAndGoActivated,
    ItemBanana,
    ItemPotato,
    ItemBottle,
};

inline void* tagFor(BodyKind k) {
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(k));
}

inline BodyKind kindOf(const b2Body* b) {
    return static_cast<BodyKind>(reinterpret_cast<std::uintptr_t>(b->GetUserData()));
}

/**
 * @brief Fill color for each body kind.
 */
QColor colorFor(BodyKind kind) {
    switch (kind) {
        case BodyKind::ItemBanana:          return QColor(255, 225, 60);   // yellow
        case BodyKind::ItemPotato:          return QColor(150, 100, 55);   // saddle brown
        case BodyKind::ItemBottle:          return QColor(130, 200, 230);  // light blue

        case BodyKind::SpawnerBanana:       return QColor(220, 200, 100);  // muted yellow
        case BodyKind::SpawnerPotato:       return QColor(140, 105, 70);   // muted brown
        case BodyKind::SpawnerBottle:       return QColor(130, 175, 200);  // muted blue

        case BodyKind::Shelf:               return QColor(120, 85, 45);    // wood
        case BodyKind::CinchAndGoLedge:     return QColor(120, 85, 45);    // wood

        case BodyKind::PackWall:            return QColor(75, 75, 85);     // dark slate
        case BodyKind::WorldBound:          return QColor(55, 55, 55);     // near-black

        case BodyKind::CinchAndGoIdle:      return QColor(80, 180, 90);    // green
        case BodyKind::CinchAndGoActivated: return QColor(50, 130, 60);    // darker green

        case BodyKind::MouseAnchor:         return QColor(0, 0, 0, 0);     // invisible

        case BodyKind::None:
        default:                            return QColor(Qt::blue);
    }
}

/**
 * @brief Label drawn over a body in screen space (unrotated, unscaled).
 *
 * Returns an empty string for bodies that should not carry a label.
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

/**
 * @class MainWindow::DestructionListener
 * @brief Clears MainWindow::mouseJoint when Box2D destroys it.
 *
 * When a body with an attached joint is destroyed, Box2D automatically
 * destroys the joint but leaves any external pointers to it dangling.
 * Receiving SayGoodbye(b2Joint*) lets us null the pointer safely.
 */
class MainWindow::DestructionListener : public b2DestructionListener {
public:
    MainWindow* owner = nullptr;

    void SayGoodbye(b2Joint* joint) override {
        if (owner && joint == owner->mouseJoint) {
            owner->mouseJoint = nullptr;
        }
    }
    void SayGoodbye(b2Fixture*) override {}
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , world(b2Vec2(0.0f, -10.0f))
    , timer(new QTimer(this))
{
    ui->setupUi(this);

    // Lock the window geometry so the screen <-> world math is stable.
    setFixedSize(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX);
    menuBar()->hide();
    statusBar()->hide();

    // Register a destruction listener so a destroyed body never leaves
    // us with a dangling mouseJoint pointer.
    destructionListener = std::make_unique<DestructionListener>();
    destructionListener->owner = this;
    world.SetDestructionListener(destructionListener.get());

    // Build the scene.
    createMouseAnchor();
    createWorldBounds();
    createShelf();
    createBackpack();
    createCinchAndGoButton();
    createSpawners();

    // Connect the timer to the updatePhysics slot
    connect(timer, &QTimer::timeout, this, &MainWindow::updatePhysics);

    // Decides how often the screen gets repainted
    timer->start(1000 / REFRESH_RATE);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Cosmetic pen keeps outlines at 1 px regardless of the world-space
    // scale transform applied below.
    QPen outlinePen(Qt::black);
    outlinePen.setCosmetic(true);

    QFont labelFont = painter.font();
    labelFont.setBold(true);

    for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
        BodyKind kind = kindOf(body);
        QColor fill = colorFor(kind);

        // Skip fully-transparent bodies (e.g. the mouse anchor).
        if (fill.alpha() == 0) {
            continue;
        }

        QPointF screenPos = worldToScreen(body->GetPosition());

        // Draw the body's fixtures in world-space, scaled to pixels.
        painter.save();
        painter.translate(screenPos);
        painter.rotate(-body->GetAngle() * 180.0f / b2_pi);
        painter.scale(PIXELS_PER_METER, PIXELS_PER_METER);
        painter.setBrush(fill);
        painter.setPen(outlinePen);

        for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
            if (fixture->GetShape()->GetType() != b2Shape::e_polygon) {
                continue;
            }
            auto* poly = static_cast<b2PolygonShape*>(fixture->GetShape());
            QPolygonF qPoly;
            for (int32 i = 0; i < poly->GetVertexCount(); ++i) {
                b2Vec2 v = poly->GetVertex(i);
                // Local-space Y flip: Box2D uses Y-up, Qt uses Y-down.
                qPoly << QPointF(v.x, -v.y);
            }
            painter.drawPolygon(qPoly);
        }

        painter.restore();

        // Labels are drawn after restore() so they never rotate or scale.
        QString label = labelFor(kind);
        if (!label.isEmpty()) {
            painter.setFont(labelFont);
            painter.setPen(Qt::black);
            QRectF labelRect(screenPos.x() - 70.0, screenPos.y() - 12.0, 140.0, 24.0);
            painter.drawText(labelRect, Qt::AlignCenter, label);
        }
    }
}

b2Vec2 MainWindow::screenToWorld(const QPoint& point)
{
    // Flip Y so the bottom of the screen is y = 0, then scale pixels -> meters.
    return b2Vec2(point.x() / PIXELS_PER_METER,
                  (WINDOW_HEIGHT_PX - point.y()) / PIXELS_PER_METER);
}

QPointF MainWindow::worldToScreen(const b2Vec2& point)
{
    // Scale meters -> pixels, then flip Y so Box2D's y = 0 lands at the bottom.
    return QPointF(point.x * PIXELS_PER_METER,
                   WINDOW_HEIGHT_PX - point.y * PIXELS_PER_METER);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    const b2Vec2 worldPos = screenToWorld(event->pos());
    b2Body* hit = findBodyAt(worldPos);
    if (!hit) {
        return;
    }

    switch (kindOf(hit)) {
        case BodyKind::SpawnerBanana: {
            b2Vec2 spawnPos = hit->GetPosition();
            spawnPos.y += 0.5f;  // spawn just above the spawner
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

        case BodyKind::CinchAndGoIdle: {
            // Flip the button from static to dynamic, retag, and launch it.
            // Mass is recomputed from fixture density (set to 1.0 at creation).
            hit->SetType(b2_dynamicBody);
            hit->SetAwake(true);
            hit->SetLinearVelocity(b2Vec2(0.0f, 15.0f));
            hit->SetUserData(tagFor(BodyKind::CinchAndGoActivated));
            break;
        }

        case BodyKind::ItemBanana:
        case BodyKind::ItemPotato:
        case BodyKind::ItemBottle:
        case BodyKind::CinchAndGoActivated:
            beginDragging(hit, worldPos);
            break;

        default:
            break;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (mouseJoint) {
        mouseJoint->SetTarget(screenToWorld(event->pos()));
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        endDragging();
    }
}

void MainWindow::updatePhysics()
{
    // Step the simulation and request a repaint
    world.Step(1.0f / static_cast<float>(REFRESH_RATE) , 8, 3);

    // Watch for the Cinch & Go button exiting the visible area.
    // This check runs AFTER the step, so destroying the body here is safe.
    if (!levelStarted && cinchAndGoButton
        && kindOf(cinchAndGoButton) == BodyKind::CinchAndGoActivated) {

        const b2Vec2 pos = cinchAndGoButton->GetPosition();
        const bool offScreen = pos.y > WORLD_HEIGHT_M + 2.0f
                               || pos.x < -2.0f
                               || pos.x > WORLD_WIDTH_M + 2.0f
                               || pos.y < -2.0f;

        if (offScreen) {
            levelStarted = true;
            if (mouseJoint && mouseJoint->GetBodyB() == cinchAndGoButton) {
                endDragging();
            }
            world.DestroyBody(cinchAndGoButton);
            cinchAndGoButton = nullptr;
            emit levelHasStarted();
        }
    }

    update();
}

// ---------------------------------------------------------------------------
// Scene construction
// ---------------------------------------------------------------------------

void MainWindow::createMouseAnchor()
{
    // Invisible static body used as bodyA for the mouse joint.
    b2BodyDef def;
    def.type = b2_staticBody;
    def.position.Set(0.0f, 0.0f);
    mouseJointAnchor = world.CreateBody(&def);
    mouseJointAnchor->SetUserData(tagFor(BodyKind::MouseAnchor));
    // No fixtures — the anchor should never collide or be picked up.
}

void MainWindow::createWorldBounds()
{
    auto makeWall = [this](float cx, float cy, float halfW, float halfH) {
        b2BodyDef def;
        def.type = b2_staticBody;
        def.position.Set(cx, cy);
        b2Body* body = world.CreateBody(&def);

        b2PolygonShape box;
        box.SetAsBox(halfW, halfH);

        b2FixtureDef fix;
        fix.shape = &box;
        fix.friction = 0.5f;
        body->CreateFixture(&fix);

        body->SetUserData(tagFor(BodyKind::WorldBound));
    };

    // Ground spans the full window width, sitting at the bottom.
    makeWall(WORLD_WIDTH_M * 0.5f, 0.1f, WORLD_WIDTH_M * 0.5f, 0.1f);
    // Left and right walls. No top wall — the Cinch & Go button escapes up.
    makeWall(0.1f, WORLD_HEIGHT_M * 0.5f, 0.1f, WORLD_HEIGHT_M * 0.5f);
    makeWall(WORLD_WIDTH_M - 0.1f, WORLD_HEIGHT_M * 0.5f, 0.1f, WORLD_HEIGHT_M * 0.5f);
}

void MainWindow::createShelf()
{
    b2BodyDef def;
    def.type = b2_staticBody;
    def.position.Set(2.5f, 4.0f);
    b2Body* body = world.CreateBody(&def);

    b2PolygonShape box;
    box.SetAsBox(2.0f, 0.1f);

    b2FixtureDef fix;
    fix.shape = &box;
    fix.friction = 0.6f;
    body->CreateFixture(&fix);

    body->SetUserData(tagFor(BodyKind::Shelf));
}

void MainWindow::createBackpack()
{
    auto makePackWall = [this](float cx, float cy, float halfW, float halfH) {
        b2BodyDef def;
        def.type = b2_staticBody;
        def.position.Set(cx, cy);
        b2Body* body = world.CreateBody(&def);

        b2PolygonShape box;
        box.SetAsBox(halfW, halfH);

        b2FixtureDef fix;
        fix.shape = &box;
        fix.friction = 0.6f;
        body->CreateFixture(&fix);

        body->SetUserData(tagFor(BodyKind::PackWall));
    };

    // Left wall, right wall, and bottom forming a U open at the top.
    makePackWall(10.5f, 3.25f, 0.1f, 2.25f);
    makePackWall(13.5f, 3.25f, 0.1f, 2.25f);
    makePackWall(12.0f, 1.0f,  1.6f, 0.1f);
}

void MainWindow::createSpawners()
{
    auto makeSpawner = [this](float cx, float cy, BodyKind kind) {
        b2BodyDef def;
        def.type = b2_staticBody;
        def.position.Set(cx, cy);
        b2Body* body = world.CreateBody(&def);

        b2PolygonShape box;
        box.SetAsBox(0.25f, 0.15f);

        b2FixtureDef fix;
        fix.shape = &box;
        // maskBits = 0 means nothing collides with the spawner — newly
        // spawned items pass right through instead of exploding on birth.
        fix.filter.maskBits = 0;
        body->CreateFixture(&fix);

        body->SetUserData(tagFor(kind));
    };

    makeSpawner(1.25f, 4.25f, BodyKind::SpawnerBanana);
    makeSpawner(2.50f, 4.25f, BodyKind::SpawnerPotato);
    makeSpawner(3.75f, 4.25f, BodyKind::SpawnerBottle);
}

void MainWindow::createCinchAndGoButton()
{
    // A small static ledge for the button to rest on.
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

    // The button itself. Starts static. Density is > 0 so that when
    // SetType(b2_dynamicBody) fires later, Box2D can recompute a real mass
    // from the fixture (density 0 on a dynamic body yields the fallback
    // mass = 1.0, which moves oddly).
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
    fix.density = 1.0f;
    fix.friction = 0.5f;
    cinchAndGoButton->CreateFixture(&fix);

    cinchAndGoButton->SetUserData(tagFor(BodyKind::CinchAndGoIdle));
}

// ---------------------------------------------------------------------------
// Item spawn helpers
// ---------------------------------------------------------------------------

b2Body* MainWindow::spawnBanana(const b2Vec2& position)
{
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = position;
    def.linearDamping = 0.5f;
    def.angularDamping = 1.0f;
    b2Body* body = world.CreateBody(&def);

    // Six-vertex convex polygon approximating a curved banana blob.
    // Counter-clockwise, all within b2_maxPolygonVertices (8).
    b2Vec2 verts[6] = {
        b2Vec2(-0.30f, -0.02f),
        b2Vec2(-0.15f, -0.12f),
        b2Vec2( 0.15f, -0.12f),
        b2Vec2( 0.30f,  0.02f),
        b2Vec2( 0.10f,  0.14f),
        b2Vec2(-0.10f,  0.14f),
    };
    b2PolygonShape shape;
    shape.Set(verts, 6);

    b2FixtureDef fix;
    fix.shape = &shape;
    fix.density = 0.5f;
    fix.friction = 0.5f;
    body->CreateFixture(&fix);

    body->SetUserData(tagFor(BodyKind::ItemBanana));
    return body;
}

b2Body* MainWindow::spawnPotato(const b2Vec2& position)
{
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = position;
    def.linearDamping = 0.5f;
    def.angularDamping = 1.0f;
    b2Body* body = world.CreateBody(&def);

    // Six-vertex convex blob, slightly lopsided.
    b2Vec2 verts[6] = {
        b2Vec2(-0.22f, -0.05f),
        b2Vec2(-0.10f, -0.17f),
        b2Vec2( 0.14f, -0.15f),
        b2Vec2( 0.24f,  0.02f),
        b2Vec2( 0.08f,  0.17f),
        b2Vec2(-0.16f,  0.14f),
    };
    b2PolygonShape shape;
    shape.Set(verts, 6);

    b2FixtureDef fix;
    fix.shape = &shape;
    fix.density = 1.5f;
    fix.friction = 0.5f;
    body->CreateFixture(&fix);

    body->SetUserData(tagFor(BodyKind::ItemPotato));
    return body;
}

b2Body* MainWindow::spawnBottle(const b2Vec2& position)
{
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = position;
    def.linearDamping = 0.5f;
    def.angularDamping = 1.0f;
    b2Body* body = world.CreateBody(&def);

    // Two-fixture water bottle: tall main body + small cap offset upward.
    b2PolygonShape bodyRect;
    bodyRect.SetAsBox(0.12f, 0.35f);
    b2FixtureDef bodyFix;
    bodyFix.shape = &bodyRect;
    bodyFix.density = 1.0f;
    bodyFix.friction = 0.5f;
    body->CreateFixture(&bodyFix);

    b2PolygonShape capRect;
    capRect.SetAsBox(0.07f, 0.05f, b2Vec2(0.0f, 0.40f), 0.0f);
    b2FixtureDef capFix;
    capFix.shape = &capRect;
    capFix.density = 1.0f;
    capFix.friction = 0.5f;
    body->CreateFixture(&capFix);

    body->SetUserData(tagFor(BodyKind::ItemBottle));
    return body;
}

// ---------------------------------------------------------------------------
// Picking and dragging
// ---------------------------------------------------------------------------

b2Body* MainWindow::findBodyAt(const b2Vec2& worldPos)
{
    for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
            if (f->TestPoint(worldPos)) {
                return body;
            }
        }
    }
    return nullptr;
}

void MainWindow::beginDragging(b2Body* body, const b2Vec2& worldPos)
{
    if (!body || !mouseJointAnchor) {
        return;
    }
    if (mouseJoint) {
        endDragging();
    }
    if (body->GetType() != b2_dynamicBody) {
        return;
    }

    b2MouseJointDef def;
    def.bodyA = mouseJointAnchor;
    def.bodyB = body;
    def.target = worldPos;
    // maxForce defaults to 0 in Box2D, which would make the joint silently
    // do nothing. Scale with mass so heavy items are still draggable.
    def.maxForce = 1000.0f * body->GetMass();
    def.frequencyHz = 5.0f;
    def.dampingRatio = 0.7f;

    mouseJoint = static_cast<b2MouseJoint*>(world.CreateJoint(&def));
    body->SetAwake(true);
}

void MainWindow::endDragging()
{
    if (mouseJoint) {
        world.DestroyJoint(mouseJoint);
        mouseJoint = nullptr;
    }
}
