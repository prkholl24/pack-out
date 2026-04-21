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
        case GearType::POCKET_KNIFE: return "POCKET_KNIFE";
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

TerrainType stringToTerrainType(const QString& s) {
    if (s == "DESERT") return TerrainType::DESERT;
    if (s == "FOREST") return TerrainType::FOREST;
    if (s == "MOUNTAIN") return TerrainType::MOUNTAIN;
    if (s == "ROCKY") return TerrainType::ROCKY;
    return TerrainType::ALL;
}

AdventureType stringToAdventureType(const QString& s) {
    if (s == "BACKPACKING") return AdventureType::BACKPACKING;
    if (s == "CAMPING") return AdventureType::CAMPING;
    if (s == "DAY_HIKE") return AdventureType::DAY_HIKE;
    return AdventureType::ALL;
}

bool isOvernightAdventure(AdventureType type) {
    return type == AdventureType::BACKPACKING || type == AdventureType::CAMPING;
}
}

GearItem::GearItem(GearType gearEnum)
    : GearItem(gearEnumToString(gearEnum))
{
}

GearItem::GearItem(const QString& name)
{
    ItemStats stats = ItemRegistry::getGearStats(name);
    itemName = name;
    weight = stats.weight;
    volumeLiters = stats.volume;
    description = stats.description;
    category = stringToCategory(stats.category);
    wasteStorage = stats.isWasteStorage;
    isCotton = stats.isCotton;
    isPole = stats.isPole;
    terrainType = stringToTerrainType(stats.terrain);
    adventureType = stringToAdventureType(stats.adventure);
    weatherKeywords = stats.weatherKeywords;
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

QString GearItem::getItemName() const
{
    return itemName;
}

bool GearItem::matchesTerrain(TerrainType requiredTerrain) const
{
    if (requiredTerrain == TerrainType::ALL) {
        return true;
    }
    return terrainType == requiredTerrain;
}

bool GearItem::matchesAdventure(AdventureType requiredAdventure) const
{
    if (requiredAdventure == AdventureType::ALL) {
        return true;
    }
    if (adventureType == requiredAdventure) {
        return true;
    }
    return isOvernightAdventure(requiredAdventure) && isOvernightAdventure(adventureType);
}

bool GearItem::matchesWeatherKeywords(const QStringList& keywords) const
{
    QString descriptionText = description.toLower();
    for (const QString& keyword : weatherKeywords) {
        if (keywords.contains(keyword, Qt::CaseInsensitive)) {
            return true;
        }
    }

    for (const QString& keyword : keywords) {
        if (descriptionText.contains(keyword.toLower())) {
            return true;
        }
    }
    return false;
}
