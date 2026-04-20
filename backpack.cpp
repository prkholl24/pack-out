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
 *          Brady Nelson, Lindsey Reese, Kent Wilkison, Claude Opus 4.6 (1M context)
 * @date 4/9/2026
 */
#include "backpack.h"
#include "spritecache.h"
#include "theme.h"
#include "ui_backpack.h"
#include "itemregistry.h"

#include <QColor>
#include <QFont>
#include <QPen>
#include <QPolygonF>
#include <QRandomGenerator>
#include <QRectF>

#include <algorithm>
#include <cmath>
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
 * Uses a generic Spawner/Item/Tab approach instead of per-item
 * entries.  The specific item name is stored in bodyNames.
 */
enum class BodyKind : std::uintptr_t {
    None = 0,
    MouseAnchor,
    WorldBound,
    Shelf,
    Spawner,                ///< Generic spawner button (any item).
    Tab,                    ///< Category tab button (food/drink/gear).
    PackWall,
    PackSensor,
    CinchAndGoLedge,
    CinchAndGoIdle,
    CinchAndGoActivated,
    Item,                   ///< Dynamic item body (any type).
};

inline bool isItemKind(BodyKind kind) {
    return kind == BodyKind::Item;
}

inline void* tagFor(BodyKind k) {
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(k));
}

inline BodyKind kindOf(const b2Body* b) {
    return static_cast<BodyKind>(reinterpret_cast<std::uintptr_t>(b->GetUserData()));
}

/**
 * @brief Returns the fill color for structural bodies.
 * Item and Spawner colors come from ItemRegistry shape data instead.
 */
QColor colorFor(BodyKind kind) {
    switch (kind) {
        case BodyKind::Shelf:               return QColor(120, 85, 45);
        case BodyKind::CinchAndGoLedge:     return QColor(120, 85, 45);
        case BodyKind::PackWall:            return QColor(75, 75, 85);
        case BodyKind::WorldBound:          return QColor(55, 55, 55);
        case BodyKind::CinchAndGoIdle:      return QColor(80, 180, 90);
        case BodyKind::CinchAndGoActivated: return QColor(50, 130, 60);
        case BodyKind::MouseAnchor:         return QColor(0, 0, 0, 0);
        case BodyKind::PackSensor:          return QColor(0, 0, 0, 0);
        case BodyKind::Spawner:             return QColor(100, 100, 100);
        case BodyKind::Tab:                 return QColor(70, 70, 80);
        case BodyKind::Item:                return QColor(180, 180, 180);
        case BodyKind::None:
        default:                            return QColor(Qt::blue);
    }
}

QString labelFor(BodyKind kind) {
    switch (kind) {
        case BodyKind::CinchAndGoIdle: return QStringLiteral("Go!");
        default:                       return QString();
    }
}

}  // namespace

// =========================================================================
//  Box2D listener classes (nested in BackpackScene)
// =========================================================================

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

class BackpackScene::ContactListener : public b2ContactListener {
public:
    BackpackScene* owner = nullptr;

    void BeginContact(b2Contact* contact) override {
        b2Body* itemBody = itemBodyFromContact(contact);
        if (!itemBody) return;
        int& count = inBagCount[itemBody];
        if (count == 0 && owner) {
            auto it = owner->bodyNames.find(itemBody);
            if (it != owner->bodyNames.end())
                emit owner->itemAddedToBag(it->second);
        }
        ++count;
    }

    void EndContact(b2Contact* contact) override {
        b2Body* itemBody = itemBodyFromContact(contact);
        if (!itemBody) return;
        auto it = inBagCount.find(itemBody);
        if (it == inBagCount.end()) return;
        if (--it->second <= 0) {
            if (owner) {
                auto nameIt = owner->bodyNames.find(itemBody);
                if (nameIt != owner->bodyNames.end())
                    emit owner->itemRemovedFromBag(nameIt->second);
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

// =========================================================================
//  Constructor / destructor
// =========================================================================

BackpackScene::BackpackScene(const QStringList& availableItems, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BackpackScene)
    , world(b2Vec2(0.0f, -10.0f))
    , timer(new QTimer(this))
    , allAvailableItems(availableItems)
{
    ui->setupUi(this);

    // Categorize available items
    for (const QString& name : allAvailableItems) {
        QString cat = ItemRegistry::categoryOf(name);
        if (cat == "food")       foodItems.append(name);
        else if (cat == "drink") drinkItems.append(name);
        else if (cat == "gear")  gearItems.append(name);
    }

    // Pick the first non-empty category as the default tab
    if (!foodItems.isEmpty())       activeTab = "food";
    else if (!drinkItems.isEmpty()) activeTab = "drink";
    else if (!gearItems.isEmpty())  activeTab = "gear";
    else                            activeTab = "none";

    // --- Box2D listener registration ---
    destructionListener = std::make_unique<DestructionListener>();
    destructionListener->owner = this;
    world.SetDestructionListener(destructionListener.get());

    contactListener = std::make_unique<ContactListener>();
    contactListener->owner = this;
    world.SetContactListener(contactListener.get());

    // Debug logging (temporary)
    connect(this, &BackpackScene::itemAddedToBag, this, [](const QString& name) {
        std::cout << "Added to bag: " << name.toStdString() << std::endl;
    });
    connect(this, &BackpackScene::itemRemovedFromBag, this, [](const QString& name) {
        std::cout << "Removed from bag: " << name.toStdString() << std::endl;
    });

    // --- Scene construction ---
    createMouseAnchor();
    createBackpack();
    createCinchAndGoButton();
    createTabs();
    showCategory("food");

    // Enable mouse tracking for hover tooltips
    setMouseTracking(true);

    // --- Game loop ---
    connect(timer, &QTimer::timeout, this, &BackpackScene::updatePhysics);
    timer->start(1000 / REFRESH_RATE);
}

BackpackScene::~BackpackScene()
{
    world.SetContactListener(nullptr);
    world.SetDestructionListener(nullptr);
    bodyNames.clear();
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

    // Keep the packing scene background neutral and low-contrast.
    painter.fillRect(rect(), QColor("#b0acae"));

    // Themed inset border around the scene.
    QPen borderPen(themePrimary.darker(115));
    borderPen.setWidth(6);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect().adjusted(3, 3, -3, -3));

    QPen outlinePen(Qt::black);
    outlinePen.setCosmetic(true);

    QFont labelFont = painter.font();
    labelFont.setBold(true);
    labelFont.setPointSize(18);

    for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
        BodyKind kind = kindOf(body);

        // Determine fill color — items and spawners use per-item shape color
        QColor fill;
        if (kind == BodyKind::Item || kind == BodyKind::Spawner) {
            auto nameIt = bodyNames.find(body);
            if (nameIt != bodyNames.end()) {
                QString cat = ItemRegistry::categoryOf(nameIt->second);
                ItemStats stats;
                if (cat == "food")       stats = ItemRegistry::getFoodStats(nameIt->second);
                else if (cat == "drink") stats = ItemRegistry::getDrinkStats(nameIt->second);
                else if (cat == "gear")  stats = ItemRegistry::getGearStats(nameIt->second);
                fill = stats.shape.color.isValid() ? stats.shape.color : colorFor(kind);
            } else {
                fill = colorFor(kind);
            }
        } else if (kind == BodyKind::Tab) {
            // Highlight active tab using theme primary
            auto nameIt = bodyNames.find(body);
            if (nameIt != bodyNames.end() && nameIt->second == activeTab)
                fill = themePrimary;
            else
                fill = themePrimary.darker(160);
        } else if (kind == BodyKind::CinchAndGoIdle) {
            fill = themePrimary;
        } else if (kind == BodyKind::CinchAndGoActivated) {
            fill = themePrimary.darker(120);
        } else {
            fill = colorFor(kind);
        }

        if (fill.alpha() == 0) continue;

        QPointF screenPos = worldToScreen(body->GetPosition());

        // --- Try sprite rendering for items and spawners ---
        bool drewSprite = false;
        if (kind == BodyKind::Item || kind == BodyKind::Spawner) {
            auto nameIt = bodyNames.find(body);
            if (nameIt != bodyNames.end() && SpriteCache::has(nameIt->second)) {
                const SpriteEntry& sprite = SpriteCache::get(nameIt->second);
                painter.save();
                painter.translate(screenPos);
                painter.rotate(-body->GetAngle() * 180.0f / b2_pi);
                // Map the sprite image to the AABB in screen pixels.
                // Y is negated because Box2D Y-up → screen Y-down.
                QRectF target(
                    sprite.aabb.minX * PIXELS_PER_METER,
                    -sprite.aabb.maxY * PIXELS_PER_METER,
                    sprite.aabb.width() * PIXELS_PER_METER,
                    sprite.aabb.height() * PIXELS_PER_METER
                );
                painter.drawPixmap(target, sprite.pixmap, QRectF(sprite.pixmap.rect()));
                painter.restore();
                drewSprite = true;
            }
        }

        // --- Fallback: polygon rendering ---
        if (!drewSprite) {
            painter.save();
            painter.translate(screenPos);
            painter.rotate(-body->GetAngle() * 180.0f / b2_pi);
            painter.scale(PIXELS_PER_METER, PIXELS_PER_METER);
            painter.setBrush(fill);
            painter.setPen(outlinePen);

            for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
                if (fixture->GetShape()->GetType() != b2Shape::e_polygon) continue;
                auto* poly = static_cast<b2PolygonShape*>(fixture->GetShape());
                QPolygonF qPoly;
                for (int32 i = 0; i < poly->GetVertexCount(); ++i) {
                    b2Vec2 v = poly->GetVertex(i);
                    qPoly << QPointF(v.x, -v.y);
                }
                painter.drawPolygon(qPoly);
            }

            painter.restore();
        }

        // "Go!" label on the Cinch & Go button
        QString label = labelFor(kind);
        if (!label.isEmpty()) {
            QFont goFont = labelFont;
            goFont.setPointSize(26);
            painter.setFont(goFont);
            painter.setPen(themeOnPrim);
            QRectF labelRect(screenPos.x() - 120.0, screenPos.y() - 24.0, 240.0, 48.0);
            painter.drawText(labelRect, Qt::AlignCenter, label);
        }

        // Tab labels
        if (kind == BodyKind::Tab) {
            auto nameIt = bodyNames.find(body);
            if (nameIt != bodyNames.end()) {
                painter.setFont(labelFont);
                painter.setPen(themeOnPrim);
                QString display = nameIt->second;
                display[0] = display[0].toUpper();
                QRectF labelRect(screenPos.x() - 110.0, screenPos.y() - 20.0, 220.0, 40.0);
                painter.drawText(labelRect, Qt::AlignCenter, display);
            }
        }
    }

    // Draw Backpack Totals (top-right HUD card)
    {
        QFont hudFont = painter.font();
        hudFont.setBold(true);
        hudFont.setPointSize(20);
        painter.setFont(hudFont);

        const int hudW = 340;
        const int hudH = 140;
        const QRect hudRect(width() - hudW - 32,
                            32,
                            hudW, hudH);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(themePrimary.red(),
                                themePrimary.green(),
                                themePrimary.blue(), 235));
        painter.drawRoundedRect(hudRect, 18, 18);

        painter.setPen(themeOnPrim);
        painter.drawText(hudRect.adjusted(24, 16, -24, -hudH / 2),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         QString("Weight  %1 lbs").arg(currentWeight, 0, 'f', 1));
        painter.drawText(hudRect.adjusted(24, hudH / 2, -24, -16),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         QString("Volume  %1 L").arg(currentVolume, 0, 'f', 1));
    }

    // Draw Tooltip ---
    if (hoveredBody && kindOf(hoveredBody) == BodyKind::Spawner) {
        auto nameIt = bodyNames.find(hoveredBody);
        if (nameIt != bodyNames.end()) {
            const QString& itemName = nameIt->second;
            QString cat = ItemRegistry::categoryOf(itemName);
            ItemStats stats;
            if (cat == "food")       stats = ItemRegistry::getFoodStats(itemName);
            else if (cat == "drink") stats = ItemRegistry::getDrinkStats(itemName);
            else if (cat == "gear")  stats = ItemRegistry::getGearStats(itemName);

            QPointF bodyScreen = worldToScreen(hoveredBody->GetPosition());

            // Build tooltip text
            QString tip = itemName.toLower().replace("_", " ");
            tip[0] = tip[0].toUpper();
            tip += QString("\nWeight: %1 lbs  Vol: %2 L").arg(stats.weight, 0, 'f', 2).arg(stats.volume, 0, 'f', 1);
            if (!stats.description.isEmpty())
                tip += "\n" + stats.description;

            // Measure and position tooltip
            QFont tipFont = painter.font();
            tipFont.setPointSize(9);
            tipFont.setBold(false);
            painter.setFont(tipFont);
            QFontMetrics fm(tipFont);
            QRect tipBounds = fm.boundingRect(QRect(0, 0, 220, 200), Qt::TextWordWrap, tip);
            tipBounds.adjust(-6, -4, 6, 4);

            // Position above the spawner
            int tipX = static_cast<int>(bodyScreen.x()) - tipBounds.width() / 2;
            int tipY = static_cast<int>(bodyScreen.y()) - tipBounds.height() - 30;
            // Keep on screen
            tipX = std::max(2, std::min(tipX, WINDOW_WIDTH_PX - tipBounds.width() - 2));
            tipY = std::max(2, tipY);

            tipBounds.moveTo(tipX, tipY);

            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(30, 30, 40, 220));
            painter.drawRoundedRect(tipBounds, 4, 4);
            painter.setPen(Qt::white);
            painter.drawText(tipBounds.adjusted(6, 4, -6, -4), Qt::TextWordWrap, tip);
        }
    }
}

void BackpackScene::updateStats(double weight, double volume)
{
    currentWeight = weight;
    currentVolume = volume;
}

// =========================================================================
//  Mouse interaction
// =========================================================================

void BackpackScene::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;

    const b2Vec2 worldPos = screenToWorld(event->pos());
    b2Body* hit = findBodyAt(worldPos);
    if (!hit) return;

    switch (kindOf(hit)) {

        // --- Spawner: create item and begin dragging ---
        case BodyKind::Spawner: {
            auto nameIt = bodyNames.find(hit);
            if (nameIt == bodyNames.end()) break;
            b2Vec2 spawnPos = hit->GetPosition();
            spawnPos.y += 0.5f;
            b2Body* item = spawnItem(spawnPos, nameIt->second);
            if (item) beginDragging(item, worldPos);
            break;
        }

        // --- Tab: switch category ---
        case BodyKind::Tab: {
            auto nameIt = bodyNames.find(hit);
            if (nameIt != bodyNames.end())
                showCategory(nameIt->second);
            break;
        }

        // --- Cinch & Go button ---
        case BodyKind::CinchAndGoIdle: {
            hit->SetType(b2_dynamicBody);
            hit->SetAwake(true);
            // Random launch: speed 12-18, angle biased upward (30°-150°)
            float speed = 12.0f + QRandomGenerator::global()->bounded(6.0f);
            float angle = (30.0f + QRandomGenerator::global()->bounded(120.0f)) * b2_pi / 180.0f;
            hit->SetLinearVelocity(b2Vec2(speed * std::cos(angle), speed * std::sin(angle)));
            hit->SetAngularVelocity(QRandomGenerator::global()->bounded(20.0f) - 10.0f);
            hit->SetUserData(tagFor(BodyKind::CinchAndGoActivated));
            break;
        }

        // --- Existing items: rearrange ---
        case BodyKind::Item:
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

    // --- Hover tooltip tracking ---
    b2Body* hit = findBodyAt(screenToWorld(event->pos()));
    b2Body* newHover = nullptr;
    if (hit && kindOf(hit) == BodyKind::Spawner)
        newHover = hit;

    if (newHover != hoveredBody) {
        hoveredBody = newHover;
        update();
    }
}

void BackpackScene::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        endDragging();
    }
}

// =========================================================================
//  Physics step + destroy-on-exit sweep
// =========================================================================

void BackpackScene::updatePhysics()
{
    world.Step(1.0f / static_cast<float>(REFRESH_RATE), 8, 3);

    constexpr float EXIT_MARGIN = 2.0f;

    std::vector<b2Body*> toDestroy;
    for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
        if (body->GetType() != b2_dynamicBody) continue;

        const BodyKind kind = kindOf(body);
        const bool destroyable =
               kind == BodyKind::Item
            || kind == BodyKind::CinchAndGoActivated;
        if (!destroyable) continue;

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

        bodyNames.erase(body);
        world.DestroyBody(body);
    }

    update();
}

// =========================================================================
//  Scene construction
// =========================================================================

void BackpackScene::createMouseAnchor()
{
    b2BodyDef def;
    def.type = b2_staticBody;
    def.position.Set(0.0f, 0.0f);
    mouseJointAnchor = world.CreateBody(&def);
    mouseJointAnchor->SetUserData(tagFor(BodyKind::MouseAnchor));
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

    // U-shaped pack — centered at X=17.5 (right side) so the spawner
    // shelf can occupy the middle of the scene without overlap.
    makePackWall(16.5f, 3.25f, 0.1f, 2.25f);  // left wall
    makePackWall(18.5f, 3.25f, 0.1f, 2.25f);  // right wall
    makePackWall(17.5f, 1.0f,  1.1f, 0.1f);   // bottom

    // Interior sensor
    b2BodyDef sensorDef;
    sensorDef.type = b2_staticBody;
    sensorDef.position.Set(17.5f, 3.3f);
    packSensor = world.CreateBody(&sensorDef);

    b2PolygonShape sensorBox;
    sensorBox.SetAsBox(0.9f, 2.2f);

    b2FixtureDef sensorFix;
    sensorFix.shape = &sensorBox;
    sensorFix.isSensor = true;
    packSensor->CreateFixture(&sensorFix);

    packSensor->SetUserData(tagFor(BodyKind::PackSensor));
}

void BackpackScene::createTabs()
{
    struct TabDef { QString cat; QString name; };
    std::vector<TabDef> allTabs;
    if (!foodItems.isEmpty())  allTabs.push_back({"food", "food"});
    if (!drinkItems.isEmpty()) allTabs.push_back({"drink", "drink"});
    if (!gearItems.isEmpty())  allTabs.push_back({"gear", "gear"});

    // Center the tabs horizontally over the shelf (shelf occupies
    // roughly X=[7.0, 14.5]). Kept below world Y=8.22 so they clear
    // the Survival Guide reservation.
    constexpr float tabCenterX = 10.75f;
    constexpr float tabSpacing = 2.6f;
    constexpr float tabY       = 7.95f;
    float startX = tabCenterX - (allTabs.size() - 1) * (tabSpacing / 2.0f);
    for (size_t i = 0; i < allTabs.size(); ++i) {
        b2BodyDef def;
        def.type = b2_staticBody;
        def.position.Set(startX + i * tabSpacing, tabY);
        b2Body* body = world.CreateBody(&def);

        b2PolygonShape box;
        box.SetAsBox(1.10f, 0.32f);

        b2FixtureDef fix;
        fix.shape = &box;
        fix.filter.maskBits = 0;
        body->CreateFixture(&fix);

        body->SetUserData(tagFor(BodyKind::Tab));
        bodyNames[body] = allTabs[i].name;
    }
}

void BackpackScene::createCinchAndGoButton()
{
    // Static button — turns dynamic when clicked. Sits in the
    // bottom-center of the scene, clear of the pack (X≈17.5).
    b2BodyDef def;
    def.type = b2_staticBody;
    def.position.Set(14.5f, 1.45f);
    def.linearDamping = 0.0f;
    def.angularDamping = 0.5f;
    cinchAndGoButton = world.CreateBody(&def);

    b2PolygonShape box;
    box.SetAsBox(0.85f, 0.40f);

    b2FixtureDef fix;
    fix.shape = &box;
    fix.density = 1.0f;
    fix.friction = 0.5f;
    cinchAndGoButton->CreateFixture(&fix);

    cinchAndGoButton->SetUserData(tagFor(BodyKind::CinchAndGoIdle));
}

// =========================================================================
//  Dynamic spawner management
// =========================================================================

void BackpackScene::showCategory(const QString& category)
{
    activeTab = category;

    // Destroy existing spawner bodies
    for (b2Body* body : spawnerBodies) {
        bodyNames.erase(body);
        world.DestroyBody(body);
    }
    spawnerBodies.clear();

    // Select items for this category
    const QStringList* items = nullptr;
    if (category == "food")       items = &foodItems;
    else if (category == "drink") items = &drinkItems;
    else if (category == "gear")  items = &gearItems;

    if (items && !items->isEmpty())
        layoutSpawners(*items);

    // Clear hover since spawner bodies changed
    hoveredBody = nullptr;
    update();
}

void BackpackScene::layoutSpawners(const QStringList& items)
{
    // Grid layout parameters — shelf starts at X=7.0 so the top-left
    // pixel region (kGuideReservedRect, 550×340) stays visually empty
    // for the future Survival Guide overlay.
    constexpr float LEFT_MARGIN = 7.0f;
    constexpr float SPACING_X = 1.5f;
    constexpr float SPACING_Y = 1.3f;
    constexpr float START_Y = 6.8f;
    constexpr int ITEMS_PER_ROW = 6;
    constexpr float SPAWNER_HALF_W = 0.40f;
    constexpr float SPAWNER_HALF_H = 0.20f;

    for (int i = 0; i < items.size(); ++i) {
        int col = i % ITEMS_PER_ROW;
        int row = i / ITEMS_PER_ROW;

        float cx = LEFT_MARGIN + col * SPACING_X;
        float cy = START_Y - row * SPACING_Y;

        b2BodyDef def;
        def.type = b2_staticBody;
        def.position.Set(cx, cy);
        b2Body* body = world.CreateBody(&def);

        // Create spawner shape using the item's actual shape data
        const QString& itemName = items[i];
        QString cat = ItemRegistry::categoryOf(itemName);
        ItemStats stats;
        if (cat == "food")       stats = ItemRegistry::getFoodStats(itemName);
        else if (cat == "drink") stats = ItemRegistry::getDrinkStats(itemName);
        else if (cat == "gear")  stats = ItemRegistry::getGearStats(itemName);

        bool hasShape = !stats.shape.fixtures.empty();

        if (hasShape) {
            // Use the item's actual shape for the spawner (scaled to fit)
            for (const auto& fix : stats.shape.fixtures) {
                b2FixtureDef fixDef;
                fixDef.filter.maskBits = 0;

                if (fix.isBox) {
                    b2PolygonShape box;
                    if (fix.centerX != 0.0f || fix.centerY != 0.0f)
                        box.SetAsBox(fix.halfW, fix.halfH, b2Vec2(fix.centerX, fix.centerY), 0.0f);
                    else
                        box.SetAsBox(fix.halfW, fix.halfH);
                    fixDef.shape = &box;
                    body->CreateFixture(&fixDef);
                } else if (!fix.vertices.empty()) {
                    b2PolygonShape poly;
                    std::vector<b2Vec2> verts;
                    for (const auto& [x, y] : fix.vertices)
                        verts.emplace_back(x, y);
                    poly.Set(verts.data(), static_cast<int32>(verts.size()));
                    fixDef.shape = &poly;
                    body->CreateFixture(&fixDef);
                }
            }
        } else {
            // Fallback: simple rectangle
            b2PolygonShape box;
            box.SetAsBox(SPAWNER_HALF_W, SPAWNER_HALF_H);
            b2FixtureDef fixDef;
            fixDef.shape = &box;
            fixDef.filter.maskBits = 0;
            body->CreateFixture(&fixDef);
        }

        body->SetUserData(tagFor(BodyKind::Spawner));
        bodyNames[body] = itemName;
        spawnerBodies.push_back(body);
    }
}

// =========================================================================
//  Data-driven item spawning
// =========================================================================

b2Body* BackpackScene::spawnItem(const b2Vec2& position, const QString& itemName)
{
    QString cat = ItemRegistry::categoryOf(itemName);
    ItemStats stats;
    if (cat == "food")       stats = ItemRegistry::getFoodStats(itemName);
    else if (cat == "drink") stats = ItemRegistry::getDrinkStats(itemName);
    else if (cat == "gear")  stats = ItemRegistry::getGearStats(itemName);

    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = position;
    def.linearDamping = 0.5f;
    def.angularDamping = 1.0f;
    b2Body* body = world.CreateBody(&def);

    if (!stats.shape.fixtures.empty()) {
        // Create fixtures from shape data
        for (const auto& fix : stats.shape.fixtures) {
            b2FixtureDef fixDef;
            fixDef.density = stats.shape.density;
            fixDef.friction = 0.5f;

            if (fix.isBox) {
                b2PolygonShape box;
                if (fix.centerX != 0.0f || fix.centerY != 0.0f)
                    box.SetAsBox(fix.halfW, fix.halfH, b2Vec2(fix.centerX, fix.centerY), 0.0f);
                else
                    box.SetAsBox(fix.halfW, fix.halfH);
                fixDef.shape = &box;
                body->CreateFixture(&fixDef);
            } else if (!fix.vertices.empty()) {
                b2PolygonShape poly;
                std::vector<b2Vec2> verts;
                for (const auto& [x, y] : fix.vertices)
                    verts.emplace_back(x, y);
                poly.Set(verts.data(), static_cast<int32>(verts.size()));
                fixDef.shape = &poly;
                body->CreateFixture(&fixDef);
            }
        }
    } else {
        // Fallback generic shape
        float scale = std::clamp(static_cast<float>(std::sqrt(stats.volume)) * 0.35f, 0.15f, 0.60f);
        b2PolygonShape box;
        box.SetAsBox(scale, scale);
        b2FixtureDef fixDef;
        fixDef.shape = &box;
        fixDef.density = 1.0f;
        fixDef.friction = 0.5f;
        body->CreateFixture(&fixDef);
    }

    body->SetUserData(tagFor(BodyKind::Item));
    bodyNames[body] = itemName;
    return body;
}

// =========================================================================
//  Picking and dragging
// =========================================================================

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
    if (!body || !mouseJointAnchor) return;
    if (mouseJoint) endDragging();
    if (body->GetType() != b2_dynamicBody) return;

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

// =========================================================================
//  Coordinate conversion
// =========================================================================

b2Vec2 BackpackScene::screenToWorld(const QPoint& point)
{
    return b2Vec2(
        point.x() / PIXELS_PER_METER,
        (height() - point.y()) / PIXELS_PER_METER
    );
}

QPointF BackpackScene::worldToScreen(const b2Vec2& point)
{
    return QPointF(
        point.x * PIXELS_PER_METER,
        height() - point.y * PIXELS_PER_METER
    );
}

// =========================================================================
//  Theme application
// =========================================================================

void BackpackScene::applyTheme(const LevelTheme& theme)
{
    themePrimary   = theme.accent;
    themeSecondary = theme.accent.lighter(160);
    themeOnPrim    = QColor("#FFFFFF");
    themeOnSurface = QColor("#2A2A2A");
    update();
}
