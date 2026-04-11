/**
 * @file drinkitem.cpp
 * @brief Implementation of the DrinkItem class.
 */

#include "drinkitem.h"

DrinkItem::DrinkItem(DrinkType drinkEnum)
    : hydrationMultiplier(0.0)
{
    const DrinkStats& drinkStats = DrinkRegistry.at(drinkEnum);

    nutrition = drinkStats.nutrition;
    weight = drinkStats.weight;
    volumeLiters = drinkStats.volume;
    producesWaste = drinkStats.producesWaste;
    description = drinkStats.description;
    hydrationMultiplier = drinkStats.hydrationMultiplier;
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
