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
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , world(b2Vec2(0.0f, -10.0f))
    , timer(new QTimer(this))
{
    ui->setupUi(this);

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
    painter.setBrush(Qt::blue);
    painter.setPen(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);

    for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
        painter.save();

        QPointF screenPos = worldToScreen(body->GetPosition());
        painter.translate(screenPos);
        painter.rotate(-body->GetAngle() * 180.0f / b2_pi);

        for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {

            // Translates the Box2D shape into a shape Qt understands
            b2PolygonShape* poly = static_cast<b2PolygonShape*>(fixture->GetShape());
            QPolygonF qPoly;

            for (int32 i = 0; i < poly->GetVertexCount(); ++i) {

                b2Vec2 v = poly->GetVertex(i);

                QPointF vertexPos(v.x, -v.y);

                qPoly << vertexPos;
            }
            painter.drawPolygon(qPoly);
        }

        painter.restore();
    }
}

b2Vec2 MainWindow::screenToWorld(const QPoint& point)
{
    // Inverts the Y value such that the
    // BOTTOM of the screen is 0.
    float worldY = this->height() - point.y();

    return b2Vec2(point.x(), worldY);
}

QPointF MainWindow::worldToScreen(const b2Vec2& point)
{
    // Inverts the Y value such that the
    // TOP of the screen is 0
    float worldY = this->height() - point.y;

    return QPointF(point.x, worldY);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {

        b2Vec2 worldPos = screenToWorld(event->pos());

        // TO-DO: Remove this
        std::cout << "Qt coordinate (" << event->pos().x() << ", " << event->pos().y() << ")" << std::endl
                  << "Box2D coordinate (" << worldPos.x << ", " << worldPos.y << ")" << std::endl;

        // Define the dynamic body. We set its position and call the body factory.
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(worldPos.x, worldPos.y);
        b2Body* body = world.CreateBody(&bodyDef);

        // Define another box shape for our dynamic body.
        b2PolygonShape dynamicBox;
        dynamicBox.SetAsBox(10.0f, 10.0f);

        // Define the dynamic body fixture.
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;

        // Set the box density to be non-zero, so it will be dynamic.
        fixtureDef.density = 1.0f;

        // Override the default friction
        fixtureDef.friction = 0.3f;

        // Add the shape to the body.
        body->CreateFixture(&fixtureDef);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // TO-DO: Implement this
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    // TO-DO: Implement this
}

void MainWindow::updatePhysics()
{
    // Step the simulation and request a repaint
    world.Step(1.0f / static_cast<float>(REFRESH_RATE) , 8, 3);
    update();
}
