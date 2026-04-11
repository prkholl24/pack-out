/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow scene switcher.
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "backpack.h"
#include "levelreportbuilder.h"
#include "trekregistry.h"

#include <QMenuBar>
#include <QStackedWidget>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Lock the window geometry and strip chrome
    setFixedSize(800, 600);
    menuBar()->hide();
    statusBar()->hide();

    // Set up the scene stack
    sceneStack = new QStackedWidget(this);
    setCentralWidget(sceneStack);

    // Initialize the packing scene
    backpackScene = new BackpackScene(this);
    sceneStack->addWidget(backpackScene);
    sceneStack->setCurrentWidget(backpackScene);

    // Initialize Utah Treks from external data
    treks = TrekRegistry::loadTreks();

    // Set up Signal/Slot connections for the Player Model
    connect(&player, &Player::backpackChanged, this, &MainWindow::onBackpackChanged);
    connect(&player, &Player::statsChanged, this, &MainWindow::onStatsChanged);

    // Connect scene transition
    connect(backpackScene, &BackpackScene::levelHasStarted, this, []() {
        // TODO: switch to the trip / grading scene
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addPackItem(Item* item)
{
    if (item) {
        // MainWindow wraps raw pointer from UI into unique_ptr for Model
        player.addItem(std::unique_ptr<Item>(item));
    }
}

void MainWindow::evaluateTrek(int trekIndex)
{
    if (trekIndex < 0 || trekIndex >= (int)treks.size())
        return;

    LevelResult result = treks[trekIndex].evaluateBackpack(player.getBackpack(), player);
    QString report = LevelReportBuilder::buildReport(result);

    // TODO: Display report in UI
}

void MainWindow::onBackpackChanged()
{
    // TODO: Refresh the backpack list widget
}

void MainWindow::onStatsChanged(double weight, double volume)
{
    Q_UNUSED(weight);
    Q_UNUSED(volume);
    // TODO: Update weight/volume progress bars or labels
}
