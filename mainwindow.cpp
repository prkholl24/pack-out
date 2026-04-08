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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    // TO-DO: Implement this
}

b2Vec2 MainWindow::screenToWorld(const QPoint& point)
{
    // Inverts the Y value such that the
    // bottom of the screen is 0.
    float worldY = this->height() - point.y();

    return b2Vec2(point.x(), worldY);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        b2Vec2 worldPos = screenToWorld(event->pos());

        // TO-DO: Remove this
        std::cout << "Qt coordinate [X:" << event->pos().x() << ", " << event->pos().y() << ")" << std::endl
                  << "Box2D coordinate (" << worldPos.x << ", " << worldPos.y << ")" << std::endl;
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
    // TO-DO: Implement this
}
