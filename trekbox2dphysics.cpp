#include "trekbox2dphysics.h"
#include "trekview.h"
#include <iostream>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileDialog>

TrekBox2DPhysics::TrekBox2DPhysics(QObject *parent)
    : QObject{parent}
    , world(b2Vec2(0.0f,0.0f))
    , timer(new QTimer(this))
{
    //

    if(loadFromFile){
    fromJsonLevel(FilePath);
    }
    connect(timer, &QTimer::timeout,this, &TrekBox2DPhysics::updateWorldSlot);

    timer->start(10);





    if(!loadFromFile){
    createPhysicsStructure();

        if(saveToFile){
        toJsonLevel(FilePath);

        }

    }

    createPhysicsObjects();


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
    bodyDef.position.Set(startLocationX, startLocationY);
    body = world.CreateBody(&bodyDef);
    PhysicsObjectDef temp;
    temp.x = bodyDef.position.x;
    temp.y = bodyDef.position.y;
    temp.width = 50;
    temp.height = 50;
    bodyVector.append(qMakePair(temp,body));

    // Define another box shape for our dynamic body.
    b2CircleShape circle;
    circle.m_radius = (temp.width * 0.5) / 20;
    // dynamicCircle.SetAsBox(1.0f, 1.0f);


    // Define the dynamic body geometry, size.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;

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

    targetPoint = screenToWorld(mousePos);

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

        b2Body* body = world.CreateBody(&bodyDef);

        bodyVector.append(qMakePair(phys,body));

        if(phys.shapeType == "box")
        {

            b2PolygonShape  shape;
            shape.SetAsBox((phys.width / pixelsPerMeter) * 0.5f, (phys.height / pixelsPerMeter) * 0.5f );

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &shape;
            fixtureDef.density = phys.isStatic ? 0.0f : 1.0f;
            fixtureDef.friction = phys.friction;
            fixtureDef.restitution = phys.restitution;

            body->CreateFixture(&fixtureDef);
        }else if (phys.shapeType == "circle") {


            b2CircleShape shape;
            shape.m_radius = (phys.width * 0.5f) / pixelsPerMeter;
            b2FixtureDef fixtureDef;
            fixtureDef.shape = &shape;
            fixtureDef.density = phys.isStatic ? 0.0f : 1.0f;
            fixtureDef.friction = phys.friction;
            fixtureDef.restitution = phys.restitution;

            body->CreateFixture(&fixtureDef);
        }
    }


}

void TrekBox2DPhysics::updateWorldSlot(){

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


        if (body)
        {


            // (phys.width * 0.5f) / pixelsPerMeter

            // emit loadVisualObjectsSignal(objectsVector);
            // emit sendBodyLocation(screen.x(), screen.y());
            // emit loadVisualObjectsSignal(objectsVector);
            QVector<PhysicsObjectDef> liveBody;
            for (auto& [def,vis] : bodyVector){
                b2Vec2 position = vis->GetPosition();
                QPointF screen = worldToScreen(position);
                // emit sendBodyLocation(screen.x(), screen.y());
                // std::cout << screen.x() << std::endl;
                PhysicsObjectDef tempBody = def;
                tempBody.x = screen.x();
                tempBody.y = screen.y();
                tempBody.angle = vis->GetAngle() * 180.0f / 3.14159265f;
                liveBody.append(tempBody);
            }
            emit loadVisualObjectsSignal(liveBody);

        }
    }
}



void TrekBox2DPhysics::createPhysicsStructure(){

    if(!worldEdit){

    //Width [-960,960]
    //Height [540,-540] (bottom,top)

    // PhysicsObjectDef p1;
    // p1.type = "ABoxCuh";
    // p1.shapeType = "box";
    // p1.x = -275;
    // p1.y = 220;
    // p1.width = 1370;
    // p1.height = 50;
    // p1.angle = 0;
    // p1.isStatic = true;
    // p1.friction = 0.3f;
    // p1.restitution = 0.9f;

    // p1.color = {99,57,15,0};

    // objectsVector.append(p1);




    // int y = 0;
    // int x = -940;
    // for(int i = 0; i < 5000; i++){

    //     if(i%165 == 0){

    //         y += 11;
    //         x = -900;
    //     }
    //     PhysicsObjectDef p1;
    //     p1.type = "ABoxCuh";
    //     p1.shapeType = "circle";
    //     p1.x = x;
    //     p1.y = y;
    //     p1.width = 10;
    //     p1.height = 10;
    //     p1.angle = 0;
    //     p1.isStatic = false;
    //     p1.friction = 0.3f;
    //     p1.restitution = 0.9f;
    //     p1.color = {99,57,15,0};
    //     objectsVector.append(p1);

    //     x+=11;
    // }

    }
}



void TrekBox2DPhysics::worldEditorSlot(int key){



    if(!worldEdit){
        return;
    }

    switch(key)
    {
            //Insert new rectangle
        case Qt::Key_R:
        {
            std::cout << "New Rectangle" << std::endl;

            //Removes last object
            if(objectAdded){
                objectsVector.removeLast();


            }
            removeAllPhysicsObjects();

            PhysicsObjectDef p1;

            p1.type = "ABoxCuh";
            p1.shapeType = "box";
            p1.x = 0;
            p1.y = 0;
            p1.width = 50;
            p1.height = 50;
            p1.angle = 0;
            p1.isStatic = true;
            p1.friction = 0.3f;
            p1.restitution = 0.9f;

            p1.color = currentColor;

            objectsVector.append(p1);

            objectAdded = true;
            createPhysicsObjects();

            break;
        }
        //Insert new circle
        case Qt::Key_C:
        {
            std::cout << "New Circle" << std::endl;

            //Removes last object
            if(objectAdded){
                objectsVector.removeLast();
            }
            removeAllPhysicsObjects();

            PhysicsObjectDef p1;

            p1.type = "ABoxCuh";
            p1.shapeType = "circle";
            p1.x = 0;
            p1.y = 0;
            p1.width = 50;
            p1.height = 50;
            p1.angle = 0;
            p1.isStatic = true;
            p1.friction = 0.3f;
            p1.restitution = 0.9f;

            p1.color = currentColor;

            objectsVector.append(p1);

            objectAdded = true;
            createPhysicsObjects();

            break;
        }
        //Object Movement
        case Qt::Key_W:
        {
            if(objectAdded){
                removeAllPhysicsObjects();
                if(objectsVector.last().y <540)
                {
                    objectsVector.last().y += movementModifier;
                }

                createPhysicsObjects();
            }
            break;

        }
        case Qt::Key_S:
        {
            if(objectAdded){
                removeAllPhysicsObjects();


                if(objectsVector.last().y > -540)
                {
                    objectsVector.last().y -= movementModifier;
                }

                createPhysicsObjects();
            }
            break;

        }
        case Qt::Key_A:
        {
            if(objectAdded){
                removeAllPhysicsObjects();


                if(objectsVector.last().x > -960)
                {
                    objectsVector.last().x -= movementModifier;
                }

                createPhysicsObjects();
            }
            break;

        }
        case Qt::Key_D:
        {
            if(objectAdded){
                removeAllPhysicsObjects();


                if(objectsVector.last().x < 960)
                {
                    objectsVector.last().x += movementModifier;
                }

                createPhysicsObjects();
            }
            break;

        }
        //Object Resizing
        case Qt::Key_Up:
        {
            if(objectAdded){



                removeAllPhysicsObjects();



                if(objectsVector.last().shapeType == "circle"){
                    objectsVector.last().height += movementModifier;
                    objectsVector.last().width += movementModifier;

                }else{
                    objectsVector.last().height += movementModifier;

                }



                createPhysicsObjects();
            }
            break;

        }
        case Qt::Key_Down:
        {
            if(objectAdded && (objectsVector.last().height > (movementModifier+1))){
                removeAllPhysicsObjects();


                if(objectsVector.last().shapeType == "circle"){
                    objectsVector.last().height -= movementModifier;
                    objectsVector.last().width -= movementModifier;

                }else{
                    objectsVector.last().height -= movementModifier;
                }

                createPhysicsObjects();
            }
            break;

        }
        case Qt::Key_Right:
        {
            if(objectAdded){
                removeAllPhysicsObjects();


                if(objectsVector.last().shapeType == "circle"){
                    objectsVector.last().height += movementModifier;
                    objectsVector.last().width += movementModifier;

                }else{
                    objectsVector.last().width += movementModifier;
                }

                createPhysicsObjects();
            }
            break;

        }
        case Qt::Key_Left:
        {
            if(objectAdded && (objectsVector.last().width > (movementModifier+1))){
                removeAllPhysicsObjects();


                if(objectsVector.last().shapeType == "circle"){
                    objectsVector.last().height -= movementModifier;
                    objectsVector.last().width -= movementModifier;

                }else{
                    objectsVector.last().width -= movementModifier;
                }

                createPhysicsObjects();
            }
            break;

        }
        //Saves current object
        case Qt::Key_N:
        {
            objectAdded = false;

            break;
        }
        //Adjusts angle
        case Qt::Key_Q:
        {

            if(objectAdded && (objectsVector.last().shapeType == "box")){
                removeAllPhysicsObjects();


                objectsVector.last().angle += movementModifier/2;

                createPhysicsObjects();
            }

            break;
        }
        case Qt::Key_E:
        {

            if(objectAdded && (objectsVector.last().shapeType == "box")){
                removeAllPhysicsObjects();


                objectsVector.last().angle -= movementModifier/2;

                createPhysicsObjects();
            }

            break;
        }
        case Qt::Key_Z:
        {


            if(objectAdded){
                removeAllPhysicsObjects();

                if(objectsVector.last().isStatic){
                    std::cout << "Object is dynamic" << std::endl;
                    objectsVector.last().isStatic = false;
                }else{
                    std::cout << "Object is static" << std::endl;
                    objectsVector.last().isStatic = true;
                }
                createPhysicsObjects();
            }



            break;
        }

        case Qt::Key_K:
        {
            if(objectAdded){


                emit getColorSignal();
            }

            break;
        }
        case Qt::Key_Return:
        {

            if(objectsVector.size() > 0){

                std::cout << "File Saved" << std::endl;
                toJsonLevel(FilePath);

            }

        }

    }

}

void TrekBox2DPhysics::setColorSlot(QVector<int> color){
    std::cout << "got color" << std::endl;

    currentColor = color;

    objectsVector.last().color = currentColor;

    removeLastPhysicsObject();

    createPhysicsObjects();


}



void TrekBox2DPhysics::removeLastPhysicsObject(){
    if(bodyVector.isEmpty()){
        return;
    }

    b2Body* obj = bodyVector.last().second;

    if(obj){
        world.DestroyBody(obj);
    }

    bodyVector.removeLast();


}

void TrekBox2DPhysics::removeAllPhysicsObjects(){
    while(bodyVector.size() > 1){
        std::cout << "removed another"<<std::endl;
        removeLastPhysicsObject();
    }


}

void TrekBox2DPhysics::toJsonLevel(QString& filepath){
    QJsonObject level;
    level["level"] = currentLevel;
    level["background"] = levelBackground;

    QJsonArray objectArray;
    for (PhysicsObjectDef objectDef : objectsVector){
        QJsonObject object;
        object["type"] = objectDef.type;
        object["shapetype"] = objectDef.shapeType;
        object["x"] = objectDef.x;
        object["y"] = objectDef.y;
        object["width"] = objectDef.width;
        object["height"] = objectDef.height;
        object["angle"] = objectDef.angle;
        object["static"] = objectDef.isStatic;
        object["friction"] = objectDef.friction;
        object["restitution"] = objectDef.restitution;
        QJsonArray color;
        for (int colorId : objectDef.color){
            color.append(colorId);
        }
        object["color"] = color;
        objectArray.append(object);
    }
    level["objects"] = objectArray;


    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly)){
        file.write(QJsonDocument(level).toJson(QJsonDocument::Indented));
        std::cout << "Saved to: " + FilePath.toStdString() << std::endl;
    }


}

void TrekBox2DPhysics::fromJsonLevel(QString& filepath){
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject level = doc.object();
    currentLevel = level["level"].toInt();
    levelBackground = level["background"].toString();
    QJsonArray objectArray = level["objects"].toArray();
    for (auto objectdef: std::as_const(objectArray)){
        PhysicsObjectDef temp;
        QJsonObject object = objectdef.toObject();
        temp.type = object["type"].toString();
        temp.shapeType = object["shapetype"].toString();
        temp.x = object["x"].toDouble();
        temp.y = object["y"].toDouble();
        temp.width = object["width"].toDouble();
        temp.height = object["height"].toDouble();
        temp.angle = object["angle"].toDouble();
        temp.isStatic = object["static"].toBool();
        temp.friction = object["friction"].toDouble();
        temp.restitution = object["restitution"].toDouble();
        QJsonArray color = object["color"].toArray();
        temp.color.clear();
        for (auto colorID : std::as_const(color)){
            temp.color.push_back(colorID.toInt());
        }
        objectsVector.push_back(temp);
    }
}




