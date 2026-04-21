/**
 * @file gearcheck.h
 * @brief Safety logic responsible for validating gear requirements.
 */

#ifndef GEARCHECK_H
#define GEARCHECK_H

#include <vector>
#include <memory>
#include <map>
#include "item.h"
#include "levelresult.h"
#include "tripconditions.h"

/**
 * @brief Specialized logic that validates backpack safety and completeness.
 * 
 * Checks for mandatory terrain gear, weather gear, and the 10 essentials standard.
 */
class GearCheck
{
public:
    /**
     * @brief Performs a full safety check on the backpack.
     * @param backpack - The items to check.
     * @param req - The trip requirements.
     * @param result - The result object to update with tags.
     */
    static void validate(const std::vector<std::unique_ptr<Item>>& backpack,
                         const TripRequirements& req,
                         LevelResult& result);

private:
    /**
     * @brief Checks for exact required gear item names.
     */
    static bool hasRequiredGearItems(const std::vector<std::unique_ptr<Item>>& backpack,
                                     const QStringList& requiredItems);

    /**
     * @brief Checks if gear matching the terrain/adventure is present.
     */
    static bool hasTerrainGear(const std::vector<std::unique_ptr<Item>>& backpack, const TripRequirements& req);
    
    /**
     * @brief Checks for weather-appropriate gear based on keywords.
     */
    static bool hasWeatherGear(const std::vector<std::unique_ptr<Item>>& backpack, WeatherType weather);
    
    /**
     * @brief Validates the presence of at least 8/10 safety categories.
     */
    static bool hasTenEssentials(const std::vector<std::unique_ptr<Item>>& backpack);

    static const std::map<WeatherType, std::vector<QString>> WeatherSafetyKeywords;
};

#endif // GEARCHECK_H
