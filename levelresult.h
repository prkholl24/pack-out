/**
 * @file levelresult.h
 * @brief Container for the results of a backpack evaluation.
 */

#ifndef LEVELRESULT_H
#define LEVELRESULT_H

#include <vector>
#include "nutritionprofile.h"

/**
 * @brief ScienceLesson codes used to trigger specific educational feedback.
 */
enum class ScienceLesson {
    NONE,
    BMR_BASE_METABOLISM,
    PANDOLF_WEIGHT_PENALTY,
    HYDRATION_BASICS,
    HYDRATION_DIURETICS,
    PACK_WEIGHT_RATIO,
    COTTON_CLOTHING_WARNING,
    FIBER_ENERGY_STABILITY,
    SODIUM_HYDRATION_LOAD,
    WHOLE_FOOD_FUEL,
    LEAVE_NO_TRACE_VIOLATION,
    VOLUME_OVERFLOW,
    TEN_ESSENTIALS_MISSING
};

/**
 * @brief Specific physical or safety conditions detected by the Model.
 */
enum class TripCondition {
    STARVING,
    DEHYDRATED,
    TOO_HEAVY,
    TOO_BULKY,
    HIGH_CAFFEINE,
    HIGH_SUGAR,
    ALCOHOL_PENALTY,
    MISSING_GEAR,
    MISSING_ESSENTIALS,
    LNT_VIOLATION
};

/**
 * @brief Combined data totals from a single pass through the backpack.
 */
struct BackpackTotals {
    double calories = 0.0;
    double weightLbs = 0.0;
    double volume = 0.0;
    double hydrationOz = 0.0;
    double effectiveHydration = 0.0; 
    double sugar = 0.0;
    double fiber = 0.0;
    double sodium = 0.0;
    double caffeine = 0.0;
    double alcohol = 0.0;
    int wasteCount = 0;
    
    bool hasWasteStorage = false;
    bool hasCotton = false;
    bool hasPoles = false;
};

/**
 * @brief Stores the full results of evaluating a backpack against trip requirements.
 */
struct LevelResult
{
    double grade = 0.0;              // Overall score (0-100)
    bool passed = false;             // Did the player meet the passing score?

    // Model Data State
    BackpackTotals totals;           // Raw sums from the backpack
    double maxVolume = 0.0;          // Backpack capacity limit
    double idealWeight = 0.0;        // Target weight for this trip

    // Energy Expenditure Breakdown (Math results)
    double bmrBurn = 0.0;            
    double distanceBurn = 0.0;       
    double climbBurn = 0.0;          
    double loadPenalty = 0.0;        

    /**
     * @brief List of specific conditions identified during evaluation.
     */
    std::vector<TripCondition> conditions;

    // High-level Logic Flags
    bool hasMissingGear = false;
    bool missingEssentials = false;
    bool hasCottonWarning = false;
    bool leftTrashBehind = false;

    bool hydrationOnly = false;
    ScienceLesson lessonCode = ScienceLesson::NONE;

    int requiredCalories = 0;
    double requiredHydration = 0.0;
};

#endif // LEVELRESULT_H
