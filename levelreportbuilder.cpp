/**
 * @file levelreportbuilder.cpp
 * @brief Implementation of the LevelReportBuilder class.
 *
 * Implements LevelReportBuilder, which is responsible for formatting
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

#include "levelreportbuilder.h"

QString LevelReportBuilder::buildReport(const LevelResult& result)
{
    QString report = buildSummaryReport(result);

    if (result.passed)
        report += buildPassReport();
    else
        report += buildFailReport(result);

    return report;
}

QString LevelReportBuilder::buildSummaryReport(const LevelResult& result)
{
    QString report;
    report += "Backpack Report\n";
    report += "Grade: " + QString::number(result.grade) + "/100\n";
    report += "Calories packed: " + QString::number(result.totalCalories) + "\n";
    report += "Hydration packed: " + QString::number(result.effectiveHydration) + " oz\n";
    return report;
}

QString LevelReportBuilder::buildPassReport()
{
    QString report;
    report += "Result: Pass\n";
    report += "You packed enough resources for this level.\n";
    return report;
}

QString LevelReportBuilder::buildFailReport(const LevelResult& result)
{
    QString report;
    report += "Result: Fail\n";

    if (result.totalCalories < result.requiredCalories)
        report += "You need more energy.\n";

    if (result.effectiveHydration < result.requiredHydration)
        report += "You need more hydration.\n";

    if (result.totalCaffeine > 150)
        report += "High caffeine reduces hydration effectiveness.\n";

    if (result.totalAlcohol > 0.0)
        report += "Alcohol reduces hydration effectiveness.\n";

    if (result.totalSugar > 50.0)
        report += "Too much sugar can make your food and drink choices less effective.\n";

    return report;
}
