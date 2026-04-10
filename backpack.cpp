/**
 * @file backpack.cpp
 * @brief Implementation of the BackpackScene widget.
 *
 * Implements the packing-phase scene: Box2D world, item rendering,
 * mouse-based drag-and-drop, spawners, Cinch & Go button, and
 * bag-entry/exit signal emission.
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
    PackSensor,
    CinchAndGoLedge,
    CinchAndGoIdle,
    CinchAndGoActivated,
    ItemBanana,
    ItemPotato,
    ItemBottle,
};

inline bool isItemKind(BodyKind kind) {
    return kind == BodyKind::ItemBanana
        || kind == BodyKind::ItemPotato
        || kind == BodyKind::ItemBottle;
}

inline QString itemNameOf(BodyKind kind) {
    switch (kind) {
        case BodyKind::ItemBanana: return QStringLiteral("BANANA");
        case BodyKind::ItemPotato: return QStringLiteral("POTATO");
        case BodyKind::ItemBottle: return QStringLiteral("BOTTLE");
        default:                   return QString();
    }
}

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
        case BodyKind::PackSensor:          return QColor(0, 0, 0, 0);     // invisible

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
 * @class BackpackScene::DestructionListener
 * @brief Clears BackpackScene::mouseJoint when Box2D destroys it.
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
 * @brief Turns pack-sensor contacts into itemAddedToBag / itemRemovedFromBag.
 *
 * Multi-fixture items generate one BeginContact per fixture-sensor pair.
 * A per-body contact counter de-duplicates so only one signal fires per
 * true entry/exit.
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
    b2Body* itemBodyFromContact(b2Contact* contact) const {
        b2Body* a = contact->GetFixtureA()->GetBody();
        b2Body* b = contact->GetFixtureB()->GetBody();
        const BodyKind ka = kindOf(a);
        const BodyKind kb = kindOf(b);
        if (ka == BodyKind::PackSensor && isItemKind(kb)) return b;
        if (kb == BodyKind::PackSensor && isItemKind(ka)) return a;
        return nullptr;
    }

    std::unordered_map<b2Body*, int> inBagCount;
};

BackpackScene::BackpackScene(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BackpackScene)
    , world(b2Vec2(0.0f, -10.0f))
    , timer(new QTimer(this))
{
    ui->setupUi(this);

    // Register a destruction listener so a destroyed body never leaves
    // us with a dangling mouseJoint pointer.
    destructionListener = std::make_unique<DestructionListener>();
    destructionListener->owner = this;
    world.SetDestructionListener(destructionListener.get());

    // Register a contact listener that turns pack-sensor contacts into
    // itemAddedToBag / itemRemovedFromBag signals.
    contactListener = std::make_unique<ContactListener>();
    contactListener->owner = this;
    world.SetContactListener(contactListener.get());

    // Temporary: log add/remove events to stdout so we can watch the
    // signals fire until a real subscriber exists.
    connect(this, &BackpackScene::itemAddedToBag, this, [](const QString& name) {
        std::cout << "Added to bag: " << name.toStdString() << std::endl;
    });
    connect(this, &BackpackScene::itemRemovedFromBag, this, [](const QString& name) {
        std::cout << "Removed from bag: " << name.toStdString() << std::endl;
    });

    // Build the scene. No world bounds — items that fall off the screen
    // are destroyed by updatePhysics() instead.
    createMouseAnchor();
    createShelf();
    createBackpack();
    createCinchAndGoButton();
    createSpawners();

    // Connect the timer to the updatePhysics slot
    connect(timer, &QTimer::timeout, this, &BackpackScene::updatePhysics);

    // Decides how often the screen gets repainted
    timer->start(1000 / REFRESH_RATE);
}

BackpackScene::~BackpackScene()
{
    // Detach Box2D listeners before our nested listener objects are
    // torn down by member destruction.
    world.SetContactListener(nullptr);
    world.SetDestructionListener(nullptr);
    delete ui;
}

void BackpackScene::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen outlinePen(Qt::black);
    outlinePen.setCosmetic(true);

    QFont labelFont = painter.font();
    labelFont.setBold(true);
    labelFont.setPointSize(12);

    for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
        BodyKind kind = kindOf(body);
        QColor fill = colorFor(kind);

        if (fill.alpha() == 0) {
            continue;
        }

        QPointF screenPos = worldToScreen(body->GetPosition());

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
                qPoly << QPointF(v.x, -v.y);
            }
            painter.drawPolygon(qPoly);
        }

        painter.restore();

        QString label = labelFor(kind);
        if (!label.isEmpty()) {
            painter.setFont(labelFont);
            painter.setPen(Qt::white);
            QRectF labelRect(screenPos.x() - 80.0, screenPos.y() - 42.0, 160.0, 22.0);
            painter.drawText(labelRect, Qt::AlignCenter, label);
        }
    }
}

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

void BackpackScene::mousePressEvent(QMouseEvent *event)
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

        case BodyKind::CinchAndGoIdle: {
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

void BackpackScene::mouseMoveEvent(QMouseEvent *event)
{
    if (mouseJoint) {
        mouseJoint->SetTarget(screenToWorld(event->pos()));
    }
}

void BackpackScene::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        endDragging();
    }
}

void BackpackScene::updatePhysics()
{
    world.Step(1.0f / static_cast<float>(REFRESH_RATE) , 8, 3);

    constexpr float EXIT_MARGIN = 2.0f;

    std::vector<b2Body*> toDestroy;
    for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
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

    update();
}

// ---------------------------------------------------------------------------
// Scene construction
// ---------------------------------------------------------------------------

void BackpackScene::createMouseAnchor()
{
    b2BodyDef def;
    def.type = b2_staticBody;
    def.position.Set(0.0f, 0.0f);
    mouseJointAnchor = world.CreateBody(&def);
    mouseJointAnchor->SetUserData(tagFor(BodyKind::MouseAnchor));
}

void BackpackScene::createShelf()
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

void BackpackScene::createBackpack()
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

    makePackWall(11.0f, 3.25f, 0.1f, 2.25f);
    makePackWall(13.0f, 3.25f, 0.1f, 2.25f);
    makePackWall(12.0f, 1.0f,  1.1f, 0.1f);

    b2BodyDef sensorDef;
    sensorDef.type = b2_staticBody;
    sensorDef.position.Set(12.0f, 3.3f);
    packSensor = world.CreateBody(&sensorDef);

    b2PolygonShape sensorBox;
    sensorBox.SetAsBox(0.9f, 2.2f);

    b2FixtureDef sensorFix;
    sensorFix.shape = &sensorBox;
    sensorFix.isSensor = true;
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
        fix.filter.maskBits = 0;
        body->CreateFixture(&fix);

        body->SetUserData(tagFor(kind));
    };

    makeSpawner(1.00f, 4.30f, BodyKind::SpawnerBanana);
    makeSpawner(2.50f, 4.30f, BodyKind::SpawnerPotato);
    makeSpawner(4.00f, 4.30f, BodyKind::SpawnerBottle);
}

void BackpackScene::createCinchAndGoButton()
{
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

b2Body* BackpackScene::spawnBanana(const b2Vec2& position)
{
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = position;
    def.linearDamping = 0.5f;
    def.angularDamping = 1.0f;
    b2Body* body = world.CreateBody(&def);

    b2Vec2 leftHalf[4] = {
        b2Vec2(-0.70f,  0.05f),
        b2Vec2( 0.00f, -0.13f),
        b2Vec2( 0.00f,  0.07f),
        b2Vec2(-0.70f,  0.10f),
    };
    b2Vec2 rightHalf[4] = {
        b2Vec2( 0.00f, -0.13f),
        b2Vec2( 0.70f,  0.05f),
        b2Vec2( 0.70f,  0.10f),
        b2Vec2( 0.00f,  0.07f),
    };

    b2PolygonShape leftShape;
    leftShape.Set(leftHalf, 4);
    b2FixtureDef leftFix;
    leftFix.shape = &leftShape;
    leftFix.density = 0.5f;
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
    fix.density = 1.5f;
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

    b2PolygonShape bodyRect;
    bodyRect.SetAsBox(0.17f, 0.50f);
    b2FixtureDef bodyFix;
    bodyFix.shape = &bodyRect;
    bodyFix.density = 1.0f;
    bodyFix.friction = 0.5f;
    body->CreateFixture(&bodyFix);

    b2PolygonShape capRect;
    capRect.SetAsBox(0.10f, 0.07f, b2Vec2(0.0f, 0.57f), 0.0f);
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

b2Body* BackpackScene::findBodyAt(const b2Vec2& worldPos)
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

void BackpackScene::beginDragging(b2Body* body, const b2Vec2& worldPos)
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
    def.maxForce = 1000.0f * body->GetMass();
    def.frequencyHz = 5.0f;
    def.dampingRatio = 0.7f;

    mouseJoint = static_cast<b2MouseJoint*>(world.CreateJoint(&def));
    body->SetAwake(true);
}

void BackpackScene::endDragging()
{
    if (mouseJoint) {
        world.DestroyJoint(mouseJoint);
        mouseJoint = nullptr;
    }
}
