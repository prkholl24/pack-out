/**
 * @file trekphysiology.h
 * @brief Pure math module for backpacking physiological calculations.
 */

#ifndef TREKPHYSIOLOGY_H
#define TREKPHYSIOLOGY_H

#include "tripconditions.h"
#include "player.h"
#include "levelresult.h"

/**
 * @brief Stat-less utility class that handles the "Bio-Math" of a trek.
 * 
 * Responsible for computing calorie and hydration needs based on 
 * distance, elevation, terrain, weather, and pack load.
 */
class TrekPhysiology
{
public:
    /**
     * @brief Computes the calorie and hydration needs for a specific player on a specific trip.
     * @param result - The LevelResult to update with burn and need data.
     * @param req - The requirements/constraints of the level.
     * @param player - The player's physical profile.
     * @param hasPoles - Whether the player packed trekking poles.
     */
    static void computeRequirements(LevelResult& result, 
                                    const TripRequirements& req, 
                                    const Player& player,
                                    bool hasPoles);

private:
    static constexpr double CALORIES_PER_MILE_PER_LB = 0.035;
    static constexpr double CALORIES_PER_VERTICAL_FOOT_LB = 0.005;
    static constexpr double HYDRATION_OZ_PER_HOUR = 8.0;
    static constexpr double PACK_WEIGHT_SCALER = 0.15;
    static constexpr double ELEVATION_GAIN_STEP = 100.0;
    static constexpr double TREKKING_POLES_BONUS = 0.85;
};

#endif // TREKPHYSIOLOGY_H
