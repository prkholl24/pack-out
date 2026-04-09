/**
 * @file fooditem.cpp
 * @brief Implementation of the FoodItem class
 *
 * Implements FoodItem, which represents food that the player
 * can pack in their backpack. Each food item pulls its values
 * from the FoodRegistry and provides stats like calories and
 * weight for use in grading and gameplay.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 */

#include "fooditem.h"

FoodItem::FoodItem(Food foodEnum)
{
    // Looks up the foodEnum from the table
    const FoodStats& foodStruct = FoodRegistry.at(foodEnum);

    this->sustenance  = foodStruct.sustenance;
    this->calories    = foodStruct.calories;
    this->weight      = foodStruct.weight;
    this->description = &foodStruct.description;
}

std::map<QString, double> FoodItem::getStats() const {
    std::map<QString, double> output;

    output["Calories"] = this->calories;
    output["Weight(lbs)"] = this->weight;

    return output;
}
