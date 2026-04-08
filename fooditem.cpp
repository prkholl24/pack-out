/**
 * @file fooditem.cpp
 * @brief Implementation of the FoodItem class
 */

#include "fooditem.h"

FoodItem::FoodItem(Food foodEnum) {
    // Looks up the foodEnum from the table
    const FoodStats& foodStruct = FoodRegistry.at(foodEnum);

    this->sustenance  = foodStruct.sustenance;
    this->calories    = foodStruct.calories;
    this->weight      = foodStruct.weight;
    this->description = &foodStruct.description;
}

std::map<QString, double> FoodItem::getStats() {
    std::map<QString, double> output;

    output["Calories"] = this->calories;
    output["Weight(lbs)"] = this->weight;

    return output;
}
