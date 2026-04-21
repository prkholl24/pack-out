/**
 * @file level.h
 * @brief Header file for the Level orchestrator class.
 */

#ifndef LEVEL_H
#define LEVEL_H

#include <QString>
#include <vector>
#include <memory>
#include "item.h"
#include "levelresult.h"
#include "tripconditions.h"
#include "player.h"

enum class PenaltyType {
    MISSING_GEAR,
    BULKY_PACK,
    LNT_VIOLATION,
    MISSING_ESSENTIALS
};

/**
 * @brief Orchestrates the backpack evaluation process.
 */
class Level
{
public:
    Level();
    Level(const TripRequirements& req);
    virtual ~Level() = default;

    /**
     * @brief Performs a full evaluation of the player's backpack.
     */
    LevelResult evaluateBackpack(const std::vector<std::unique_ptr<Item>>& backpack,
                                 const Player& player = Player{}) const;

    virtual double gradeMyBag(const std::vector<std::unique_ptr<Item>>& backpack,
                              const Player& player = Player{}) const;

    virtual bool passesLevel(const std::vector<std::unique_ptr<Item>>& backpack,
                             const Player& player = Player{}) const;

    // --- Getters ---
    QString getLocationName() const { return requirements.locationName; }
    WeatherType getWeather() const { return requirements.weather; }
    TerrainType getTerrain() const { return requirements.terrain; }
    AdventureType getAdventure() const { return requirements.adventure; }
    int getDuration() const { return requirements.durationHours; }
    double getMiles() const { return requirements.miles; }
    double getElevationGain() const { return requirements.elevationGain; }
    double getRequiredHydration() const { return requirements.customHydration; }
    int getRequiredCalories() const { return requirements.customCalories; }
    bool getSkipGearCheck() const { return requirements.skipGearCheck; }
    QString getScienceFocus() const { return requirements.scienceFocus; }
    QString getDescription() const { return requirements.description; }
    QStringList getAvailableItems() const { return requirements.availableItems; }

    /**
     * @brief Sets the custom briefing description for this level.
     * @param newDescription The custom level description to display.
     */
    void setDescription(const QString& newDescription);

private:
    // --- Orchestration Phases ---
    
    BackpackTotals gatherBackpackTotals(const std::vector<std::unique_ptr<Item>>& backpack) const;
    void applyEvaluationScoring(LevelResult& result) const;
    void applyEvaluationPenalties(LevelResult& result) const;
    void generateStatusConditions(LevelResult& result) const;
    ScienceLesson determineScienceLesson(const LevelResult& result) const;

    // --- Internal Logic Helpers ---
    double calculateEnergyScore(double calories, double required, double sugar) const;
    double calculateHydrationScore(double effective, double required) const;
    double calculateOverpackDeduction(double actual, double required) const;
    void applyDiureticPenalties(BackpackTotals& totals) const;

    TripRequirements requirements;
};

#endif // LEVEL_H
