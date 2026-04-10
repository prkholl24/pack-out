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
    report += "Grade: " + QString::number(result.grade, 'f', 1) + "/100\n";
    report += "Weight: " + QString::number(result.totalWeight, 'f', 1) + " lbs\n";
    report += "Calories packed: " + QString::number(result.totalCalories, 'f', 0) + "\n";
    report += "Hydration packed: " + QString::number(result.effectiveHydration, 'f', 1) + " oz\n";
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

    if (result.hasMissingGear)
        report += "Missing essential gear for this terrain or activity.\n";

    if (result.totalCalories < result.requiredCalories)
        report += "You need more energy.\n";

    if (result.effectiveHydration < result.requiredHydration)
        report += "You need more hydration.\n";

    if (result.totalWeight > result.idealWeight && result.idealWeight > 0.0)
        report += "Your bag is too heavy for this duration, reducing your efficiency.\n";

    if (result.totalCaffeine > 150)
        report += "High caffeine reduces hydration effectiveness.\n";

    if (result.totalAlcohol > 0.0)
        report += "Alcohol reduces hydration effectiveness.\n";

    if (result.totalSugar > 50.0)
        report += "Too much sugar can cause an energy crash on the trail.\n";

    return report;
}
