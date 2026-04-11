/**
 * @file item.h
 * @brief Base class for all packable items
 *
 * Defines the Item interface, which all items
 * (FoodItem, WaterItem, etc.) must implement. Each item
 * provides its stats through getStats().
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 */

#ifndef ITEM_H
#define ITEM_H
#include "nutritionprofile.h"

#include <QString>
#include <map>
#include "nutritionprofile.h"

/**
 * @brief Base class representing an item that can be packed.
 *
 * All derived item types must provide their stats
 * (e.g. calories, water, weight) through the getStats() method.
 */
class Item {
    public:
        /**
         * @brief Constructor for Item.
         *
         * Constructs a new Item and sets the weight.
         */
        Item();

        /**
         * @brief Virtual destructor for safe polymorphic cleanup.
         */
        virtual ~Item() = default;

        /**
         * @brief Gets the weight of this item.
         *
         * @return The weight of this item.
         */
        double getWeight() const;

        /**
         * @brief Gets the nutrition profile associated with this item.
         *
         * Returns the NutritionProfile containing all gameplay-relevant
         * values such as calories, hydration, sugar, caffeine, and alcohol.
         * This information is used by the Level class to evaluate the
         * effectiveness of the player's packed items.
         *
         * The returned reference is const to prevent modification of
         * the item's internal nutrition data.
         *
         * @return A const reference to this item's NutritionProfile.
         */
        const NutritionProfile& getNutrition() const;

        /**
         * @brief Gets the description of this item.
         *
         * @return A QString containing the description.
         */
        QString getDescription() const;

        /**
         * @brief Whether this item is a food item.
         */
        virtual bool isFood() const { return false; }

        /**
         * @brief Whether this item is a drink item.
         */
        virtual bool isDrink() const { return false; }

        /**
         * @brief Whether this item is a gear item.
         */
        virtual bool isGear() const { return false; }

    protected:
        /**
         * @brief weight in pounds.
         *
         * The weight of this item in pounds (lbs).
         */
        double weight;

        /**
         * @brief Description of this item.
         *
         * Points to the description text used for this item.
         */
        QString description;

        /**
         * @brief Nutrition data for this item.
         */
        NutritionProfile nutrition;
};

#endif // ITEM_H
