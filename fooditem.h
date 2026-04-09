/**
 * @file fooditem.h
 * @brief Contains the FoodItem class, Food enum, and supporting data structures.
 *
 * Defines the FoodItem class, which represents food that the player
 * can pack in their backpack. Each food item pulls its values from
 * the FoodRegistry and provides stats such as calories and weight
 * for use in grading and gameplay.
 *
 * Also includes:
 * - The Food enum, which defines all available food item types
 * - The FoodStats struct, which stores the properties of each food item
 * - The FoodRegistry map, which serves as a lookup table from Food enum values
 *   to their corresponding stats
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 */

#ifndef FOODITEM_H
#define FOODITEM_H

#include "item.h"
#include "qpixmap.h"

/**
 * @brief The source of truth for FoodItem.
 *
 * Contains all the possible values for FoodItem.
 */
enum class Food {
    BANANA,
    POTATO
};

/**
 * @brief Struct for a food item
 *
 * Contains all assets that a food-item needs to know.
 */
struct FoodStats {
    QPixmap image;
    double calories;
    double sustenance;
    double weight;
    QString description;
};

/**
 * @brief Maps Foods to a premade struct.
 *
 * Used by the FoodItem constructor. Maps enumerated
 * `Food` object to a premade struct.
 */
static const std::map<Food, FoodStats> FoodRegistry = {
    /**
     * TEMPLATE:

       { Food::{FOOD_NAME}, {
            // Image
            pixmap("{IMAGE}"),

            // Calories
            95.0,

            // SUSTENANCE
            10.0,

            // WEIGHT
            0.5,

            // DESCRIPTION
            QString("{DESCRIPTION}")
       }},

     */
    { Food::BANANA, {
                       // IMAGE
                       QPixmap("banana.png"),

                       // CALORIES
                       95.0,

                       // SUSTENANCE
                       10.0,

                       // WEIGHT
                       0.5,

                       // DESCRIPTION
                       QString("The potassium in a banana helps prevent muscle cramps and regulate fluid balance.")
                   }},

    { Food::POTATO, {
                        // IMAGE
                        QPixmap("potato.png"),

                       // CALORIES
                       340.0,

                       // SUSTENANCE
                       2.5,

                       // WEIGHT
                       1.25,

                       // DESCRIPTION
                       QString("Dense in calories but high in weight.")
                   }},
};

/**
 * @brief Represents a Food Item
 *
 * A FoodItem contains calories and a sustenance values.
 * Inherits from `Item`: weight, description.
 */
class FoodItem : public Item {
public:
    /**
     * @brief Public constructor for FoodItem.
     *
     * Accepts the `Food` enum, looks up the corresponding
     * FoodStats values in the FoodRegistry, and uses those
     * values to initialize the object.
     *
     * @param foodEnum Maps to an existing FoodStats struct.
     */
    FoodItem(Food foodEnum);

private:
    /**
     * @brief Calories contained in this FoodItem
     *
     * Calories are the primary unit of measure used
     * to determine whether the user has packed enough
     * food for their trip.
     */
    double calories;

    /**
     * @brief Sustenance value for this FoodItem.
     *
     * Sustenance represents how filling this food is
     * and may be used in grading or gameplay logic.
     */
    double sustenance;

protected:
    /**
     * @brief Gets the stats of this item
     *
     * Returns a structured output of this items
     * stats
     *
     * Example:
     * - Calories:               95
     * - Weight(lbs):           0.5
     *
     * @return a map containing the stat and its value
     */
    std::map<QString, double> getStats() const override;
};

#endif // FOODITEM_H
