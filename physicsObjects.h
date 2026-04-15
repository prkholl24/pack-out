#ifndef PHYSICSOBJECTS_H
#define PHYSICSOBJECTS_H
#include <QString>
#include <Box2D/Box2D.h>
#include <qvector.h>
/**
 * Defines the struct which contains all relevant information
 * to the location, color, size, and type of objects being made
 * in the physics engine, and drawn by view.
 */


//Struct for all the objects on each level
struct PhysicsObjectDef{
    QString type;
    QString shapeType;
    float x;
    float y;
    float width;
    float height;
    float angle;
    bool isStatic;
    float friction;
    float restitution;
    QVector<int> color;
};




#endif // PHYSICSOBJECTS_H
