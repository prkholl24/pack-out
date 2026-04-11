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
#include "tripconditions.h"

namespace {

// Converts a TerrainType enum to a human-readable string for the
// trip-conditions panel. Kept local to briefview because the
// mapping is purely presentational.
QString terrainToString(TerrainType terrain)
{
    switch (terrain) {
    case TerrainType::DESERT:   return QStringLiteral("Desert");
    case TerrainType::FOREST:   return QStringLiteral("Forest");
    case TerrainType::MOUNTAIN: return QStringLiteral("Mountain");
    case TerrainType::ROCKY:    return QStringLiteral("Rocky");
    case TerrainType::ALL:      return QStringLiteral("Open Choice");
    }
    return QStringLiteral("Unknown");
}

// Converts an AdventureType enum to a human-readable string.
QString adventureToString(AdventureType adventure)
{
    switch (adventure) {
    case AdventureType::BACKPACKING: return QStringLiteral("Backpacking");
    case AdventureType::CAMPING:     return QStringLiteral("Camping");
    case AdventureType::DAY_HIKE:    return QStringLiteral("Day Hike");
    case AdventureType::ALL:         return QStringLiteral("Open Choice");
    }
    return QStringLiteral("Unknown");
}

} // namespace

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
    // Show only broad trip metadata here. The authored description
    // carries the mission flavor; hidden scoring requirements stay
    // out of the brief.
    ui->adventureLabel->setText(adventureToString(level.getAdventure()));
    ui->terrainLabel->setText(terrainToString(level.getTerrain()));
    ui->durationLabel->setText(QString::number(level.getDuration()) + " hours");
    ui->briefingTextEdit->setText(level.getDescription());
}
