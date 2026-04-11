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
#include "briefview.h"
#include "debriefview.h"
#include "level.h"
#include "levelresult.h"

#include <QMenuBar>
#include <QStackedWidget>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , levelOne(0, 32.0, 2, TerrainType::FOREST, AdventureType::DAY_HIKE)
    , levelTwo(1200, 64.0, 8, TerrainType::MOUNTAIN, AdventureType::BACKPACKING)
    , currentLevel(nullptr)
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

    // Create all scene views. addWidget() parents each to the stack,
    // so Qt's parent-child ownership handles cleanup.
    briefView = new BriefView(this);
    backpackScene = new BackpackScene(this);
    debriefView = new DebriefView(this);

    sceneStack->addWidget(briefView);
    sceneStack->addWidget(backpackScene);
    sceneStack->addWidget(debriefView);

    // Start on level one. currentLevel is a non-owning pointer into
    // the levelOne/levelTwo members, so no delete is needed.
    currentLevel = &levelOne;

    // Initialize the brief view with the current level and start there.
    briefView->setLevel(*currentLevel);
    sceneStack->setCurrentWidget(briefView);

    // Connect BriefView's start button to transition to packing.
    connect(briefView, &BriefView::startPackingClicked,
            this, &MainWindow::showBackpackScene);

    // When the packing scene completes (player clicked Cinch & Go
    // and the button exited the screen), transition to the debrief.
    connect(backpackScene, &BackpackScene::levelHasStarted,
            this, &MainWindow::showDebriefView);

    // Connect DebriefView navigation signals.
    connect(debriefView, &DebriefView::retryLevelClicked, this, [this]() {
        // Retry: show brief view for the same level.
        briefView->setLevel(*currentLevel);
        showBriefView();
    });
    connect(debriefView, &DebriefView::nextLevelClicked, this, [this]() {
        // Next level: for now, just reload the same level.
        // Future: advance to next level in sequence.
        briefView->setLevel(*currentLevel);
        showBriefView();
    });
    connect(debriefView, &DebriefView::returnToMenuClicked, this, [this]() {
        // Return to menu: for now, just go back to brief view.
        // Future: implement main menu.
        showBriefView();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showBriefView()
{
    sceneStack->setCurrentWidget(briefView);
}

void MainWindow::showBackpackScene()
{
    sceneStack->setCurrentWidget(backpackScene);
}

void MainWindow::showDebriefView()
{
    // Create a result from the current level.
    // For now, use placeholder packed items (empty vector).
    // Future: get actual packed items from BackpackScene.
    std::vector<Item*> packedItems;
    LevelResult result;
    result.grade = currentLevel->gradeMyBag(packedItems);
    result.passed = currentLevel->passesLevel(packedItems);
    result.requiredCalories = currentLevel->getRequiredCalories();
    result.requiredHydration = currentLevel->getRequiredHydration();

    debriefView->setResults(result, packedItems);
    sceneStack->setCurrentWidget(debriefView);
}
