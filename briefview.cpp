/**
 * @file briefview.cpp
 * @brief Implementation of the BriefView widget.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison, Claude Opus 4.5
 * @date 4/10/2026
 */
#include "briefview.h"
#include "ui_briefview.h"
#include "level.h"

BriefView::BriefView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BriefView)
{
    ui->setupUi(this);

    // Connect the Start Packing button to emit our signal.
    connect(ui->startPackingButton, &QPushButton::clicked,
            this, &BriefView::startPackingClicked);
}

BriefView::~BriefView()
{
    delete ui;
}

void BriefView::setLevel(const Level& level)
{
    // Populate placeholder text for location/weather/terrain.
    // These will be replaced when Level gains those properties.
    ui->locationLabel->setText("Location TBD");
    ui->weatherLabel->setText("Weather TBD");
    ui->terrainLabel->setText("Terrain TBD");

    // Populate actual data from the Level.
    ui->durationLabel->setText(QString::number(level.getDuration()) + " hours");
    ui->caloriesLabel->setText(QString::number(level.getRequiredCalories()) + " cal");
    ui->hydrationLabel->setText(QString::number(level.getRequiredHydration(), 'f', 1) + " oz");

    // Placeholder packing hints.
    ui->hintsTextEdit->setText(
        "Packing Hints:\n"
        "- Pack enough food to meet calorie requirements\n"
        "- Bring water or sports drinks for hydration\n"
        "- Avoid too much caffeine as it reduces hydration effectiveness"
    );
}
