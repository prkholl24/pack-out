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

    // ---------- Helper methods ----------
    // --- Data collection ---

    /**
     * @brief Helper method that calculates the total of a specific stat in the backpack.
     *
     * Iterates through all packed items and sums the value
     * associated with the requested stat name.
     *
     * @param backpack - The collection of packed items being evaluated.
     * @param statName - The stat to total.
     * @return The total value for that stat.
     */
    double calculateTotalStat(const std::vector<Item*>& backpack, const QString& statName) const;

    /**
     * @brief Helper method that calculates the total calories in the backpack.
     *
     * @param backpack - The collection of packed items being evaluated.
     * @return The total calories packed.
     */
    double calculateTotalCalories(const std::vector<Item*>& backpack) const;

    /**
     * @brief Helper method that calculates the effective hydration in the backpack.
     *
     * Effective hydration is based on the drink types packed.
     * Water counts fully, sports drinks count partially,
     * and soda counts weakly.
     *
     * @param backpack - The collection of packed items being evaluated.
     * @return The effective hydration packed in ounces.
     */
    double calculateEffectiveHydration(const std::vector<Item*>& backpack) const;

    // --- Scoring ---

    /**
     * @brief Helper method that calculates the energy portion of the grade.
     *
     * Computes the energy contribution to the overall score based on
     * total calories, with an additional boost from caffeine.
     *
     * @param totalCalories The total calories packed.
     * @param totalCaffeine The total caffeine contributing to energy.
     * @return The energy contribution to the final score.
     */
    double calculateEnergyScore(double totalCalories, double totalCaffeine) const;

    /**
     * @brief Helper method that calculates the hydration portion of the grade.
     *
     * Computes the hydration contribution to the overall score.
     *
     * @param effectiveHydration - The effective hydration packed.
     * @return The hydration contribution to the final score.
     */
    double calculateHydrationScore(double effectiveHydration) const;

    // --- Evaluation ---

    /**
     * @brief Evaluates the contents of a backpack for this level.
     *
     * Calculates total calories and effective hydration from the items
     * in the backpack, then computes the overall grade and determines
     * whether the level requirements are met.
     *
     * The results are returned in a LevelResult so they can be used
     * to build a report elsewhere without exposing the grading logic.
     *
     * @param backpack The collection of packed items to evaluate.
     * @return A LevelResult containing totals, grade, and pass/fail status.
     */
    LevelResult evaluateBackpack(const std::vector<Item*>& backpack) const;

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
