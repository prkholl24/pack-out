/**
 * @file item.h
 * @brief Header file for the PackableItem class
 */
#ifndef ITEM_H
#define ITEM_H

#include <QString>
#include <map>

/**
 * @class PackableItem
 */
class PackableItem {
    public:
        /**
         * @brief Constructor for PackableItem
         *
         * Constructs a new PackableItem and sets the weight.
         * Also calls createToopTip to set the internal state
         * of toolTip.
         */
        PackableItem();

        /**
         * @brief Gets the weight of this item
         *
         * Returns the weight as a constant double.
         *
         * @return The weight of this item
         */
        double getWeight() const;

        /**
         * @brief Gets a tool tip for this item
         *
         * A Tool-Tip is a short message detailing this item
         * Example...
         *
         *      Banana:
         *
         * 105          Calories
         * 0.5          Weight
         *
         * The potassium in a banana
         * helps prevent muscle cramps
         * and regulate fluid balance.
         */
        QString getToolTip();

    protected:

        /**
         * @brief weight in pounds
         *
         * The weight of this item in pounds (lbs).
         */
        double weight = -1;

        const QString* description = nullptr;

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
        virtual std::map<QString, double> getStats();
};

#endif // ITEM_H
