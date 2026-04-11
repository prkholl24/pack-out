#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "levelreportbuilder.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize Utah Treks
    treks.push_back(Level("Angels Landing", -1, -1, 4, TerrainType::ROCKY, AdventureType::DAY_HIKE, WeatherType::HOT, 5.4, 1488.0));
    treks.push_back(Level("The Narrows", -1, -1, 8, TerrainType::ROCKY, AdventureType::DAY_HIKE, WeatherType::RAINY, 9.4, 334.0));
    treks.push_back(Level("Delicate Arch", -1, -1, 2, TerrainType::DESERT, AdventureType::DAY_HIKE, WeatherType::HOT, 3.0, 480.0));
    treks.push_back(Level("Mount Timpanogos", -1, -1, 10, TerrainType::MOUNTAIN, AdventureType::DAY_HIKE, WeatherType::SNOW, 14.0, 4400.0));
    treks.push_back(Level("Canyonlands Backpacking", -1, -1, 48, TerrainType::DESERT, AdventureType::BACKPACKING, WeatherType::WINDY, 25.0, 3000.0));
    treks.push_back(Level("Lake Blanche", -1, -1, 5, TerrainType::FOREST, AdventureType::DAY_HIKE, WeatherType::COLD, 6.9, 2700.0));
    treks.push_back(Level("Wall Street (Bryce)", -1, -1, 3, TerrainType::ROCKY, AdventureType::DAY_HIKE, WeatherType::HOT, 3.2, 600.0));
    treks.push_back(Level("Grandview Point", -1, -1, 1, TerrainType::DESERT, AdventureType::DAY_HIKE, WeatherType::WINDY, 2.0, 160.0));
    treks.push_back(Level("Uinta Camping", -1, -1, 24, TerrainType::FOREST, AdventureType::CAMPING, WeatherType::RAINY, 5.0, 1000.0));
    treks.push_back(Level("Lone Peak", -1, -1, 12, TerrainType::MOUNTAIN, AdventureType::DAY_HIKE, WeatherType::COLD, 11.0, 5500.0));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addPackItem(Item* item)
{
    player.addItem(item);
    // TODO: Update UI to show item in backpack
}

void MainWindow::evaluateTrek(int trekIndex)
{
    if (trekIndex < 0 || trekIndex >= (int)treks.size())
        return;

    LevelResult result = treks[trekIndex].evaluateBackpack(player.getBackpack(), player);
    QString report = LevelReportBuilder::buildReport(result);

    // TODO: Update UI with report
}
