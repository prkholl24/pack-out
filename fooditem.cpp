/**
 * @file fooditem.cpp
 * @brief Implementation of the FoodItem class.
 */

#include "fooditem.h"
#include "itemregistry.h"

namespace {
QString foodEnumToString(Food f) {
    switch(f) {
        case Food::BANANA: return "BANANA";
        case Food::BEEF_JERKY: return "BEEF_JERKY";
        case Food::GRANOLA_BAR: return "GRANOLA_BAR";
        case Food::MIXED_NUTS: return "MIXED_NUTS";
        case Food::PEANUT_BUTTER_SANDWICH: return "PEANUT_BUTTER_SANDWICH";
        case Food::POTATO: return "POTATO";
        case Food::TRAIL_MIX: return "TRAIL_MIX";
    }
    return "";
}
}

FoodItem::FoodItem(Food foodEnum)
    : FoodItem(foodEnumToString(foodEnum))
{
}

FoodItem::FoodItem(const QString& name)
{
    ItemStats stats = ItemRegistry::getFoodStats(name);
    nutrition = stats.nutrition;
    weight = stats.weight;
    volumeLiters = stats.volume;
    producesWaste = stats.producesWaste;
    description = stats.description;
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
