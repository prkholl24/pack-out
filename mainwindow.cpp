#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , levelOne(0, 32.0, 2, TerrainType::FOREST, AdventureType::DAY_HIKE)
    , levelTwo(1200, 64.0, 8, TerrainType::MOUNTAIN, AdventureType::BACKPACKING)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
