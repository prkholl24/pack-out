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
    WHISTLE
};

#endif // GEARITEM_H
