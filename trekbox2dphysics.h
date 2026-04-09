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

    /**
     * @brief body
     */
    b2Body* body;

    /**
     * @brief timer
     */
    QTimer* timer;

signals:
    void sendBodyLocation(float x, float y);

public slots:
    void updateWorldSlot();
};

#endif // TREKBOX2DPHYSICS_H
