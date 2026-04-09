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

#include <QString>
#include <map>

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
         * @brief Gets a tool tip for this item
         *
         * A Tool-Tip is a short message detailing this item
         * Example:
         *
         *      Banana:
         *
         * 105          Calories
         * 0.5          Weight
         *
         * The potassium in a banana
         * helps prevent muscle cramps
         * and regulate fluid balance.
         *
         * @return A QString containing the tooltip text.
         */
        QString getToolTip() const;

        /**
         * @brief Gets the stats of this item.
         *
         * Returns a structured output of this item's
         * stats..
         * Example:
         *
         * Calories               95
         * Weight(lbs)           0.5
         *
         * @return a map containing the stat and its value.
         */
        virtual std::map<QString, double> getStats() const;

    protected:

        /**
         * @brief weight in pounds.
         *
         * The weight of this item in pounds (lbs).
         */
        double weight = -1;

        /**
         * @brief Description of this item.
         *
         * Points to the description text used for this item.
         */
        const QString* description = nullptr;
};

#endif // ITEM_H
