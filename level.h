/**
 * @file level.h
 * @brief Header file for the Level class.
 *
 * Defines the Level class, which represents one backpacking challenge.
 * A level stores the requirements for a trip, grades the player's
 * backpack, determines whether the player passed, and generates
 * feedback based on the packed items.
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

/**
 * @brief Represents a single backpacking level.
 *
 * A Level defines the minimum resources needed for a trip
 * and evaluates a backpack by assigning a preparedness score
 * based on its contents. It can also determine whether the
 * backpack passes the level and generate feedback.
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
     * @param calories - The minimum calories required for the trip.
     * @param hydration - The minimum hydration required for the trip, in ounces.
     * @param duration - The duration of the trip in hours.
     */
    Level(int calories, double hydration, int duration);

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
     * @param backpack - The collection of packed items.
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
     * @param backpack - The collection of packed items being evaluated.
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
     * @param backpack - The collection of packed items being evaluated.
     * @return A QString containing a report describing the backpack's preparedness.
     */
    virtual QString getReport(const std::vector<Item*>& backpack) const;

private:
    /**
     * @brief Evaluates the backpack and computes total stats.
     *
     * @param backpack The items being evaluated.
     * @return The aggregated LevelResult.
     */
    LevelResult evaluateBackpack(const std::vector<Item*>& backpack) const;

    // ---------- Helper methods ----------
    // --- Data collection ---

    /**
     * @brief Helper method that calculates the total calories in the backpack.
     *
     * @param backpack - The collection of packed items being evaluated.
     * @return The total calories packed.
     */
    double calculateTotalCalories(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the total caffeine in the backpack.
     *
     * @param backpack - The collection of packed items being evaluated.
     * @return The total caffeine packed.
     */
    double calculateTotalCaffeine(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the total alcohol in the backpack.
     *
     * @param backpack - The collection of packed items being evaluated.
     * @return The total alcohol packed.
     */
    double calculateTotalAlcohol(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the effective hydration in the backpack.
     *
     * Effective hydration is computed using DrinkItem hydration multipliers
     * and reduced by caffeine and alcohol penalties.
     *
     * @param backpack - The collection of packed items being evaluated.
     * @return The effective hydration packed in ounces.
     */
    double calculateEffectiveHydration(const std::vector<Item*>& backpack) const;

    // --- Scoring ---

    /**
     * @brief Helper method that calculates the hydration portion of the grade.
     *
     * Computes the hydration contribution to the overall score.
     *
     * @param effectiveHydration - The effective hydration packed.
     * @return The hydration contribution to the final score.
     */
    double calculateHydrationScore(double effectiveHydration) const;

    /**
     * @brief Helper method that calculates the total sugar in the backpack.
     *
     * @param backpack - The collection of packed items being evaluated.
     * @return The total sugar packed in grams.
     */
    double calculateTotalSugar(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the total weight in the backpack.
     *
     * @param backpack - The collection of packed items being evaluated.
     * @return The total weight packed in pounds.
     */
    double calculateTotalWeight(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the energy portion of the grade.
     *
     * @param totalCalories - The total calories packed.
     * @param totalCaffeine - The total caffeine contributing to energy.
     * @param totalSugar - The total sugar contributing to energy.
     * @param totalWeight - The total backpack weight in pounds.
     * @return The energy contribution to the final score.
     */
    double calculateEnergyScore(double totalCalories, double totalCaffeine,
                                double totalSugar, double totalWeight) const;

    // ---------- Member variables ----------

    /**
     * @brief The number of calories required to complete the trip.
     *
     * Used when grading the backpack to determine whether
     * the player has packed enough food.
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
     * @brief The minimum grade required to pass the level.
     *
     * A backpack must achieve at least this score to pass.
     */
    double passingScore;
};

#endif // LEVEL_H
