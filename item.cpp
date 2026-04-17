/**
 * @file item.cpp
 * @brief Implementation of the Item base class.
 *
 * Implements shared functionality used by all packable items,
 * including weight access, nutrition access, tooltip generation,
 * and default stat reporting.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/9/2026
 */

#include "item.h"

Item::Item()
{
    weight = 0.0;
    volumeLiters = 0.0;
    producesWaste = false;
    isCotton = false;
    description = "";
    nutrition = NutritionProfile{};
}

double Item::getWeight() const
{
    return weight;
}

double Item::getVolume() const
{
    return volumeLiters;
}

bool Item::generatesWaste() const
{
    return producesWaste;
}

bool Item::isCottonMaterial() const
{
    return isCotton;
}

QString Item::getDescription() const
{
    return description;
}

const NutritionProfile& Item::getNutrition() const
{
    return nutrition;
}
