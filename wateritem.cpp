/**
 * @file wateritem.cpp
 * @brief Implementation of the WaterItem class.
 *
 * Implements WaterItem, which represents water that the player
 * can pack in their backpack. Each water item pulls its values
 * from the WaterRegistry and calculates total weight based on
 * container weight and water capacity.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 */
#include "wateritem.h"

WaterItem::WaterItem(Water waterEnum)
{
    const WaterStats& waterStruct = WaterRegistry.at(waterEnum);

    this->capacity    = waterStruct.capacity;
    this->weight = waterStruct.containerWeight + calculateWaterWeight(this->capacity);
    this->description = &waterStruct.description;
}

double WaterItem::calculateWaterWeight(double ounces) const
{
    return ounces * (8.34 / 128.0);
}

std::map<QString, double> WaterItem::getStats() const
{
    std::map<QString, double> output;

    output["Water(oz)"] = this->capacity;
    output["Weight(lbs)"] = this->weight;

    return output;
}
