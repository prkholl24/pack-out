/**
 * @file fooditem.h
 * @brief Contains the FoodItem method headers and pre-made definitions of Foods.
 *
 * The FoodItem interface is a way to create foods
 * through the Map data structure, avoiding the need to
 * create a new child class for every food instance we want to add.
 *
 * Team: Green Goombas
 * Github: GitHub: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Aldair, Lindsey Reese, Kent Wilkison
 * @date 4/8/2026
 *
 * Last Revision: 4/8/2026 3:58 MST by KENT WILKISON
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
 * A food item contains calories and a sustenance variable
 * Inherits from `PackableItem`: int weight, QString toolTip
 */
class FoodItem : public PackableItem {
public:
    /**
     * @brief Public constructor for FoodItem
     *
     * Accepts the `Food` enum->Looks up the FoodStats
     * ->calls the private constructor using the FoodStates
     * values
     *
     * @param foodEnum maps to an existing FoodStats struct
     */
    FoodItem(Food foodEnum);


private:
    /**
     * @brief calories contained in this FoodItem
     *
     * Calories are the primary unit of measure used
     * to determine whether the user has packed enough
     * food for their trip.
     */
    double calories;

    /**
     * @brief sustenance
     *
     * ???
     * Not sure how this is going to work.
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
    std::map<QString, double> getStats() override;
};

#endif // FOODITEM_H
