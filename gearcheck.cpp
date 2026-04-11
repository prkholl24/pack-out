/**
 * @file gearcheck.cpp
 * @brief Implementation of the GearCheck validation logic.
 */

#include "gearcheck.h"
#include "gearitem.h"
#include "trekrules.h"
#include <set>
#include <utility>

void GearCheck::validate(const std::vector<std::unique_ptr<Item>>& backpack,
                         const TripRequirements& req,
                         LevelResult& result)
{
    if (!hasTerrainGear(backpack, req)) {
        result.hasMissingGear = true;
        result.conditions.push_back(TripCondition::MISSING_GEAR);
    }

    if (!hasWeatherGear(backpack, req.weather)) {
        result.hasMissingGear = true;
        // Don't double-add the condition if already there
        bool alreadyTagged = false;
        for (TripCondition c : result.conditions) if (c == TripCondition::MISSING_GEAR) alreadyTagged = true;
        
        if (!alreadyTagged) {
            result.conditions.push_back(TripCondition::MISSING_GEAR);
        }
    }

    if (!hasTenEssentials(backpack)) {
        result.missingEssentials = true;
        result.conditions.push_back(TripCondition::MISSING_ESSENTIALS);
    }
}

bool GearCheck::hasTerrainGear(const std::vector<std::unique_ptr<Item>>& backpack, const TripRequirements& req)
{
    bool terrainFound = (req.terrain == TerrainType::ALL);
    bool adventureFound = (req.adventure == AdventureType::ALL);

    for (const auto& item : backpack) {
        const GearItem* gear = dynamic_cast<const GearItem*>(item.get());
        if (!gear) continue;
        if (gear->getTerrainType() == req.terrain) terrainFound = true;
        if (gear->getAdventureType() == req.adventure) adventureFound = true;
    }
    return terrainFound && adventureFound;
}

bool GearCheck::hasWeatherGear(const std::vector<std::unique_ptr<Item>>& backpack, WeatherType weather)
{
    if (weather == WeatherType::DEFAULT) return true;
    
    const QStringList keywords = TrekRules::getWeatherKeywords(weather);
    if (keywords.isEmpty()) return true;

    for (const auto& item : backpack) {
        const GearItem* gear = dynamic_cast<const GearItem*>(item.get());
        if (!gear) continue;
        
        QString desc = gear->getDescription().toLower();
        for (const QString& keyword : std::as_const(keywords)) {
            if (desc.contains(keyword)) return true;
        }
    }
    return false;
}

bool GearCheck::hasTenEssentials(const std::vector<std::unique_ptr<Item>>& backpack)
{
    std::set<GearCategory> categories;
    for (const auto& item : backpack) {
        const GearItem* gear = dynamic_cast<const GearItem*>(item.get());
        if (!gear) continue;
        GearCategory cat = gear->getCategory();
        if (cat != GearCategory::SAFETY && cat != GearCategory::OTHER) {
            categories.insert(cat);
        }
    }
    return categories.size() >= 8;
}
