/**
 * @file level.cpp
 * @brief Implementation of the Level class
 *
 * Implements the Level model, including grading logic,
 * pass/fail evaluation, and feedback generation for
 * backpack preparedness.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 * Reviewed by Charles Adair
 */

#include "level.h"

Level::Level()
    : requiredCalories(-1), requiredWater(-1), duration(-1), description("")
{
}

Level::Level(int calories, double water, int duration)
    : requiredCalories(calories), requiredWater(water), duration(duration), description("")
{
}

int Level::getRequiredCalories() const
{
    return requiredCalories;
}

double Level::getRequiredWater() const
{
    return requiredWater;
}

int Level::getDuration() const
{
    return duration;
}

QString Level::getDescription() const
{
    return description;
}

double Level::gradeMyBag(const std::vector<Item*>& backpack) const
{
    double totalCalories = calculateTotalCalories(backpack);
    double totalWater = calculateTotalWater(backpack);

    double calorieScore = calculateCalorieScore(totalCalories);
    double waterScore = calculateWaterScore(totalWater);

    return calorieScore + waterScore;
}

bool Level::passesLevel(const std::vector<Item*>& backpack) const
{
    return gradeMyBag(backpack) >= passingScore;
}

QString Level::getReport(const std::vector<Item*>& backpack) const
{
    double totalCalories = calculateTotalCalories(backpack);
    double totalWater = calculateTotalWater(backpack);
    double grade = gradeMyBag(backpack);

    QString report = buildSummaryReport(grade, totalCalories, totalWater);

    if (grade >= passingScore)
        report += buildPassReport();
    else
        report += buildFailReport(totalCalories, totalWater);

    return report;
}

// ---------- Helper Methods ----------
// --- Data collection ---

double Level::calculateTotalCalories(const std::vector<Item*>& backpack) const
{
    double totalCalories = 0.0;

    for (const Item* item : backpack)
    {
        if (item == nullptr)
            continue;

        std::map<QString, double> stats = item->getStats();

        if (stats.count("Calories") > 0)
            totalCalories += stats.at("Calories");
    }

    return totalCalories;
}

double Level::calculateTotalWater(const std::vector<Item*>& backpack) const
{
    double totalWater = 0.0;

    for (const Item* item : backpack)
    {
        if (item == nullptr)
            continue;

        std::map<QString, double> stats = item->getStats();

        if (stats.count("Water(oz)") > 0)
            totalWater += stats.at("Water(oz)");
    }

    return totalWater;
}

// --- Scoring ---

double Level::calculateCalorieScore(double totalCalories) const
{
    if (requiredCalories <= 0)
        return 0.0;

    double score = (totalCalories / requiredCalories) * 50.0;

    if (score > 50.0)
        score = 50.0;

    return score;
}

double Level::calculateWaterScore(double totalWater) const
{
    if (requiredWater <= 0)
        return 0.0;

    double score = (totalWater / requiredWater) * 50.0;

    if (score > 50.0)
        score = 50.0;

    return score;
}

// --- Report building ---

QString Level::buildSummaryReport(double grade, double totalCalories, double totalWater) const
{
    QString report;
    report += "Backpack Report\n";
    report += "Grade: " + QString::number(grade) + "/100\n";
    report += "Calories packed: " + QString::number(totalCalories) + "\n";
    report += "Water packed: " + QString::number(totalWater) + "\n";
    return report;
}

QString Level::buildFailReport(double totalCalories, double totalWater) const
{
    QString report;
    report += "Result: Fail\n";

    if (totalCalories < requiredCalories)
        report += "- You need more calories.\n";

    if (totalWater < requiredWater)
        report += "- You need more water.\n";

    return report;
}

QString Level::buildPassReport() const
{
    QString report;
    report += "Result: Pass\n";
    report += "You packed enough resources for this level.\n";
    return report;
}
