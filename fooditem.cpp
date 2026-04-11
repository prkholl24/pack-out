/**
 * @file fooditem.cpp
 * @brief Implementation of the FoodItem class.
 *
 * Implements FoodItem construction and stat display
 * using values pulled from the FoodRegistry.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/9/2026
 */

#include "fooditem.h"

FoodItem::FoodItem(Food foodEnum)
{
    const FoodStats& foodStats = FoodRegistry.at(foodEnum);

    nutrition = foodStats.nutrition;
    weight = foodStats.weight;
    description = foodStats.description;
}
