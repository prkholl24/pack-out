/**
 * @file trekphysiology.cpp
 * @brief Implementation of the physiological logic.
 */

#include "trekphysiology.h"
#include "trekrules.h"
#include <algorithm>

void TrekPhysiology::computeRequirements(LevelResult& result, 
                                         const TripRequirements& req, 
                                         const Player& player,
                                         bool hasPoles)
{
    // 1. Fetch Game Rules and Environmental Modifiers (Pure MVC: Modifiers are data)
    double hoursInDay = TrekRules::get("physiology", "hours_in_day");
    RuleModifier terrain = TrekRules::getTerrainModifier(req.terrain);
    RuleModifier weather = TrekRules::getWeatherModifier(req.weather);

    // 2. Basal Burn: Stay-alive energy
    result.bmrBurn = (player.calculateDailyBMR() / hoursInDay) * req.durationHours;

    // 3. Distance Burn: Horizontal movement
    double calPerMile = TrekRules::get("physiology", "calories_per_mile_lb");
    result.distanceBurn = player.getWeightLbs() * req.miles * calPerMile * terrain.energy * weather.energy;
    
    // 4. Climb Burn: Vertical movement
    double calPerVertical = TrekRules::get("physiology", "calories_per_vertical_lb");
    double efficiency = hasPoles ? TrekRules::get("physiology", "trekking_pole_bonus") : 1.0;
    result.climbBurn = player.getWeightLbs() * req.elevationGain * calPerVertical * efficiency;

    // 5. Load Penalty: Carrying the actual pack weight
    double elevationStep = TrekRules::get("physiology", "elevation_gain_step");
    double scaler = TrekRules::get("physiology", "pack_weight_scaler");
    double intensity = req.miles + (req.elevationGain / elevationStep);
    result.loadPenalty = result.totals.weightLbs * intensity * scaler;

    // Final Requirement Sum
    double totalCals = result.bmrBurn + result.distanceBurn + result.climbBurn + result.loadPenalty;
    result.requiredCalories = (req.customCalories > 0) ? req.customCalories : static_cast<int>(totalCals);
    
    // 6. Hydration Calculation
    double baseRate = TrekRules::get("physiology", "hydration_oz_per_hour");
    double surge = TrekRules::get("physiology", "steep_climb_hydration_surge");
    double threshold = TrekRules::get("physiology", "steep_climb_threshold_feet");

    double hourlyWaterLoss = baseRate * terrain.water * weather.water;
    if (req.elevationGain > threshold) hourlyWaterLoss += surge;
    
    result.requiredHydration = (req.customHydration > 0) ? req.customHydration : (req.durationHours * hourlyWaterLoss);
}
