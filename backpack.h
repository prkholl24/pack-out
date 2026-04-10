/**
 * @file backpack.h
 * @brief Contains the BackpackScene widget — the packing-phase view.
 *
 * Defines the BackpackScene class, which is responsible for:
 * - managing the Box2D physics world for the packing scene
 * - handling mouse input (spawn, drag, release, Cinch & Go)
 * - rendering all bodies to the screen
 * - emitting signals when items enter/leave the pack or the level starts
 *
 * This widget is embedded inside MainWindow's QStackedWidget and can
 * be swapped out for other scene widgets (trip, grading, etc.).
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
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
 * backpack, a Cinch & Go button, and dynamically-spawned item bodies.
 * The player drags items from the shelf into the pack, then activates
 * the button to start the level.
 */
class BackpackScene : public QWidget
{
    Q_OBJECT

public:
    BackpackScene(QWidget *parent = nullptr);
    ~BackpackScene();

signals:
    /**
     * @brief Emitted once the player activates the "Cinch & Go"
     * button and the button body has fully exited the screen.
     */
    void levelHasStarted();

    /**
     * @brief Emitted when an item body enters the pack's interior.
     *
     * Fires exactly once per item per entry, even for multi-fixture
     * items — a contact counter inside the listener de-duplicates.
     *
     * @param itemName - Short human-readable name, e.g. "BANANA".
     */
    void itemAddedToBag(const QString& itemName);

    /**
     * @brief Emitted when an item body leaves the pack's interior,
     * or when an item inside the pack is destroyed.
     *
     * @param itemName - Short human-readable name, e.g. "BANANA".
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
    const int REFRESH_RATE = 60;

    static constexpr float PIXELS_PER_METER = 50.0f;
    static constexpr float WORLD_WIDTH_M = 16.0f;
    static constexpr float WORLD_HEIGHT_M = 12.0f;
    static constexpr int WINDOW_WIDTH_PX = 800;
    static constexpr int WINDOW_HEIGHT_PX = 600;

    Ui::BackpackScene* ui;

    b2World world;
    QTimer* timer;

    b2MouseJoint* mouseJoint = nullptr;
    b2Body* mouseJointAnchor = nullptr;
    b2Body* cinchAndGoButton = nullptr;
    bool levelStarted = false;

    class DestructionListener;
    std::unique_ptr<DestructionListener> destructionListener;

    class ContactListener;
    std::unique_ptr<ContactListener> contactListener;

    b2Body* packSensor = nullptr;

    void createShelf();
    void createBackpack();
    void createSpawners();
    void createCinchAndGoButton();
    void createMouseAnchor();

    b2Body* spawnBanana(const b2Vec2& position);
    b2Body* spawnPotato(const b2Vec2& position);
    b2Body* spawnBottle(const b2Vec2& position);

    b2Body* findBodyAt(const b2Vec2& worldPos);
    void beginDragging(b2Body* body, const b2Vec2& worldPos);
    void endDragging();

    b2Vec2 screenToWorld(const QPoint& point);
    QPointF worldToScreen(const b2Vec2& point);
};

#endif // BACKPACKSCENE_H
