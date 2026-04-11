/**
 * @file itemreportbuilder.cpp
 * @brief Implementation of the ItemReportBuilder utility.
 */

#include "itemreportbuilder.h"
#include <map>

QString ItemReportBuilder::getToolTip(const Item* item)
{
    if (!item) return "";

    // View-Layer Label Mapping (Pure MVC)
    static const std::map<QString, QString> labelMap = {
        { "calories",  "Calories" },
        { "carbs",     "Carbs (g)" },
        { "protein",   "Protein (g)" },
        { "fat",       "Fat (g)" },
        { "fiber",     "Fiber (g)" },
        { "hydration", "Hydration (oz)" },
        { "caffeine",  "Caffeine (mg)" },
        { "alcohol",   "Alcohol (units)" }
    };

    QString report;

    // 1. Map Programmatic IDs from the Model to UI Labels
    auto stats = item->getStatMap();
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        QString label = labelMap.count(it->first) ? labelMap.at(it->first) : it->first;
        report += label + ": " + QString::number(it->second, 'f', 1) + "\n";
    }

    // 2. Add Universal Stats
    report += "Weight: " + QString::number(item->getWeight(), 'f', 2) + " lbs\n";

    // 3. Add Narrative Description
    if (!item->getDescription().isEmpty()) {
        report += "\n" + item->getDescription();
    }

    return report;
}
