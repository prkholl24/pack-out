/**
 * @file mainwindow.cpp
 * @brief Implementation detailed in mainwindow.h
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
