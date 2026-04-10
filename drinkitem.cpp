/**
 * @file drinkitem.cpp
 * @brief Implementation of the DrinkItem class.
 *
 * Implements DrinkItem construction and stat display
 * using values pulled from the DrinkRegistry.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/9/2026
 */

#include "drinkitem.h"

DrinkItem::DrinkItem(DrinkType drinkEnum)
    : drinkType(drinkEnum), hydrationMultiplier(0.0)
{
    const DrinkStats& drinkStats = DrinkRegistry.at(drinkEnum);

    nutrition = drinkStats.nutrition;
    weight = drinkStats.weight;
    description = drinkStats.description;
    hydrationMultiplier = drinkStats.hydrationMultiplier;
}

std::map<QString, double> DrinkItem::getStats() const
{
    std::map<QString, double> output;

    output["Alcohol"] = nutrition.alcohol;
    output["Calories"] = nutrition.calories;
    output["Caffeine(mg)"] = nutrition.caffeineMg;
    output["Hydration(oz)"] = nutrition.hydrationOz;
    output["Sugar(g)"] = nutrition.sugar;
    output["Weight(lbs)"] = weight;

    return output;
}

double DrinkItem::getEffectiveHydration() const
{
    return nutrition.hydrationOz * hydrationMultiplier;
}
