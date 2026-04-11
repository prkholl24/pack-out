/**
 * @file item.h
 * @brief Base class for all packable items.
 */

#ifndef ITEM_H
#define ITEM_H

#include <QString>
#include <map>
#include "nutritionprofile.h"

/**
 * @brief Base class representing an item that can be packed.
 *
 * All derived item types must provide their stats through getStatMap().
 */
class Item {
public:
    /**
     * @brief Constructs a new Item and sets default values.
     */
    Item();

    /**
     * @brief Virtual destructor for safe polymorphic cleanup.
     */
    virtual ~Item() = default;

    // --- Physical Properties ---

    /**
     * @brief Gets the weight of this item in pounds.
     */
    double getWeight() const;

    /**
     * @brief Gets the volume of this item in liters.
     */
    double getVolume() const;

    /**
     * @brief Whether this item produces waste (e.g. wrappers).
     */
    bool generatesWaste() const;

    /**
     * @brief Gets the narrative description of this item.
     */
    QString getDescription() const;

    // --- Gameplay Logic ---

    /**
     * @brief Gets the nutrition profile associated with this item.
     */
    const NutritionProfile& getNutrition() const;

    /**
     * @brief Returns a map of numeric stats (programmatic IDs to values).
     * @return A map used by the View layer for display.
     */
    virtual std::map<QString, double> getStatMap() const = 0;

    // --- Type & Material Checks ---

    virtual bool isFood() const { return false; }
    virtual bool isDrink() const { return false; }
    virtual bool isGear() const { return false; }
    virtual bool isWasteStorage() const { return false; }
    virtual bool isTrekkingPole() const { return false; }

    /**
     * @brief Whether this item is cotton (safety hazard in rain/snow).
     */
    bool isCottonMaterial() const;

protected:
    double weight;
    double volumeLiters;
    bool producesWaste;
    bool isCotton;
    QString description;
    NutritionProfile nutrition;
};

#endif // ITEM_H
