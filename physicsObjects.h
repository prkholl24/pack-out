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
    QString type = " ";
    QString shapeType = " ";
    float x = 0.0;
    float y = 0.0;
    float width = 0.0;
    float height = 0.0;
    float angle = 0;
    bool isStatic = true;
    float friction = 0.3f;
    float restitution = 0.9f;
    QVector<int> color = {255,255,255};
};




#endif // PHYSICSOBJECTS_H
