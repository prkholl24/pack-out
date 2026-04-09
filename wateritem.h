/**
 * @file wateritem.h
 * @brief Contains the WaterItem class and supporting water definitions.
 *
 * Defines the WaterItem class, which represents water that the player
 * can pack in their backpack. This file also includes the Water enum,
 * the WaterStats struct, and the WaterRegistry used to look up the
 * values for each water item.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 */

#ifndef WATERITEM_H
#define WATERITEM_H

#include "item.h"

/**
 * @brief The source of truth for WaterItem.
 *
 * Contains all the possible values for WaterItem.
 */
enum class Water {
    BOTTLE,
    CAMELBACK
};

/**
 * @brief Struct representing a water item.
 *
 * Stores the values associated with a given Water enum value.
 */
struct WaterStats {
    double capacity;
    double containerWeight;
    QString description;
};

/**
 * @brief Maps Waters to a premade struct.
 *
 * Used by the WaterItem constructor. Maps an enumerated
 * `Water` value to a premade struct.
 */
static const std::map<Water, WaterStats> WaterRegistry = {
    /**
     * TEMPLATE:

    { Water::NAME, {
        // CAPACITY
        32.0,

        // WEIGHT
        0.2,

        // DESCRIPTION
        QString("DESCRIPTION.")
    }},

     */
    { Water::BOTTLE, {
                      // CAPACITY IN oz
                      32.0,

                      // WEIGHT IN lbs
                      0.2,

                      // DESCRIPTION of container and capacity.
                      QString("Standard 32 oz water bottle.")
                  }},
    { Water::CAMELBACK, {
                      // CAPACITY
                      64.0,

                      // WEIGHT
                      0.5,

                      // DESCRIPTION
                      QString("Camelback allows you to store 64 oz of water without taking up any space in your bag.")
                  }},
};

/**
 * @brief Represents a WaterItem.
 *
 * A WaterItem stores the amount of water it holds and * stores the
 * amount of water it holds and its total weight.
 *
 * Inherits from `Item` and provides water-specific stats such as
 * capacity in ounces.
 */
class WaterItem : public Item {
public:
    /**
     * @brief Public constructor for WaterItem.
     *
     * Accepts the `Water` enum, looks up the corresponding
     * WaterStats values in the WaterRegistry, and uses those
     * values to initialize the object.
     *
     * @param waterEnum Maps to an existing WaterStats struct.
     */
    WaterItem(Water waterEnum);

private:
    /**
     * @brief Capacity of this WaterItem
     *
     * Stores the amount of water this item holds in ounces.
     */
    double capacity;

    /**
     * @brief Calculates the weight of water based on ounces.
     *
     * Each ounce of water weighs about (8.34 / 128) pounds,
     * so we multiply ounces by this value to get total weight.
     *
     * @param ounces The amount of water in ounces.
     * @return The weight of the water in pounds.
     */
    double calculateWaterWeight(double ounces) const;

protected:
    /**
     * @brief Gets the stats of this item
     *
     * Returns a structured output of this item's
     * stats.
     * Example:
     *
     * Water(oz)            32
     * Weight(lbs)          0.5
     *
     * @return A map of stat names and their values.
     */
    std::map<QString, double> getStats() const override;
};

#endif // WATERITEM_H
