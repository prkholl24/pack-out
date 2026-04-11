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
    return buildOutcomeSummary(result) + "\n\n" + buildStatsReport(result);
}

QString LevelReportBuilder::buildOutcomeSummary(const LevelResult& result)
{
    QString report;

    if (result.passed)
    {
        if (result.hasMissingGear)
        {
            report += "You somehow made it through even with missing gear. ";
            report += "It was sloppy, but you still reached the end of the trip.";
            return report;
        }

        if (result.totalWeight > result.idealWeight && result.idealWeight > 0.0)
        {
            report += "You made it to the finish, but your overloaded pack slowed you down the whole way.";
            return report;
        }

        if (result.totalCaffeine > 150.0)
        {
            report += "You packed enough to survive, but way too much caffeine made the trip rougher than it needed to be.";
            return report;
        }

        if (result.totalSugar > 50.0)
        {
            report += "You crossed the finish line, but you rode a sugar rush the whole time instead of packing steady trail fuel.";
            return report;
        }

        report += "You packed smart, stayed fueled, and had what you needed to finish the trip.";
        return report;
    }

    if (result.hasMissingGear)
    {
        report += "You headed out without the right gear and paid for it out on the trail.";

        if (result.requiredHydration > 0.0 && result.effectiveHydration < result.requiredHydration)
            report += " You were already underprepared, and dehydration finished you off.";

        return report;
    }

    if (result.requiredHydration > 0.0 && result.effectiveHydration < result.requiredHydration)
    {
        if (result.requiredCalories > 0 && result.totalCalories < result.requiredCalories)
        {
            report += "You brought some supplies, but not enough food or water to make it through the trip.";
            return report;
        }

        if (result.totalCaffeine > 150.0 || result.totalAlcohol > 0.0)
        {
            report += "You thought you packed drinks, but they did not keep you hydrated enough to survive the trip.";
            return report;
        }

        report += "You brought enough food to keep moving, but not enough water and died of dehydration.";
        return report;
    }

    if (result.requiredCalories > 0 && result.totalCalories < result.requiredCalories)
    {
        report += "You had enough to drink, but you ran out of usable energy before the trip was over.";
        return report;
    }

    if (result.totalWeight > result.idealWeight && result.idealWeight > 0.0)
    {
        report += "Your bag was so overloaded that you burned out before you could finish the trip.";
        return report;
    }

    if (result.totalAlcohol > 0.0)
    {
        report += "Packing alcohol looked fun at the trailhead, but it made the trip harder and left you underprepared.";
        return report;
    }

    if (result.totalSugar > 50.0)
    {
        report += "You packed a quick sugar high instead of lasting trail fuel, and your energy crashed at the worst time.";
        return report;
    }

    if (result.totalCaffeine > 150.0)
    {
        report += "You leaned too hard on caffeine and ended up less prepared than you thought.";
        return report;
    }

    report += "You set out with a bag that looked plausible, but it was not enough to get you through the trip.";
    return report;
}

QString LevelReportBuilder::buildStatsReport(const LevelResult& result)
{
    QString report;
    report += "Trip Stats\n";
    report += "Grade: " + QString::number(result.grade, 'f', 1) + "/100\n";
    report += "Outcome: " + QString(result.passed ? "Pass" : "Fail") + "\n";
    report += "Packed weight: " + QString::number(result.totalWeight, 'f', 1) + " lbs\n";
    report += "Calories packed: " + QString::number(result.totalCalories, 'f', 0) + "\n";
    report += "Effective hydration: " + QString::number(result.effectiveHydration, 'f', 1) + " oz\n";

    if (result.requiredCalories > 0)
        report += "Calories needed: " + QString::number(result.requiredCalories) + "\n";

    if (result.requiredHydration > 0.0)
        report += "Hydration needed: " + QString::number(result.requiredHydration, 'f', 1) + " oz\n";

    if (result.idealWeight > 0.0)
        report += "Recommended pack weight: " + QString::number(result.idealWeight, 'f', 1) + " lbs\n";

    report += "Caffeine: " + QString::number(result.totalCaffeine, 'f', 0) + " mg\n";
    report += "Sugar: " + QString::number(result.totalSugar, 'f', 0) + " g\n";
    report += "Alcohol: " + QString::number(result.totalAlcohol, 'f', 1) + "\n";
    report += "Gear check: " + QString(result.hasMissingGear ? "Missing required gear" : "Required gear covered");

    return report;
}
