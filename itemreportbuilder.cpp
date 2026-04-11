/**
 * @file itemreportbuilder.cpp
 * @brief Implementation of the ItemReportBuilder utility.
 *
 * Implements logic to format Item data into strings for tooltips.
 */

#include "itemreportbuilder.h"

QString ItemReportBuilder::getToolTip(const Item* item)
{
    if (!item) return "";

    QString report;
    const NutritionProfile& n = item->getNutrition();

    // Specific formatting for Food
    if (item->isFood()) {
        report += "Calories: " + QString::number(n.calories) + "\n";
        report += "Carbs: " + QString::number(n.carbs) + "g\n";
        report += "Fat: " + QString::number(n.fat) + "g\n";
        report += "Protein: " + QString::number(n.protein) + "g\n";
        report += "Sugar: " + QString::number(n.sugar) + "g\n";
    }
    // Specific formatting for Drinks
    else if (item->isDrink()) {
        report += "Hydration: " + QString::number(n.hydrationOz) + "oz\n";
        report += "Calories: " + QString::number(n.calories) + "\n";
        report += "Sugar: " + QString::number(n.sugar) + "g\n";
        if (n.caffeineMg > 0)
            report += "Caffeine: " + QString::number(n.caffeineMg) + "mg\n";
        if (n.alcohol > 0)
            report += "Alcohol: " + QString::number(n.alcohol) + " units\n";
    }
    // Specific formatting for Gear
    else if (item->isGear()) {
        // Gear items currently only show weight and description
    }

    report += "Weight: " + QString::number(item->getWeight(), 'f', 2) + " lbs\n";

    if (!item->getDescription().isEmpty()) {
        report += "\n" + item->getDescription();
    }

    return report;
}
