/**
 * @file gearitem.cpp
 * @brief Implementation of the GearItem class.
 */

#include "gearitem.h"
#include "itemregistry.h"

namespace {
QString gearEnumToString(GearType g) {
    switch(g) {
        case GearType::BEAR_SPRAY: return "BEAR_SPRAY";
        case GearType::COMPASS: return "COMPASS";
        case GearType::FIRESTARTER: return "FIRESTARTER";
        case GearType::FIRST_AID_KIT: return "FIRST_AID_KIT";
        case GearType::FLASHLIGHT: return "FLASHLIGHT";
        case GearType::HAT: return "HAT";
        case GearType::MATCHES: return "MATCHES";
        case GearType::MULTI_TOOL: return "MULTI_TOOL";
        case GearType::RAIN_JACKET: return "RAIN_JACKET";
        case GearType::SLEEPING_BAG: return "SLEEPING_BAG";
        case GearType::SUNSCREEN: return "SUNSCREEN";
        case GearType::TENT: return "TENT";
        case GearType::WATER_FILTER: return "WATER_FILTER";
        case GearType::WHISTLE: return "WHISTLE";
        case GearType::COTTON_SHIRT: return "COTTON_SHIRT";
        case GearType::WOOL_SHIRT: return "WOOL_SHIRT";
        case GearType::CAST_IRON_SKILLET: return "CAST_IRON_SKILLET";
        case GearType::TRASH_BAG: return "TRASH_BAG";
        case GearType::ZIPLOC_BAG: return "ZIPLOC_BAG";
        case GearType::TREKKING_POLES: return "TREKKING_POLES";
    }
    return "";
}

GearCategory stringToCategory(const QString& s) {
    if (s == "NAVIGATION") return GearCategory::NAVIGATION;
    if (s == "ILLUMINATION") return GearCategory::ILLUMINATION;
    if (s == "SUN_PROTECTION") return GearCategory::SUN_PROTECTION;
    if (s == "FIRST_AID") return GearCategory::FIRST_AID;
    if (s == "KNIFE_TOOL") return GearCategory::KNIFE_TOOL;
    if (s == "FIRE") return GearCategory::FIRE;
    if (s == "SHELTER") return GearCategory::SHELTER;
    if (s == "INSULATION") return GearCategory::INSULATION;
    if (s == "SAFETY") return GearCategory::SAFETY;
    return GearCategory::OTHER;
}
}

GearItem::GearItem(GearType gearEnum)
    : GearItem(gearEnumToString(gearEnum))
{
}

GearItem::GearItem(const QString& name)
{
    ItemStats stats = ItemRegistry::getGearStats(name);
    weight = stats.weight;
    volumeLiters = stats.volume;
    description = stats.description;
    category = stringToCategory(stats.category);
    wasteStorage = stats.isWasteStorage;
    isCotton = stats.isCotton;
    isPole = stats.isPole;

    // These were not in the JSON but are present in GearStats struct in some versions.
    // Based on the original gearitem.h, they were used for meeting requirements.
    // I'll default them to ALL for now unless I add them to JSON.
    adventureType = AdventureType::ALL;
    terrainType = TerrainType::ALL;
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
