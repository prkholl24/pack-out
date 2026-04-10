/**
 * @file levelresult.h
 * @brief Contains the LevelResult struct used to store evaluation results.
 *
 * Defines LevelResult, which stores the results of evaluating a player's
 * backpack for a level. This includes totals for energy and hydration,
 * as well as related factors like caffeine, sugar, and alcohol, along
 * with grading information and pass/fail status.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/8/2026
 */

#ifndef LEVELRESULT_H
#define LEVELRESULT_H

/**
 * @brief Stores the results of evaluating a backpack for a level.
 *
 * This struct holds all data needed to generate a report,
 * separating evaluation logic from presentation logic.
 */
struct LevelResult
{
    double grade = 0.0;              // Final score out of 100
    bool passed = false;             // Whether the level was passed

    double totalCalories = 0.0;      // Total calories packed
    double effectiveHydration = 0.0; // Hydration after penalties (caffeine, alcohol, etc.)

    double totalCaffeine = 0.0;      // Total caffeine affecting hydration
    double totalSugar = 0.0;         // Total sugar affecting energy quality
    double totalAlcohol = 0.0;       // Total alcohol reducing hydration

    double totalWeight = 0.0;        // Total backpack weight
    double idealWeight = 0.0;        // Recommended weight for duration
    bool hasMissingGear = false;     // Whether essential gear was missing

    int requiredCalories = 0;        // Minimum calories needed for the level
    double requiredHydration = 0.0;  // Minimum effective hydration required
};

#endif // LEVELRESULT_H
