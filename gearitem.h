/**
 * @file gearitem.h
 * @brief Contains the GearItem class, GearType enum, and supporting data structures.
 *
 * Defines GearItem, which represents a piece of gear the player
 * can pack for a trip. Gear items contribute weight and can help
 * the player meet level requirements based on weather, terrain,
 * and adventure type.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/9/2026
 */

#ifndef GEARITEM_H
#define GEARITEM_H

#include <map>
#include "item.h"
#include "tripconditions.h"

/**
 * @brief Enumerates the available gear item types.
 */
enum class GearType {
    BEAR_SPRAY,
    COMPASS,
    FIRESTARTER,
    FIRST_AID_KIT,
    FLASHLIGHT,
    HAT,
    MATCHES,
    POCKET_KNIFE,
    RAIN_JACKET,
    SLEEPING_BAG,
    SUNSCREEN,
    TENT,
    WATER_FILTER,
    WHISTLE,
    COTTON_SHIRT,
    WOOL_SHIRT,
    CAST_IRON_SKILLET,
    TRASH_BAG,
    ZIPLOC_BAG,
    TREKKING_POLES
};

/**
 * @brief Categorizes gear for "The 10 Essentials" and other logic.
 */
enum class GearCategory {
    NAVIGATION,
    ILLUMINATION,
    SUN_PROTECTION,
    FIRST_AID,
    KNIFE_TOOL,
    FIRE,
    SHELTER,
    INSULATION,
    SAFETY,
    OTHER
};

/**
 * @brief Stores the data used to construct one GearItem.
 */
struct GearStats {
    double weight;
    double volume;
    QString description;
    AdventureType adventure;
    TerrainType terrain;
    GearCategory category;
    bool isWasteStorage = false;
    bool isCotton = false;
    bool isPole = false;
};

/**
 * @brief Registry of all predefined gear items.
 */
static const std::map<GearType, GearStats> GearRegistry = {
    { GearType::BEAR_SPRAY,  { 0.75, 0.5, "Essential for safety in bear country.", AdventureType::BACKPACKING, TerrainType::FOREST, GearCategory::SAFETY, false, false, false }},
    { GearType::COMPASS,     { 0.10, 0.1, "Analog navigation tool that works without batteries.", AdventureType::ALL, TerrainType::ALL, GearCategory::NAVIGATION, false, false, false }},
    { GearType::FIRESTARTER, { 0.15, 0.1, "Helps you get a fire going in damp conditions.", AdventureType::CAMPING, TerrainType::ALL, GearCategory::FIRE, false, false, false }},
    { GearType::FIRST_AID_KIT, { 1.50, 1.0, "Contains basic medical supplies for the trail.", AdventureType::ALL, TerrainType::ALL, GearCategory::FIRST_AID, false, false, false }},
    { GearType::FLASHLIGHT,  { 0.50, 0.3, "Provides light for night activities.", AdventureType::CAMPING, TerrainType::ALL, GearCategory::ILLUMINATION, false, false, false }},
    { GearType::HAT,         { 0.20, 0.5, "Protects your head from the sun and cold.", AdventureType::ALL, TerrainType::ALL, GearCategory::SUN_PROTECTION, false, false, false }},
    { GearType::MATCHES,     { 0.05, 0.1, "Simple, reliable way to start a fire.", AdventureType::CAMPING, TerrainType::ALL, GearCategory::FIRE, false, false, false }},
    { GearType::POCKET_KNIFE,{ 0.30, 0.1, "A versatile tool for various tasks.", AdventureType::ALL, TerrainType::ALL, GearCategory::KNIFE_TOOL, false, false, false }},
    { GearType::RAIN_JACKET, { 0.80, 1.5, "Keeps you dry during unexpected rain.", AdventureType::ALL, TerrainType::ALL, GearCategory::INSULATION, false, false, false }},
    { GearType::SLEEPING_BAG,{ 3.00, 10.0, "Provides warmth for overnight trips.", AdventureType::CAMPING, TerrainType::ALL, GearCategory::SHELTER, false, false, false }},
    { GearType::SUNSCREEN,   { 0.25, 0.2, "Protects your skin from UV rays. Essential even in snow.", AdventureType::ALL, TerrainType::ALL, GearCategory::SUN_PROTECTION, false, false, false }},
    { GearType::TENT,         { 5.00, 15.0, "A portable shelter for camping.", AdventureType::CAMPING, TerrainType::ALL, GearCategory::SHELTER, false, false, false }},
    { GearType::WATER_FILTER,{ 0.40, 0.5, "Makes stream water safe to drink.", AdventureType::BACKPACKING, TerrainType::ALL, GearCategory::SAFETY, false, false, false }},
    { GearType::WHISTLE,     { 0.05, 0.1, "A simple signaling device for emergencies.", AdventureType::ALL, TerrainType::ALL, GearCategory::SAFETY, false, false, false }},
    { GearType::COTTON_SHIRT, { 0.80, 1.0, "A basic t-shirt. Comfortable until it gets wet.", AdventureType::ALL, TerrainType::ALL, GearCategory::INSULATION, false, true, false }},
    { GearType::WOOL_SHIRT,   { 0.50, 1.0, "Moisture-wicking and stays warm when wet. Essential for safety.", AdventureType::ALL, TerrainType::ALL, GearCategory::INSULATION, false, false, false }},
    { GearType::CAST_IRON_SKILLET, { 8.00, 4.0, "Great for cooking, but very heavy for the trail.", AdventureType::CAMPING, TerrainType::ALL, GearCategory::OTHER, false, false, false }},
    { GearType::TRASH_BAG,   { 0.10, 0.1, "A heavy-duty bag for packing out all trash and waste.", AdventureType::ALL, TerrainType::ALL, GearCategory::OTHER, true, false, false }},
    { GearType::ZIPLOC_BAG,  { 0.01, 0.05, "A small sealable bag for snacks and small pieces of trash.", AdventureType::ALL, TerrainType::ALL, GearCategory::OTHER, true, false, false }},
    { GearType::TREKKING_POLES, { 1.00, 1.0, "Reduces impact on knees and provides stability.", AdventureType::ALL, TerrainType::MOUNTAIN, GearCategory::OTHER, false, false, true }}
};

/**
 * @brief Represents a piece of gear that can be packed.
 */
class GearItem : public Item
{
public:
    /**
     * @brief Constructs a GearItem from a GearType enum value.
     * @param gearEnum The gear type to construct.
     */
    GearItem(GearType gearEnum);

    /**
     * @brief Constructs a GearItem from a string identifier.
     * @param name The name of the gear in the registry.
     */
    GearItem(const QString& name);

    virtual bool isGear() const override { return true; }
    virtual bool isWasteStorage() const override { return wasteStorage; }
    virtual bool isTrekkingPole() const override { return isPole; }

    /**
     * @brief Gear items currently have no additional numeric stats.
     */
    virtual std::map<QString, double> getStatMap() const override;

    /**
     * @brief Gets the category of this gear item for the 10 Essentials.
     */
    GearCategory getCategory() const;

    /**
     * @brief Gets the terrain type associated with this gear.
     */
    TerrainType getTerrainType() const;

    /**
     * @brief Gets the adventure type associated with this gear.
     */
    AdventureType getAdventureType() const;

private:
    GearCategory category;
    TerrainType terrainType;
    AdventureType adventureType;
    bool wasteStorage;
    bool isPole;
};

#endif // GEARITEM_H
