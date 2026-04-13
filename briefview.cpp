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
#include "trekrules.h"
#include "levelreportbuilder.h"

namespace {

// Clean display names for UI
QString displayTerrain(TerrainType t) {
    switch(t) {
        case TerrainType::DESERT:   return "Desert";
        case TerrainType::FOREST:   return "Forest";
        case TerrainType::MOUNTAIN: return "Mountain";
        case TerrainType::ROCKY:    return "Rocky";
        default: return "Open Choice";
    }
}

QString displayWeather(WeatherType w) {
    switch(w) {
        case WeatherType::COLD:    return "Cold";
        case WeatherType::HOT:     return "Hot";
        case WeatherType::RAINY:   return "Rainy";
        case WeatherType::WINDY:   return "Windy";
        case WeatherType::SNOW:    return "Snow";
        default: return "Clear";
    }
}

QString displayAdventure(AdventureType a) {
    switch(a) {
        case AdventureType::BACKPACKING: return "Backpacking";
        case AdventureType::CAMPING:     return "Camping";
        case AdventureType::DAY_HIKE:    return "Day Hike";
        default: return "Open Choice";
    }
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

void BriefView::setLevel(const Level& level, int trekNumber)
{
    // Update Title with Trek Number and Name
    QString title = QString("Trek %1: %2").arg(trekNumber).arg(level.getLocationName());
    ui->titleLabel->setText(title);

    // Populate metadata labels
    ui->adventureLabel->setText(displayAdventure(level.getAdventure()));
    ui->terrainLabel->setText(displayTerrain(level.getTerrain()));
    ui->durationLabel->setText(QString::number(level.getDuration()) + " hours");
    
    // Safety check for UI elements that might have been lost in layout swaps
    if (ui->milesLabel) ui->milesLabel->setText(QString::number(level.getMiles()) + " miles");
    if (ui->elevationLabel) ui->elevationLabel->setText(QString::number(level.getElevationGain()) + " feet");
    if (ui->weatherLabel) ui->weatherLabel->setText(displayWeather(level.getWeather()));

    // Generate Madlibs-style Tips based on the trip conditions
    QString tips = "\n\n--- Pro Tips for this Trek ---\n";
    
    // Fetch tips dynamically from feedback.json
    QString weatherKey = TrekRules::weatherToString(level.getWeather());
    QString weatherTip = TrekRules::getTip("weather", weatherKey);
    QString itemWeatherTip = TrekRules::getTip("item_weather", weatherKey);
    if (!weatherTip.isEmpty() || !itemWeatherTip.isEmpty()) {
        tips += "• " + weatherTip + " " + itemWeatherTip + "\n";
    }

    QString terrainKey = TrekRules::terrainToString(level.getTerrain());
    QString terrainTip = TrekRules::getTip("terrain", terrainKey);
    QString itemTerrainTip = TrekRules::getTip("item_terrain", terrainKey);
    if (!terrainTip.isEmpty() || !itemTerrainTip.isEmpty()) {
        tips += "• " + terrainTip + " " + itemTerrainTip + "\n";
    }

    if (level.getAdventure() == AdventureType::BACKPACKING || level.getAdventure() == AdventureType::CAMPING) {
        QString advTip = TrekRules::getTip("adventure", "OVERNIGHT");
        QString itemAdvTip = TrekRules::getTip("item_adventure", "OVERNIGHT");
        if (!advTip.isEmpty() || !itemAdvTip.isEmpty()) {
            tips += "• " + advTip + " " + itemAdvTip + "\n";
        }
    }

    if (level.getDuration() > 8) {
        QString durTip = TrekRules::getTip("duration", "LONG");
        if (!durTip.isEmpty()) tips += "• " + durTip + "\n";
    }

    // Set the final description
    QString fullText = level.getDescription() + tips;
    
    // Append the specific scientific focus for this level
    QString scienceTip = LevelReportBuilder::getLessonByKey(level.getScienceFocus());
    if (!scienceTip.isEmpty()) {
        fullText += scienceTip;
    }
    
    ui->briefingTextEdit->setText(fullText);
}
