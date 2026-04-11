#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "levelreportbuilder.h"
#include "trekregistry.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize Utah Treks from external data
    treks = TrekRegistry::loadTreks();

    // Set up Signal/Slot connections for the Player Model
    connect(&player, &Player::backpackChanged, this, &MainWindow::onBackpackChanged);
    connect(&player, &Player::statsChanged, this, &MainWindow::onStatsChanged);
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
    // TODO: Update weight/volume progress bars or labels
    // Example: ui->weightLabel->setText(QString::number(weight) + " lbs");
}
