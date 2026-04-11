/**
 * @file itemreportbuilder.h
 * @brief Utility for building item tooltips.
 */

#ifndef ITEMREPORTBUILDER_H
#define ITEMREPORTBUILDER_H

#include <QString>
#include "item.h"

/**
 * @brief Responsible for formatting Item data into user-facing tooltips.
 */
class ItemReportBuilder
{
public:
    /**
     * @brief Generates a formatted tooltip string for an item.
     * @param item - The item to format.
     * @return A QString containing the formatted tooltip.
     */
    static QString getToolTip(const Item* item);
};

#endif // ITEMREPORTBUILDER_H
