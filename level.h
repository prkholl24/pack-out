/**
 * @file level.h
 * @brief Header file for the Level class.
 *
 * Defines the Level class, which represents one backpacking challenge.
 * A level stores the requirements for a trip, grades the player's
 * backpack, and determines whether the player passed.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 * Reviewed by Charles Adair
 */

#ifndef LEVEL_H
#define LEVEL_H

#include <QString>
#include <vector>
#include "item.h"
#include "levelresult.h"
#include "tripconditions.h"
#include "player.h"

/**
 * @brief Represents a single backpacking level.
 *
 * A Level defines the physical challenge (distance, gain, terrain)
 * and calculates the resources needed by a specific player.
 */
class Level
{
public:
    /**
     * @brief Constructs a default Level.
     *
     * Initializes a level with default requirement values.
     */
    Level();

    /**
     * @brief Constructs a Level with specific trip requirements.
     *
     * @param name The name of the trek (e.g. "Angels Landing").
     * @param calories The minimum calories required for the trip.
     * @param hydration The minimum hydration required for the trip, in ounces.
     * @param duration The duration of the trip in hours.
     * @param terrain The terrain type of the trip.
     * @param adventure The adventure type of the trip.
     * @param weather The weather conditions.
     * @param miles The distance of the trip in miles.
     * @param elevationGain The vertical gain in feet.
     */
    Level(QString name, int calories, double hydration, int duration,
          TerrainType terrain, AdventureType adventure, WeatherType weather,
          double miles = 0.0, double elevationGain = 0.0);

    /**
     * @brief Virtual destructor for Level.
     */
    virtual ~Level() = default;

    /**
     * @brief Gets the name of the trek location.
     *
     * @return The location name.
     */
    QString getLocationName() const;

    /**
     * @brief Gets the weather type of this level.
     *
     * @return The weather type.
     */
    WeatherType getWeather() const;

    /**
     * @brief Gets the hydration required for this level.
     *
     * @return The minimum hydration needed in ounces.
     */
    double getRequiredHydration() const;

    /**
     * @brief Gets the trip duration of this level.
     *
     * @return The trip duration in hours.
     */
    int getDuration() const;

    /**
     * @brief Gets the terrain type of this level.
     *
     * @return The terrain type.
     */
    TerrainType getTerrain() const;

    /**
     * @brief Gets the adventure type of this level.
     *
     * @return The adventure type.
     */
    AdventureType getAdventure() const;

    /**
     * @brief Gets the description of this level.
     *
     * @return A short description of the level scenario.
     */
    QString getDescription() const;

    /**
     * @brief Grades the preparedness of the backpack.
     *
     * Evaluates the contents of the backpack and assigns
     * a score from 0 to 100 based on how well it meets
     * the level requirements for a specific player.
     *
     * @param backpack The collection of packed items.
     * @param player The player's profile (weight, etc.)
     * @return A grade from 0 to 100 representing preparedness.
     */
    virtual double gradeMyBag(const std::vector<Item*>& backpack,
                              const Player& player = Player{}) const;

    /**
     * @brief Determines whether the backpack passes the level.
     *
     * This method is intended to support level progression.
     * A backpack passes the level if its grade meets or exceeds
     * the passing threshold.
     *
     * @param backpack The collection of packed items being evaluated.
     * @param player The player's profile.
     * @return True if the backpack passes the level; otherwise false.
     */
    virtual bool passesLevel(const std::vector<Item*>& backpack,
                             const Player& player = Player{}) const;

    /**
     * @brief Evaluates the backpack and computes total stats.
     *
     * This method provides the raw data (LevelResult) to the View/Controller,
     * allowing it to handle presentation logic.
     *
     * @param backpack The items being evaluated.
     * @param player The player's profile.
     * @return The aggregated LevelResult.
     */
    LevelResult evaluateBackpack(const std::vector<Item*>& backpack,
                                 const Player& player = Player{}) const;

private:
    // ---------- Helper methods ----------
    // --- Data collection ---

    /**
     * @brief Helper method that calculates the total calories in the backpack.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return The total calories packed.
     */
    double calculateTotalCalories(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the total caffeine in the backpack.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return The total caffeine packed.
     */
    double calculateTotalCaffeine(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the total alcohol in the backpack.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return The total alcohol packed.
     */
    double calculateTotalAlcohol(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the effective hydration in the backpack.
     *
     * Effective hydration is computed using DrinkItem hydration multipliers
     * and reduced by caffeine and alcohol penalties.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return The effective hydration packed in ounces.
     */
    double calculateEffectiveHydration(const std::vector<Item*>& backpack) const;

    // --- Scoring ---

    /**
     * @brief Helper method that calculates the hydration portion of the grade.
     *
     * Computes the hydration contribution to the overall score.
     *
     * @param effectiveHydration The effective hydration packed.
     * @param requiredHydration The calculated hydration needed.
     * @return The hydration contribution to the final score.
     */
    double calculateHydrationScore(double effectiveHydration,
                                   double requiredHydration) const;

    /**
     * @brief Helper method that calculates the total sugar in the backpack.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return The total sugar packed in grams.
     */
    double calculateTotalSugar(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the total fiber in the backpack.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return The total fiber packed in grams.
     */
    double calculateTotalFiber(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the total sodium in the backpack.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return The total sodium packed in milligrams.
     */
    double calculateTotalSodium(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the total weight in the backpack.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return The total weight packed in pounds.
     */
    double calculateTotalWeight(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that checks if the backpack contains required gear.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return True if all required gear items are present; otherwise false.
     */
    bool hasRequiredGear(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that checks if the backpack has weather-appropriate gear.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return True if weather-appropriate gear is found; otherwise false.
     */
    bool hasWeatherGear(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the energy portion of the grade.
     *
     * @param totalCalories The total calories packed.
     * @param requiredCalories The calculated energy needed.
     * @param totalCaffeine The total caffeine contributing to energy.
     * @param totalSugar The total sugar contributing to energy.
     * @return The energy contribution to the final score.
     */
    double calculateEnergyScore(double totalCalories, double requiredCalories,
                                double totalCaffeine, double totalSugar) const;

    // ---------- Member variables ----------

    /**
     * @brief The distance of the trip in miles.
     */
    double miles = 0.0;

    /**
     * @brief The total vertical elevation gain in feet.
     */
    double elevationGain = 0.0;

    /**
     * @brief The number of calories required to complete the trip.
     *
     * If set to -1, calories will be calculated based on miles/gain/hiker.
     */
    int requiredCalories = -1;

    /**
     * @brief The minimum effective hydration required to complete the trip.
     *
     * Represents the amount of usable hydration needed after accounting
     * for drink quality and penalties (e.g., caffeine).
     */
    double requiredHydration = -1;

    /**
     * @brief The duration of the trip, measured in hours.
     *
     * Represents how long the level lasts and may influence
     * the required resources.
     */
    int duration = -1;

    /**
     * @brief A short description of the level scenario.
     *
     * Provides context or instructions for the player.
     */
    QString description;

    /**
     * @brief The name of the trek location.
     */
    QString locationName;

    /**
     * @brief Gets the required calories for this level.
     *
     * @return The minimum calories needed.
     */
    int getRequiredCalories() const;

    /**
     * @brief The terrain type for this level.
     */
    TerrainType requiredTerrain = TerrainType::ALL;

    /**
     * @brief The adventure type for this level.
     */
    AdventureType requiredAdventure = AdventureType::ALL;

    /**
     * @brief The weather condition for this level.
     */
    WeatherType requiredWeather = WeatherType::DEFAULT;

    /**
     * @brief The minimum grade required to pass the level.
     */
    double passingScore;
};

#endif // LEVEL_H
