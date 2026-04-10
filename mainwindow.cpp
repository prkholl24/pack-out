/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow scene switcher.
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
#include "backpack.h"

#include <QMenuBar>
#include <QStackedWidget>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Lock the window geometry and strip chrome so the full
    // client area is available to whichever scene is active.
    setFixedSize(800, 600);
    menuBar()->hide();
    statusBar()->hide();

    // Central stack that holds every scene widget.
    sceneStack = new QStackedWidget(this);
    setCentralWidget(sceneStack);

    // Start on the packing scene.
    backpackScene = new BackpackScene(this);
    sceneStack->addWidget(backpackScene);
    sceneStack->setCurrentWidget(backpackScene);

    // When the packing scene completes, the controller/model
    // layer will switch to the next scene here.
    connect(backpackScene, &BackpackScene::levelHasStarted, this, [this]() {
        // TODO: switch to the trip / grading scene
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
