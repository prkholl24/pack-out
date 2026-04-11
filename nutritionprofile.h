/**
 * @file nutritionprofile.h
 * @brief Contains the NutritionProfile struct used to store gameplay-related nutrition data.
 *
 * Defines NutritionProfile, which stores the shared nutritional values used
 * to evaluate food and drink items in the game. This allows FoodItem and
 * DrinkItem to use one consistent model for calories, hydration, sugar,
 * caffeine, alcohol, and macronutrients.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/9/2026
 */

#ifndef NUTRITIONPROFILE_H
#define NUTRITIONPROFILE_H

/**
 * @brief Stores the nutritional values of a food or drink item.
 *
 * This struct is used as a shared model for gameplay scoring.
 * Not every value will be meaningful for every item, but keeping
 * them together allows all item types to be evaluated consistently.
 */
struct NutritionProfile
{
    /**
     * @brief Calories provided by this item.
     */
    double calories = 0.0;

    /**
     * @brief Hydration provided by this item in ounces.
     */
    double hydrationOz = 0.0;

    /**
     * @brief Protein content in grams.
     */
    double protein = 0.0;

    /**
     * @brief Carbohydrate content in grams.
     */
    double carbs = 0.0;

    /**
     * @brief Fat content in grams.
     */
    double fat = 0.0;

    /**
     * @brief Sugar content in grams.
     */
    double sugar = 0.0;

    /**
     * @brief Fiber content in grams. (Crucial for energy stability)
     */
    double fiber = 0.0;

    /**
     * @brief Sodium content in milligrams. (Affects hydration)
     */
    double sodium = 0.0;

    /**
     * @brief Caffeine content in milligrams.
     */
    double caffeineMg = 0.0;

    /**
     * @brief Alcohol content used for hydration penalties.
     */
    double alcohol = 0.0;
};

#endif // NUTRITIONPROFILE_H
