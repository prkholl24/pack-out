/**
 * @file drinkitem.cpp
 * @brief Implementation of the DrinkItem class.
 */

#include "drinkitem.h"
#include "itemregistry.h"

namespace {
QString drinkEnumToString(DrinkType d) {
    switch(d) {
        case DrinkType::BEER: return "BEER";
        case DrinkType::ENERGY_DRINK: return "ENERGY_DRINK";
        case DrinkType::SODA: return "SODA";
        case DrinkType::SPORTS_DRINK: return "SPORTS_DRINK";
        case DrinkType::WATER_BOTTLE: return "WATER_BOTTLE";
        case DrinkType::WATER_CAMELBAK: return "WATER_CAMELBAK";
    }
    return "";
}
}

DrinkItem::DrinkItem(DrinkType drinkEnum)
    : DrinkItem(drinkEnumToString(drinkEnum))
{
}

DrinkItem::DrinkItem(const QString& name)
{
    ItemStats stats = ItemRegistry::getDrinkStats(name);
    nutrition = stats.nutrition;
    weight = stats.weight;
    volumeLiters = stats.volume;
    producesWaste = stats.producesWaste;
    description = stats.description;
    hydrationMultiplier = stats.hydrationMultiplier;
}

std::map<QString, double> DrinkItem::getStatMap() const
{
    // Return programmatic IDs, not UI labels
    std::map<QString, double> m = {
        { "hydration", nutrition.hydrationOz },
        { "calories", nutrition.calories }
    };
    if (nutrition.caffeineMg > 0) m["caffeine"] = nutrition.caffeineMg;
    if (nutrition.alcohol > 0) m["alcohol"] = nutrition.alcohol;
    return m;
}

double DrinkItem::getEffectiveHydration() const
{
    return nutrition.hydrationOz * hydrationMultiplier;
}
