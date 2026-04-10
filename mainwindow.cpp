/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow scene switcher.
 *
 * The constructor builds the scene stack, adds the BackpackScene,
 * and wires the levelHasStarted signal to a TODO stub that will
 * eventually swap to the trip/grading scene.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison, Claude Opus 4.6 (1M context)
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

    // Lock the window geometry and strip chrome so the full client
    // area is available to whichever scene is active. setFixedSize
    // also prevents resizing, which keeps the screen ↔ world
    // coordinate math in BackpackScene stable.
    setFixedSize(800, 600);
    menuBar()->hide();
    statusBar()->hide();

    // QStackedWidget displays exactly one child at a time.  We set
    // it as the central widget so it fills the entire client area.
    sceneStack = new QStackedWidget(this);
    setCentralWidget(sceneStack);

    // Start on the packing scene.  addWidget() parents the scene
    // to the stack, so Qt's parent-child ownership handles cleanup.
    backpackScene = new BackpackScene(this);
    sceneStack->addWidget(backpackScene);
    sceneStack->setCurrentWidget(backpackScene);

    // When the packing scene completes (player clicked Cinch & Go
    // and the button exited the screen), the model/controller layer
    // will swap to the next scene here.  For now it's a no-op stub.
    //
    // Example of how a future transition would look:
    //   sceneStack->setCurrentWidget(tripScene);
    connect(backpackScene, &BackpackScene::levelHasStarted, this, [this]() {
        // TODO: switch to the trip / grading scene
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
