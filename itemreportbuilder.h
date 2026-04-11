/**
 * @file itemreportbuilder.h
 * @brief Utility class for formatting item data for the UI.
 *
 * This class handles all presentation logic for items, such as
 * generating tooltips and stat summaries, ensuring the Item
 * model classes remain focused on data.
 *
 * Team: Green Goombas
 * @date 4/10/2026
 */

#ifndef ITEMREPORTBUILDER_H
#define ITEMREPORTBUILDER_H

#include <QString>
#include "item.h"

/**
 * @brief Responsible for generating user-facing text for items.
 *
 * Use this class to generate tooltips or detailed stat lists
 * from any Item-derived object.
 */
class ItemReportBuilder {
public:
    /**
     * @brief Formats an item's data into a multi-line tooltip string.
     *
     * @param item The item to format.
     * @return A QString containing the formatted tooltip.
     */
    static QString getToolTip(const Item* item);
};

#endif // ITEMREPORTBUILDER_H
