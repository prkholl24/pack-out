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

/**
 * @brief Identifies specific scoring penalty types.
 */
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
    double getRequiredHydration() const { return requirements.customHydration; }
    int getRequiredCalories() const { return requirements.customCalories; }
    QString getDescription() const { return requirements.description; }

private:
    // --- Orchestration Phases ---
    
    BackpackTotals gatherBackpackTotals(const std::vector<std::unique_ptr<Item>>& backpack) const;
    void mapTotalsToResult(LevelResult& result, const BackpackTotals& totals, const Player& player) const;
    void applyEvaluationScoring(LevelResult& result, const BackpackTotals& totals) const;
    void applyEvaluationPenalties(LevelResult& result) const;
    void generateStatusTags(LevelResult& result) const;
    ScienceLesson determineScienceLesson(const LevelResult& result) const;

    // --- Internal Logic Helpers ---
    double calculateEnergyScore(double calories, double required, double sugar) const;
    double calculateHydrationScore(double effective, double required) const;
    void applyDiureticPenalties(BackpackTotals& totals) const;

    TripRequirements requirements;
};

#endif // LEVEL_H
