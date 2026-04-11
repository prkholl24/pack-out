/**
 * @file drinkitem.h
 * @brief Contains the DrinkItem class, DrinkType enum, and drink registry data.
 *
 * Defines DrinkItem, which represents a drink the player can pack.
 * Drink items use predefined values from the DrinkRegistry for
 * nutrition, weight, description, and hydration effectiveness.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/9/2026
 */

#ifndef DRINKITEM_H
#define DRINKITEM_H

#include <map>
#include "item.h"

/**
 * @brief Enumerates the available drink item types.
 *
 * Listed in alphabetical order.
 */
enum class DrinkType {
    BEER,
    ENERGY_DRINK,
    SODA,
    SPORTS_DRINK,
    WATER_BOTTLE,
    WATER_CAMELBAK
};

/**
 * @brief Stores the data used to construct one DrinkItem.
 */
struct DrinkStats {
    /**
     * @brief Nutrition values used for gameplay scoring.
     */
    NutritionProfile nutrition;

    /**
     * @brief Total weight of the drink item in pounds.
     */
    double weight;

    /**
     * @brief Volume of the drink item in liters.
     */
    double volume;

    /**
     * @brief Whether the drink item produces waste.
     */
    bool producesWaste;

    /**
     * @brief Description used in the UI.
     */
    QString description;

    /**
     * @brief Multiplier used to determine hydration effectiveness.
     */
    double hydrationMultiplier;
};

/**
 * @brief Registry of all predefined drink items.
 *
 * This acts as the source of truth for DrinkItem values.
 */
static const std::map<DrinkType, DrinkStats> DrinkRegistry = {
    { DrinkType::BEER, {
                          { 150.0, 12.0, 1.0, 13.0, 0.0, 1.0, 0.0, 5.0 },
                          0.85, 0.4, true,
                          QString("Beer provides calories, but alcohol reduces hydration effectiveness."),
                          0.25
                      }},
    { DrinkType::ENERGY_DRINK, {
                                  { 10.0, 16.0, 0.0, 3.0, 0.0, 0.0, 160.0, 0.0 },
                                  1.10, 0.5, true,
                                  QString("An energy drink provides caffeine, but too much can hurt hydration."),
                                  0.60
                              }},
    { DrinkType::SODA, {
                          { 150.0, 12.0, 0.0, 39.0, 0.0, 39.0, 34.0, 0.0 },
                          0.85, 0.4, true,
                          QString("Soda gives quick sugar and some fluid, but is not a strong hydration choice."),
                          0.25
                      }},
    { DrinkType::SPORTS_DRINK, {
                                  { 80.0, 20.0, 0.0, 21.0, 0.0, 21.0, 0.0, 0.0 },
                                  1.45, 0.6, true,
                                  QString("A sports drink provides fluid and quick carbohydrates."),
                                  0.80
                              }},
    { DrinkType::WATER_BOTTLE, {
                                  { 0.0, 32.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
                                  2.28, 1.0, true,
                                  QString("A 32 oz water bottle. A basic hydration option that can be packed in the bag."),
                                  1.00
                              }},
    { DrinkType::WATER_CAMELBAK, {
                                    { 0.0, 64.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
                                    4.67, 2.0, false,
                                    QString("A 64 oz camelbak-style water reservoir for carrying more water."),
                                    1.00
                                }}
};

/**
 * @brief Represents a drink item that can be packed by the player.
 */
class DrinkItem : public Item
{
public:
    /**
     * @brief Constructs a DrinkItem from a DrinkType value.
     *
     * Looks up the corresponding DrinkStats in the DrinkRegistry
     * and uses them to initialize this DrinkItem.
     *
     * @param drinkEnum The drink type to construct.
     */
    DrinkItem(DrinkType drinkEnum);

    virtual bool isDrink() const override { return true; }

    /**
     * @brief Returns hydration and nutrition data keys and values.
     */
    virtual std::map<QString, double> getStatMap() const override;

    /**
     * @brief Gets the effective hydration provided by this drink.
     *
     * @return The hydration contribution after applying the multiplier.
     */
    double getEffectiveHydration() const;

private:
    /**
     * @brief Multiplier used for hydration effectiveness.
     */
    double hydrationMultiplier;
};

#endif // DRINKITEM_H
