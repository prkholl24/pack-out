/**
 * @file levelreportbuilder.h
 * @brief Contains the LevelReportBuilder class used to generate reports.
 *
 * Defines LevelReportBuilder, which is responsible for formatting
 * and generating user-facing reports based on LevelResult data.
 * This class handles presentation only and does not perform any
 * grading or evaluation logic.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/8/2026
 */

#ifndef LEVELREPORTBUILDER_H
#define LEVELREPORTBUILDER_H

#include <QString>
#include "levelresult.h"

/**
 * @brief Builds a formatted report for a LevelResult.
 *
 * This class converts evaluation results into a readable
 * report for the player. It does not perform any grading
 * or calculations.
 */
class LevelReportBuilder
{
public:
    /**
     * @brief Builds the full report for a level result.
     *
     * Combines the summary and either a pass or fail message
     * into a single formatted string.
     *
     * @param result The evaluated results of the backpack.
     * @return A QString containing the full report.
     */
    static QString buildReport(const LevelResult& result);

private:
    /**
     * @brief Builds the story-style outcome summary.
     *
     * This is the narrative text shown at the top of the debrief.
     */
    static QString buildOutcomeSummary(const LevelResult& result);

    /**
     * @brief Builds the stat breakdown shown under the story summary.
     */
    static QString buildStatsReport(const LevelResult& result);
};

#endif // LEVELREPORTBUILDER_H
