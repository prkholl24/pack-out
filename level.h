/**
 * @file level.h
 * @brief Header file for the Level class
 *
 * The Level class represents one backpacking challenge.
 * Each level defines the minimum resources the player must
 * pack to successfully complete the trip. It also provides
 * functionality for grading a backpack, determining whether
 * the player passed the level, and generating feedback.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 */

#ifndef LEVEL_H
#define LEVEL_H

#include <QString>
#include <vector>
#include "item.h"

/**
 * @brief Represents a single backpacking level.
 *
 * A Level defines the minimum resources needed for a trip
 * and evaluates a backpack by assigning a preparedness score
 * based on its contents. It can also determine whether the
 * backpack passes the level and generate feedback.
 *
 * This class is designed to be extended, allowing different
 * levels to implement custom grading behavior.
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
     * @param calories The minimum calories required for the trip.
     * @param water The minimum water required for the trip, in ounces.
     * @param duration The duration of the trip in hours.
     */
    Level(int calories, double water, int duration);

    /**
     * @brief Virtual destructor for Level.
     */
    virtual ~Level() = default;

    /**
     * @brief Gets the calories required for this level.
     *
     * @return The minimum calories needed.
     */
    int getRequiredCalories() const;

    /**
     * @brief Gets the water required for this level.
     *
     * @return The minimum water needed in ounces.
     */
    double getRequiredWater() const;

    /**
     * @brief Gets the trip duration of this level.
     *
     * @return The trip duration in hours.
     */
    int getDuration() const;

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
     * the level requirements.
     *
     * @param backpack The collection of packed items.
     * @return A grade from 0 to 100 representing preparedness.
     */
    virtual double gradeMyBag(const std::vector<Item*>& backpack) const;

    /**
     * @brief Determines whether the backpack passes the level.
     *
     * This method is intended to support level progression.
     * A backpack passes the level if its grade meets or exceeds
     * the passing threshold.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return True if the backpack passes the level; otherwise false.
     */
    virtual bool passesLevel(const std::vector<Item*>& backpack) const;

    /**
     * @brief Generates written feedback about the backpack.
     *
     * The returned message describes how well the backpack
     * meets the level requirements and may identify missing
     * resources.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return A QString containing a report describing the backpack's preparedness.
     */
    virtual QString getReport(const std::vector<Item*>& backpack) const;

private:

    // ---------- Helper methods ----------
    // --- Data collection ---
    /**
     * @brief Helper method that calculates the total calories in the backpack.
     *
     * Iterates through all packed items and sums their calorie values.
     * Implemented in level.cpp.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return The total calories packed.
     */
    double calculateTotalCalories(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the total water in the backpack.
     *
     * Iterates through all packed items and sums their water amounts.
     * Implemented in level.cpp.
     *
     * @param backpack The collection of packed items being evaluated.
     * @return The total water packed.
     */
    double calculateTotalWater(const std::vector<Item*>& backpack) const;

    // --- Scoring ---
    /**
     * @brief Helper method that calculates the calorie portion of the grade.
     *
     * Computes the calorie contribution to the overall score.
     * Implemented in level.cpp.
     *
     * @param totalCalories The total calories packed.
     * @return The calorie contribution to the final score.
     */
    double calculateCalorieScore(double totalCalories) const;

    /**
     * @brief Helper method that calculates the water portion of the grade.
     *
     * Computes the water contribution to the overall score.
     * Implemented in level.cpp.
     *
     * @param totalWater The total water packed.
     * @return The water contribution to the final score.
     */
    double calculateWaterScore(double totalWater) const;

    // --- Report building ---
    /**
     * @brief The number of calories required to complete the trip.
     *
     * Used when grading the backpack to determine whether
     * the player has packed enough food.
     */
    int requiredCalories = -1;

    /**
     * @brief The number of ounces of water required to complete the trip.
     *
     * Used when grading the backpack to determine whether
     * the player has packed enough water.
     */
    double requiredWater = -1;

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
     * @brief The minimum grade required to pass the level.
     *
     * A backpack must achieve at least this score to pass.
     */
    double passingScore = 70.0;

    /**
     * @brief Helper method to build the summary section of the backpack report.
     *
     * Includes the overall grade, total calories packed,
     * and total water packed.
     *
     * @param grade The overall grade for the backpack.
     * @param totalCalories The total calories packed.
     * @param totalWater The total water packed.
     * @return A QString containing the summary section of the report.
     */
    QString buildSummaryReport(double grade, double totalCalories, double totalWater) const;

    /**
     * @brief Helper method to build the fail section of the backpack report.
     *
     * Includes feedback about which required resources
     * are missing from the backpack.
     *
     * @param totalCalories The total calories packed.
     * @param totalWater The total water packed.
     * @return A QString containing the fail section of the report.
     */
    QString buildFailReport(double totalCalories, double totalWater) const;

    /**
     * @brief Helper method to build the pass section of the backpack report.
     *
     * Includes feedback indicating that the player
     * packed enough resources for the level.
     *
     * @return A QString containing the pass section of the report.
     */
    QString buildPassReport() const;
};

#endif // LEVEL_H
