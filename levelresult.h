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
 * @brief Codes for educational science lessons.
 */
enum class ScienceLesson {
    NONE,
    BMR_BASE_METABOLISM,
    PANDOLF_WEIGHT_PENALTY,
    HYDRATION_DIURETICS,
    PACK_WEIGHT_RATIO,
    COTTON_CLOTHING_WARNING,
    FIBER_ENERGY_STABILITY,
    SODIUM_HYDRATION_LOAD,
    WHOLE_FOOD_FUEL
};

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

    // Energy expenditure breakdown (Calculated by the Model)
    double bmrCost = 0.0;            // Calories burned by just existing during the hike
    double horizontalBurn = 0.0;    // Calories burned walking the distance
    double verticalBurn = 0.0;      // Calories burned climbing the elevation
    double packWeightPenalty = 0.0;  // Extra calories burned carrying the load

    double totalCaffeine = 0.0;      // Total caffeine affecting hydration
    double totalSugar = 0.0;         // Total sugar affecting energy quality
    double totalAlcohol = 0.0;       // Total alcohol reducing hydration

    double totalWeight = 0.0;        // Total backpack weight
    double idealWeight = 0.0;        // Recommended weight for duration
    
    // Status flags (Calculated by the Model, used by the View/Reporter)
    bool hasMissingGear = false;
    bool isTooHeavy = false;
    bool isHighCaffeine = false;
    bool isHighSugar = false;
    bool isDehydrated = false;
    bool isStarving = false;
    bool isDehydratedByAlcohol = false;

    /**
     * @brief The specific science lesson triggered by this trip.
     */
    ScienceLesson lessonCode = ScienceLesson::NONE;

    int requiredCalories = 0;        // Minimum calories needed for the level
    double requiredHydration = 0.0;  // Minimum effective hydration required
};

#endif // LEVELRESULT_H
