#include "trekbox2dphysics.h"
#include "trekview.h"
#include <iostream>

TrekBox2DPhysics::TrekBox2DPhysics(QObject *parent)
    : QObject{parent}
    , world(b2Vec2(0.0f,0.0f))
    , timer(new QTimer(this))
{
    connect(timer, &QTimer::timeout,this, &TrekBox2DPhysics::updateWorldSlot);

    timer->start(10);

    // Blueprint for a body. No actual body yet
    b2BodyDef groundBodyDef;
    //Sets it as static, infinite mass. Doesn't move
    groundBodyDef.type = b2_staticBody;

    // Creates a shape with the wild name of "shape"
    //Can be modified and "stamped" into the world
    b2PolygonShape shape;

    //Widget pixel width/height / pixels per in game meter
    float width = widgetWidth / (2.0f * pixelsPerMeter);
    float height = widgetHeight / (2.0f * pixelsPerMeter);
    float wallThickness = 0.5f;

    //Bottom

    //Placing the center of the wall body, Slightly below bottom of screen
    groundBodyDef.position.Set(0.0f, -height - wallThickness);
    //Creates a static body using the position and blueprint of groundBodyDef
    b2Body* bottom = world.CreateBody(&groundBodyDef);
    //Now that a static body has been put at a point, set the shape with a size
    shape.SetAsBox(width, wallThickness);
    //Add that shape to the world based on shape, idk i am jus ballin atp
    bottom->CreateFixture(&shape, 0.0f);

    //Top
    groundBodyDef.position.Set(0.0f, height + wallThickness);
    b2Body* top = world.CreateBody(&groundBodyDef);
    top->CreateFixture(&shape, 0.0f);

    //Left
    groundBodyDef.position.Set(-width - wallThickness, 0.0f);
    b2Body* left = world.CreateBody(&groundBodyDef);
    shape.SetAsBox(wallThickness, height);
    left->CreateFixture(&shape, 0.0f);

    //Right
    groundBodyDef.position.Set(width + wallThickness, 0.0f);
    b2Body* right = world.CreateBody(&groundBodyDef);
    right->CreateFixture(&shape, 0.0f);





    // Define the dynamic body. We set its position and call the body factory.
    //But it has no size, no collision, no physics properties
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(height / 2, width / 2);
    body = world.CreateBody(&bodyDef);

    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(1.0f, 1.0f);

    // Define the dynamic body geometry, size.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;

    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 1.0f;

    // Override the default friction.
    fixtureDef.friction = 0.3f;

    //Bounce
    fixtureDef.restitution = 0.9f;

    // Add the shape to the body.
    body->CreateFixture(&fixtureDef);
}

TrekBox2DPhysics::~TrekBox2DPhysics(){
}

b2Vec2 TrekBox2DPhysics::screenToWorld(const QPointF& p) const {

    float worldX = (p.x() - (widgetWidth / 2)) / pixelsPerMeter;
    float worldY = ((widgetHeight / 2) - p.y()) / pixelsPerMeter;   // flip Y
    return b2Vec2(worldX, worldY);

}

QPointF TrekBox2DPhysics::worldToScreen(const b2Vec2& p) const{
    float screenX = (widgetWidth / 2) + p.x * pixelsPerMeter;
    float screenY = (widgetHeight / 2) - p.y * pixelsPerMeter;      // flip Y
    return QPointF(screenX, screenY);

}

void TrekBox2DPhysics::setTargetFromMouse(const QPointF& mousePos){

    std::cout << "Got mousePos X:  " <<mousePos.x() << " Y: " << mousePos.y() << std::endl;
    targetPoint = screenToWorld(mousePos);

    std::cout << "World Transformation X:  "<< targetPoint.x << " Y: " << targetPoint.y << std::endl;

    hasTarget = true;
}

void TrekBox2DPhysics::leftMouseButtonClickedSlot(bool clicked){
    hasTarget = false;
}

void TrekBox2DPhysics::createPhysicsObjects(){

    for(PhysicsObjectDef phys : objectsVector){

        b2BodyDef bodyDef;

        if(phys.isStatic){
            bodyDef.type = b2_staticBody;
        }else{
            bodyDef.type = b2_dynamicBody;
        }

        bodyDef.position.Set(phys.x / pixelsPerMeter, phys.y / pixelsPerMeter);

        bodyDef.angle = phys.angle * (b2_pi / 180.0f);

    }


}

void TrekBox2DPhysics::updateWorldSlot(){

    //std::cout << "UpdateWorldSlot" << std::endl;
    float timeStep = 1.0f / 60.0f;
    int velocityIterations = 6;
    int positionIterations = 2;

    if(body)
    {
        b2Vec2 currentPos = body->GetWorldCenter();
        b2Vec2 toTarget = targetPoint - currentPos;

        float distance = toTarget.Length();

        if(distance > 0.01f && hasTarget){

            toTarget.Normalize();

            //Scale force by distance so farther clicks pull harder
            toTarget.Normalize();
            float forceMagnitude = qMin(distance * forceGain, maxForce);
            body->ApplyForceToCenter(forceMagnitude * toTarget, true);
        }else {
            body->ApplyForceToCenter(b2Vec2(0.0f,0.0f), true);
        }

        world.Step(timeStep, velocityIterations, positionIterations);

        b2Vec2 position = body->GetPosition();

        if (body)
        {
            b2Vec2 position = body->GetPosition();
            QPointF screen = worldToScreen(position);
            emit sendBodyLocation(screen.x(), screen.y());
        }
    }
}



void TrekBox2DPhysics::createPhysicsStructure(){

}
