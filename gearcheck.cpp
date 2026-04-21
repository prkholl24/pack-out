/**
 * @file gearcheck.cpp
 * @brief Implementation of the GearCheck validation logic.
 */

#include "gearcheck.h"
#include "gearitem.h"
#include "trekrules.h"
#include <set>
#include <utility>

namespace {
void addMissingGearCondition(LevelResult& result)
{
    result.hasMissingGear = true;
    for (TripCondition c : result.conditions) {
        if (c == TripCondition::MISSING_GEAR) {
            return;
        }
    }
    result.conditions.push_back(TripCondition::MISSING_GEAR);
}
}

void GearCheck::validate(const std::vector<std::unique_ptr<Item>>& backpack,
                         const TripRequirements& req,
                         LevelResult& result)
{
    if (req.skipGearCheck) return;

    if (!req.requiredGearItems.isEmpty()) {
        if (!hasRequiredGearItems(backpack, req.requiredGearItems)) {
            addMissingGearCondition(result);
        }
    } else {
        if (!hasTerrainGear(backpack, req)) {
            addMissingGearCondition(result);
        }

        if (!hasWeatherGear(backpack, req.weather)) {
            addMissingGearCondition(result);
        }
    }

    if (!hasTenEssentials(backpack)) {
        result.missingEssentials = true;
        result.conditions.push_back(TripCondition::MISSING_ESSENTIALS);
    }
}

bool GearCheck::hasRequiredGearItems(const std::vector<std::unique_ptr<Item>>& backpack,
                                     const QStringList& requiredItems)
{
    std::set<QString> packedGear;

    for (const auto& item : backpack) {
        const GearItem* gear = dynamic_cast<const GearItem*>(item.get());
        if (!gear) continue;
        packedGear.insert(gear->getItemName());
    }

    for (const QString& required : requiredItems) {
        if (packedGear.count(required) == 0) {
            return false;
        }
    }

    return true;
}

bool GearCheck::hasTerrainGear(const std::vector<std::unique_ptr<Item>>& backpack, const TripRequirements& req)
{
    bool terrainFound = (req.terrain == TerrainType::ALL);
    bool adventureFound = (req.adventure == AdventureType::ALL);

    for (const auto& item : backpack) {
        const GearItem* gear = dynamic_cast<const GearItem*>(item.get());
        if (!gear) continue;
        if (!terrainFound && gear->matchesTerrain(req.terrain)) {
            terrainFound = true;
        }
        if (!adventureFound && gear->matchesAdventure(req.adventure)) {
            adventureFound = true;
        }
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

        if (gear->matchesWeatherKeywords(keywords)) {
            return true;
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
