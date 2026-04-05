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
 * @brief Struct for a water item
 */
struct WaterStats {
    double capacity;
    double weight;
    QString description;
};

/**
 * @brief Maps Waters to a premade struct.
 *
 * Used by the WaterItem constructor. Maps enumerated
 * `Water` object to a premade struct.
 */
static const std::map<Water, WaterStats> WaterRegistry = {
    /**
     * TEMPLATE:

    { Water::NAME, {
        // CAPACITY
        95.0,

         // WEIGHT
        0.5,

        // DESCRIPTION
        QString("DESCRIPTION")
    }},

     */
    { Water::BOTTLE, {
                      // CAPACITY
                      1.0,

                      // WEIGHT
                      0.5,

                      // DESCRIPTION
                      QString("Standard 1L water bottle.")
                  }},
    { Water::CAMELBACK, {
                      // CAPACITY
                      2.0,

                      // WEIGHT
                      0.0,

                      // DESCRIPTION
                      QString("Camelback allows you to store 2L of water without taking up any space in your bag.")
                  }},
};
class WaterItem : public PackableItem {
public:
    /**
     * @brief Public constructor for WaterItem
     *
     * Accepts the `Water` enum->Looks up the WaterStats
     * ->calls the private constructor using the WaterStates
     * values
     *
     * @param waterStruct – Enum maps to WaterStats
     */
    WaterItem(Water waterStruct);


private:
    /**
     * @brief capacity
     *
     * Capacity of this Water item in liters.
     */
    double capacity;

protected:
    /**
     * @brief Gets the stats of this item
     *
     * Returns a structured output of this items
     * stats
     * Example:
     *
     * Calories               95
     * Weight(lbs)           0.5
     *
     * @return a map containing the stat and its value
     */
    std::map<QString, double> getStats() override;
};

#endif // WATERITEM_H
