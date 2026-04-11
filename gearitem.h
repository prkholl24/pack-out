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
    CAST_IRON_SKILLET
};

/**
 * @brief Stores the data used to construct one GearItem.
 */
struct GearStats {
    /**
     * @brief Total weight of the item in pounds.
     */
    double weight;

    /**
     * @brief Description used in the UI.
     */
    QString description;

    /**
     * @brief The adventure type this gear is best suited for.
     */
    AdventureType adventure;

    /**
     * @brief The terrain type this gear is best suited for.
     */
    TerrainType terrain;
};

/**
 * @brief Registry of all predefined gear items.
 */
static const std::map<GearType, GearStats> GearRegistry = {
    { GearType::BEAR_SPRAY,  { 0.75, "Essential for safety in bear country.", AdventureType::BACKPACKING, TerrainType::FOREST }},
    { GearType::COMPASS,     { 0.10, "Helps you find your way when GPS fails.", AdventureType::ALL, TerrainType::ALL }},
    { GearType::FIRESTARTER, { 0.15, "Helps you get a fire going in damp conditions.", AdventureType::CAMPING, TerrainType::ALL }},
    { GearType::FIRST_AID_KIT, { 1.50, "Contains basic medical supplies for the trail.", AdventureType::ALL, TerrainType::ALL }},
    { GearType::FLASHLIGHT,  { 0.50, "Provides light for night activities.", AdventureType::CAMPING, TerrainType::ALL }},
    { GearType::HAT,         { 0.20, "Protects your head from the sun and cold.", AdventureType::ALL, TerrainType::ALL }},
    { GearType::MATCHES,     { 0.05, "Simple, reliable way to start a fire.", AdventureType::CAMPING, TerrainType::ALL }},
    { GearType::POCKET_KNIFE,{ 0.30, "A versatile tool for various tasks.", AdventureType::ALL, TerrainType::ALL }},
    { GearType::RAIN_JACKET, { 0.80, "Keeps you dry during unexpected rain.", AdventureType::ALL, TerrainType::ALL }},
    { GearType::SLEEPING_BAG,{ 3.00, "Provides warmth for overnight trips.", AdventureType::CAMPING, TerrainType::ALL }},
    { GearType::SUNSCREEN,   { 0.25, "Protects your skin from UV rays.", AdventureType::ALL, TerrainType::DESERT }},
    { GearType::TENT,         { 5.00, "A portable shelter for camping.", AdventureType::CAMPING, TerrainType::ALL }},
    { GearType::WATER_FILTER,{ 0.40, "Makes stream water safe to drink.", AdventureType::BACKPACKING, TerrainType::ALL }},
    { GearType::WHISTLE,     { 0.05, "A simple signaling device for emergencies.", AdventureType::ALL, TerrainType::ALL }},
    { GearType::COTTON_SHIRT, { 0.80, "A basic t-shirt. Comfortable until it gets wet.", AdventureType::ALL, TerrainType::ALL }},
    { GearType::WOOL_SHIRT,   { 0.50, "Moisture-wicking and stays warm when wet. Essential for safety.", AdventureType::ALL, TerrainType::ALL }},
    { GearType::CAST_IRON_SKILLET, { 8.00, "Great for cooking, but very heavy for the trail.", AdventureType::CAMPING, TerrainType::ALL }}
};

/**
 * @brief Represents a piece of gear that can be packed.
 */
class GearItem : public Item
{
public:
    /**
     * @brief Constructs a GearItem from a GearType enum value.
     *
     * @param gearEnum The gear type to construct.
     */
    GearItem(GearType gearEnum);

    virtual bool isGear() const override { return true; }

    /**
     * @brief Gets the terrain type associated with this gear.
     */
    TerrainType getTerrainType() const;

    /**
     * @brief Gets the adventure type associated with this gear.
     */
    AdventureType getAdventureType() const;

private:
    /**
     * @brief The terrain type for this gear item.
     */
    TerrainType terrainType;

    /**
     * @brief The adventure type for this gear item.
     */
    AdventureType adventureType;
};

#endif // GEARITEM_H
