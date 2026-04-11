/**
 * @file gearitem.cpp
 * @brief Implementation of the GearItem class.
 */

#include "gearitem.h"

GearItem::GearItem(GearType gearEnum)
{
    const GearStats& gearStats = GearRegistry.at(gearEnum);

    weight = gearStats.weight;
    volumeLiters = gearStats.volume;
    description = gearStats.description;
    adventureType = gearStats.adventure;
    terrainType = gearStats.terrain;
    category = gearStats.category;
    wasteStorage = gearStats.isWasteStorage;
    isCotton = gearStats.isCotton;
    isPole = gearStats.isPole;
}

std::map<QString, double> GearItem::getStatMap() const
{
    return {}; // Gear currently has no specific numeric stats
}

GearCategory GearItem::getCategory() const
{
    return category;
}

TerrainType GearItem::getTerrainType() const
{
    return terrainType;
}

AdventureType GearItem::getAdventureType() const
{
    return adventureType;
}
