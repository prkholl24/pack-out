/**
 * @file item.h
 * @brief Header file for the PackableItem class
 */
#ifndef ITEM_H
#define ITEM_H

#include <QString>

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
         *
         * @param weight – In pounds.
         */
        PackableItem(double weight);

        /**
         * @brief Gets the weight of this item
         *
         * Returns the weight as a constant double.
         *
         * @return The weight of this item
         */
        double getWeight() const;

    private:
        /**
         * @brief toolTip is a short message detailing this item
         * Example...
         *
         *      Banana:
         *
         * 105          Calories
         *
         * The potassium in a banana
         * helps prevent muscle cramps
         * and regulate fluid balance.
         */
        QString toolTip;

        /**
         * @brief weight in pounds
         *
         * Used to calculate exhaustion from
         * a level.
         */
        double weight;

    protected:
        /**
         * @brief Defines how a tool-tip should look
         *
         * Virtual method is called by the PackableItem constructor
         * in order to set the value of this toolTip
         */
        virtual QString createTooltip();
};

#endif // ITEM_H
