/**
 * @file fooditem.h
 * @brief Contains the FoodItem class, Food enum, and supporting data structures.
 *
 * Defines FoodItem, which represents a food that the player
 * can pack in their bag. Each FoodItem pulls its values from
 * the FoodRegistry and stores gameplay nutrition data such as
 * calories, carbs, protein, fat, and sugar.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/9/2026
 */

#ifndef FOODITEM_H
#define FOODITEM_H

#include <map>
#include "item.h"
#include "qpixmap.h"

/**
 * @brief Enumerates the available food item types.
 */
enum class Food {
    BANANA,
    BEEF_JERKY,
    GRANOLA_BAR,
    MIXED_NUTS,
    PEANUT_BUTTER_SANDWICH,
    POTATO,
    TRAIL_MIX
};

/**
 * @brief Stores the data used to construct one FoodItem.
 * @brief Struct for a food item
 *
 * Contains all assets that a food-item needs to know.
 */
struct FoodStats {
    /**
     * @brief Nutrition values used for gameplay scoring.
     */
    NutritionProfile nutrition;

    /**
     * @brief Total weight of the item in pounds.
     */
    QString imageFilePath;
    double weight;

    /**
     * @brief Volume of the item in liters.
     */
    double volume;

    /**
     * @brief Whether the item produces waste.
     */
    bool producesWaste;

    /**
     * @brief Description used in the UI.
     */
    QString description;
};

/**
 * @brief Registry of all predefined food items.
 *
 * This acts as the source of truth for all FoodItem values.
 */
static const std::map<Food, FoodStats> FoodRegistry = {
    /**
     * TEMPLATE:

       { Food::{FOOD_NAME}, {
            // ImageFilePath
            "{IMAGE FILEPATH}",

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
                       { 105.0, 0.0, 1.3, 27.0, 0.4, 14.0, 0.0, 0.0 },
                       0.26, 0.4, true,
                       QString("The potassium in a banana helps support muscle function on the trail.")
                   }},
    { Food::BEEF_JERKY, {
                           { 116.0, 0.0, 9.0, 3.0, 7.0, 2.0, 0.0, 0.0 },
                           0.10, 0.1, true,
                           QString("Beef jerky is high in protein and lightweight, making it a great trail snack.")
                       }},
    { Food::GRANOLA_BAR, {
                            { 190.0, 0.0, 3.0, 29.0, 7.0, 11.0, 0.0, 0.0 },
                            0.12, 0.1, true,
                            QString("Granola bars provide quick energy from carbs and sugars.")
                        }},
    { Food::MIXED_NUTS, {
                           { 170.0, 0.0, 5.0, 6.0, 15.0, 2.0, 0.0, 0.0 },
                           0.15, 0.2, true,
                           QString("Mixed nuts are calorie-dense and rich in healthy fats for sustained energy.")
                       }},
    { Food::PEANUT_BUTTER_SANDWICH, {
                                       { 350.0, 0.0, 12.0, 30.0, 18.0, 8.0, 0.0, 0.0 },
                                       0.40, 0.3, true,
                                       QString("A peanut butter sandwich offers a balance of fats, protein, and carbs.")
                                   }},
                       // IMAGE FILE PATH
                       QString("banana.png"), // there seems to be a bug here? the order of initialization seems to be crashing the program, i think?

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
                       { 160.0, 0.0, 4.0, 37.0, 0.2, 2.0, 0.0, 0.0 },
                       0.38, 0.5, true,
                       QString("A potato provides steady carbs, but it is heavier and bulkier to carry.")
                   }},
    { Food::TRAIL_MIX, {
                          { 300.0, 0.0, 8.0, 32.0, 18.0, 20.0, 0.0, 0.0 },
                          0.20, 0.2, true,
                          QString("Trail mix combines nuts and dried fruit for quick and sustained energy.")
                      }}
                        // IMAGE FILE PATH
                        QString("potato.png"),

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
 * @brief Represents a food item that can be packed by the player.
 */
class FoodItem : public Item
{
public:
    /**
     * @brief Constructs a FoodItem from a Food enum value.
     *
     * Looks up the corresponding FoodStats in the FoodRegistry
     * and uses them to initialize this FoodItem.
     *
     * @param foodEnum The food type to construct.
     */
    FoodItem(Food foodEnum);

    /**
     * @brief Constructs a FoodItem from a string identifier.
     * @param name The name of the food in the registry.
     */
    FoodItem(const QString& name);

    virtual bool isFood() const override { return true; }

    double calories;

    /**
     * @brief Sustenance value for this FoodItem.
     *
     * Sustenance represents how filling this food is
     * and may be used in grading or gameplay logic.
     */
    double sustenance;

    /**
     * @brief Displayed image for this FoodItem.
     *
     * This determines what sprite will show up for the
     * food image.
     */
    QPixmap image;

protected:
    /**
     * @brief Returns nutritional data keys and values.
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
    virtual std::map<QString, double> getStatMap() const override;
};

#endif // FOODITEM_H
