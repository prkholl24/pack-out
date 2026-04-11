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
#include "drinkitem.h"
#include "fooditem.h"
#include "level.h"

#include <QMenuBar>
#include <QStackedWidget>
#include <QStatusBar>

namespace {

Item* createPackedItemFromSceneName(const QString& itemName)
{
    if (itemName == QStringLiteral("BANANA"))
        return new FoodItem(Food::BANANA);

    if (itemName == QStringLiteral("POTATO"))
        return new FoodItem(Food::POTATO);

    if (itemName == QStringLiteral("BOTTLE"))
        return new DrinkItem(DrinkType::WATER_BOTTLE);

    return nullptr;
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , levelOne(0, 32.0, 2, TerrainType::ALL, AdventureType::ALL)
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
    levelOne.setDescription(
        "Welcome to PACK OUT!\n\n"
        "Your first trip is a short two-hour hike. It should be manageable, "
        "but only if you bring enough to stay hydrated while you are out on the trail.\n\n"
        "Pack what you think you need and see if your bag can get you through it!"
    );
    levelTwo.setDescription(
        "The next trip is longer, steeper, and much less forgiving.\n\n"
        "You are heading into the mountains for an extended backpacking trek. "
        "This time, both food and water matter, and a weak packout can leave you in real trouble.\n\n"
        "Think carefully about what you bring. Weight still matters, but so does being prepared for the terrain."
    );

    currentLevel = &levelOne;

    // Initialize the brief view with the current level and start there.
    briefView->setLevel(*currentLevel);
    sceneStack->setCurrentWidget(briefView);

    // Connect BriefView's start button to transition to packing.
    connect(briefView, &BriefView::startPackingClicked,
            this, &MainWindow::showBackpackScene);

    connectBackpackSceneSignals();

    // Connect DebriefView navigation signals.
    connect(debriefView, &DebriefView::retryLevelClicked, this, [this]() {
        // Retry: show brief view for the same level.
        briefView->setLevel(*currentLevel);
        showBriefView();
    });
    connect(debriefView, &DebriefView::nextLevelClicked, this, [this]() {
        advanceToNextLevel();
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
    rebuildBackpackScene();
    resetAttemptState();
    sceneStack->setCurrentWidget(backpackScene);
}

void MainWindow::showDebriefView()
{
    const std::vector<Item*>& packedItems = player.getBackpack();
    const LevelResult result = currentLevel->evaluate(packedItems);

    debriefView->setResults(result);
    sceneStack->setCurrentWidget(debriefView);
}

void MainWindow::handlePackedItemAdded(const QString& itemName)
{
    Item* item = createPackedItemFromSceneName(itemName);

    if (item == nullptr)
        return;

    player.addItem(item);
    packedItemsByName[itemName].push_back(item);
}

void MainWindow::handlePackedItemRemoved(const QString& itemName)
{
    auto found = packedItemsByName.find(itemName);

    if (found == packedItemsByName.end() || found->second.empty())
        return;

    Item* item = found->second.back();
    found->second.pop_back();
    player.removeItem(item);

    if (found->second.empty())
        packedItemsByName.erase(found);
}

void MainWindow::connectBackpackSceneSignals()
{
    connect(backpackScene, &BackpackScene::levelHasStarted,
            this, &MainWindow::showDebriefView);
    connect(backpackScene, &BackpackScene::itemAddedToBag,
            this, &MainWindow::handlePackedItemAdded);
    connect(backpackScene, &BackpackScene::itemRemovedFromBag,
            this, &MainWindow::handlePackedItemRemoved);
}

void MainWindow::rebuildBackpackScene()
{
    const int backpackSceneIndex = sceneStack->indexOf(backpackScene);

    if (backpackSceneIndex >= 0)
    {
        sceneStack->removeWidget(backpackScene);
        backpackScene->deleteLater();
    }

    backpackScene = new BackpackScene(this);
    sceneStack->insertWidget(backpackSceneIndex >= 0 ? backpackSceneIndex : 1, backpackScene);
    connectBackpackSceneSignals();
}

void MainWindow::resetAttemptState()
{
    packedItemsByName.clear();
    player.clearBackpack();
}

void MainWindow::advanceToNextLevel()
{
    if (currentLevel == &levelOne)
        currentLevel = &levelTwo;
}
