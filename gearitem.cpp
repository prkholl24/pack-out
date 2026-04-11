/**
 * @file gearitem.cpp
 * @brief Implementation of the GearItem class.
 *
 * Implements GearItem construction and stat display
 * using values pulled from the GearRegistry.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/9/2026
 */

#include "gearitem.h"

GearItem::GearItem(GearType gearEnum)
{
    const GearStats& gearStats = GearRegistry.at(gearEnum);

    weight = gearStats.weight;
    description = gearStats.description;
    adventureType = gearStats.adventure;
    terrainType = gearStats.terrain;
}

TerrainType GearItem::getTerrainType() const
{
    return terrainType;
}

AdventureType GearItem::getAdventureType() const
{
    return adventureType;
}
