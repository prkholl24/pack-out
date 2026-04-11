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
 */
struct FoodStats {
    /**
     * @brief Nutrition values used for gameplay scoring.
     */
    NutritionProfile nutrition;

    /**
     * @brief Total weight of the item in pounds.
     */
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

    virtual bool isFood() const override { return true; }

    /**
     * @brief Returns nutritional data keys and values.
     */
    virtual std::map<QString, double> getStatMap() const override;
};

#endif // FOODITEM_H
