/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow scene switcher.
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "backpack.h"
#include "briefview.h"
#include "debriefview.h"
#include "drinkitem.h"
#include "fooditem.h"
#include "gearitem.h"
#include "levelreportbuilder.h"
#include "trekregistry.h"

#include <QMenuBar>
#include <QStackedWidget>
#include <QStatusBar>

#include "itemregistry.h"

namespace {

Item* createPackedItemFromSceneName(const QString& itemName)
{
    ItemRegistry::ensureLoaded();

    // Check food
    ItemStats stats = ItemRegistry::getFoodStats(itemName);
    if (!stats.description.isEmpty()) {
        return new FoodItem(itemName);
    }

    // Check drink
    stats = ItemRegistry::getDrinkStats(itemName);
    if (!stats.description.isEmpty()) {
        return new DrinkItem(itemName);
    }

    // Check gear
    stats = ItemRegistry::getGearStats(itemName);
    if (!stats.description.isEmpty()) {
        return new GearItem(itemName);
    }

    return nullptr;
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentTrekIndex(0)
{
    ui->setupUi(this);

    // Lock the window geometry and strip chrome
    setFixedSize(800, 600);
    menuBar()->hide();
    statusBar()->hide();

    // Set up the scene stack
    sceneStack = new QStackedWidget(this);
    setCentralWidget(sceneStack);

    // Initialize all scene views
    briefView = new BriefView(this);
    backpackScene = new BackpackScene(QStringList(), this);
    backpackScene->setPlayer(&player);
    debriefView = new DebriefView(this);
    debriefView->setPlayer(&player);

    sceneStack->addWidget(briefView);
    sceneStack->addWidget(backpackScene);
    sceneStack->addWidget(debriefView);

    // Initialize Utah Treks from external data
    treks = TrekRegistry::loadTreks();

    // Set up Signal/Slot connections for the Player Model
    connect(&player, &Player::backpackChanged, this, &MainWindow::onBackpackChanged);
    connect(&player, &Player::statsChanged, this, &MainWindow::onStatsChanged);

    // Connect BriefView's start button to transition to packing
    connect(briefView, &BriefView::startPackingClicked, this, &MainWindow::showBackpackScene);

    connectBackpackSceneSignals();

    // Connect DebriefView navigation signals
    connect(debriefView, &DebriefView::retryLevelClicked, this, [this]() {
        showBriefView();
    });
    connect(debriefView, &DebriefView::nextLevelClicked, this, [this]() {
        advanceToNextLevel();
        showBriefView();
    });
    connect(debriefView, &DebriefView::returnToMenuClicked, this, [this]() {
        showBriefView();
    });

    // Start on the first trek
    showBriefView();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addPackItem(Item* item)
{
    if (item) {
        player.addItem(std::unique_ptr<Item>(item));
    }
}

void MainWindow::evaluateTrek(int trekIndex)
{
    if (trekIndex < 0 || trekIndex >= (int)treks.size())
        return;

    LevelResult result = treks[trekIndex].evaluateBackpack(player.getBackpack(), player);
    debriefView->setResults(result);
    showDebriefView();
}

void MainWindow::onBackpackChanged()
{
    // Refresh UI components that depend on backpack content
}

void MainWindow::onStatsChanged(double weight, double volume)
{
    if (backpackScene) {
        backpackScene->updateStats(weight, volume);
    }
}

void MainWindow::showBriefView()
{
    if (currentTrekIndex >= 0 && currentTrekIndex < (int)treks.size()) {
        briefView->setLevel(treks[currentTrekIndex], currentTrekIndex + 1);
    }
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
    if (currentTrekIndex >= 0 && currentTrekIndex < (int)treks.size()) {
        const auto& packedItems = player.getBackpack();
        LevelResult result = treks[currentTrekIndex].evaluateBackpack(packedItems, player);
        debriefView->setResults(result);
    }
    sceneStack->setCurrentWidget(debriefView);
}

void MainWindow::handlePackedItemAdded(const QString& itemName)
{
    Item* item = createPackedItemFromSceneName(itemName);
    if (item == nullptr)
        return;

    packedItemsByName[itemName].push_back(item);
    player.addItem(std::unique_ptr<Item>(item));
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
    connect(backpackScene, &BackpackScene::levelHasStarted, this, &MainWindow::showDebriefView);
    connect(backpackScene, &BackpackScene::itemAddedToBag, this, &MainWindow::handlePackedItemAdded);
    connect(backpackScene, &BackpackScene::itemRemovedFromBag, this, &MainWindow::handlePackedItemRemoved);
}

void MainWindow::rebuildBackpackScene()
{
    const int backpackSceneIndex = sceneStack->indexOf(backpackScene);

    if (backpackSceneIndex >= 0) {
        sceneStack->removeWidget(backpackScene);
        backpackScene->deleteLater();
    }

    QStringList items;
    if (currentTrekIndex >= 0 && currentTrekIndex < (int)treks.size())
        items = treks[currentTrekIndex].getAvailableItems();
    backpackScene = new BackpackScene(items, this);
    backpackScene->setPlayer(&player);
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
    if (currentTrekIndex < (int)treks.size() - 1) {
        currentTrekIndex++;
    }
}
