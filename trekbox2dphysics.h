#ifndef TREKBOX2DPHYSICS_H
#define TREKBOX2DPHYSICS_H

#include <QObject>
#include <Box2D/Box2D.h>
#include <QTimer>

class TrekBox2DPhysics : public QObject
{
    Q_OBJECT
public:
    explicit TrekBox2DPhysics(QObject *parent = nullptr);
    ~TrekBox2DPhysics();
    /**
     * @brief Box2D World
     */
    b2World world;

    b2Vec2 targetPoint;

    bool hasTarget = false;

    //How many pixels mark one in game meter
    float pixelsPerMeter = 20.0f;

    //Gain on the force applied based on the distance
    //From player to mouse
    float forceGain = 5.0f;

    //Maximum force that can be applied to hiker
    float maxForce = 10.0f;
    /**
     * @brief body
     */
    b2Body* body = nullptr;

    /**
     * @brief timer
     */
    QTimer* timer;

private:

    int widgetWidth = 800;
    int widgetHeight = 600;

    /**
     * @brief Converts a point on the screen to a point in box2D based on the
     *        screen size.
     *        Method was acquired from ChatGPT GPT5.3
     */
    b2Vec2 screenToWorld(const QPointF& p) const;

    QPointF worldToScreen(const b2Vec2& p) const;



signals:
    void sendBodyLocation(float x, float y);

public slots:

    void setTargetFromMouse(const QPointF& mousePos);

    void updateWorldSlot();

    void leftMouseButtonClickedSlot(bool clicked);
};

#endif // TREKBOX2DPHYSICS_H
