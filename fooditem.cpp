/**
 * @file fooditem.cpp
 * @brief Implementation of the FoodItem class.
 */

#include "fooditem.h"

FoodItem::FoodItem(Food foodEnum)
{
    const FoodStats& foodStats = FoodRegistry.at(foodEnum);

    nutrition = foodStats.nutrition;
    weight = foodStats.weight;
    volumeLiters = foodStats.volume;
    producesWaste = foodStats.producesWaste;
    description = foodStats.description;
}

std::map<QString, double> FoodItem::getStatMap() const
{
    // Return programmatic IDs, not UI labels
    return {
        { "calories", nutrition.calories },
        { "carbs", nutrition.carbs },
        { "protein", nutrition.protein },
        { "fat", nutrition.fat },
        { "fiber", nutrition.fiber }
    };
}
