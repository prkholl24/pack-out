/**
 * @file levelreportbuilder.cpp
 * @brief Implementation of the LevelReportBuilder class.
 */

#include "levelreportbuilder.h"
#include "trekrules.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

// Initialize static members
std::map<ScienceLesson, LessonContent> LevelReportBuilder::lessonRegistry;
QJsonObject LevelReportBuilder::feedbackData;

void LevelReportBuilder::ensureContentLoaded()
{
    if (!lessonRegistry.empty()) return;

    // 1. Load Lessons from JSON
    static const std::map<QString, ScienceLesson> keyToEnum = {
        { "BMR_BASE_METABOLISM",      ScienceLesson::BMR_BASE_METABOLISM },
        { "PANDOLF_WEIGHT_PENALTY",   ScienceLesson::PANDOLF_WEIGHT_PENALTY },
        { "HYDRATION_DIURETICS",      ScienceLesson::HYDRATION_DIURETICS },
        { "COTTON_CLOTHING_WARNING",  ScienceLesson::COTTON_CLOTHING_WARNING },
        { "FIBER_ENERGY_STABILITY",   ScienceLesson::FIBER_ENERGY_STABILITY },
        { "SODIUM_HYDRATION_LOAD",    ScienceLesson::SODIUM_HYDRATION_LOAD },
        { "WHOLE_FOOD_FUEL",          ScienceLesson::WHOLE_FOOD_FUEL },
        { "PACK_WEIGHT_RATIO",        ScienceLesson::PACK_WEIGHT_RATIO },
        { "LEAVE_NO_TRACE_VIOLATION", ScienceLesson::LEAVE_NO_TRACE_VIOLATION },
        { "VOLUME_OVERFLOW",          ScienceLesson::VOLUME_OVERFLOW },
        { "TEN_ESSENTIALS_MISSING",   ScienceLesson::TEN_ESSENTIALS_MISSING }
    };

    QFile lessonFile(":/data/lessons.json");
    if (lessonFile.open(QIODevice::ReadOnly)) {
        QJsonObject root = QJsonDocument::fromJson(lessonFile.readAll()).object();
        for (auto it = keyToEnum.begin(); it != keyToEnum.end(); ++it) {
            if (root.contains(it->first)) {
                QJsonObject obj = root.value(it->first).toObject();
                lessonRegistry[it->second] = { obj.value("title").toString(), obj.value("body").toString() };
            }
        }
    }

    // 2. Load Feedback Narratives from JSON
    QFile feedbackFile(":/data/feedback.json");
    if (feedbackFile.open(QIODevice::ReadOnly)) {
        feedbackData = QJsonDocument::fromJson(feedbackFile.readAll()).object();
    }
}

QString LevelReportBuilder::buildReport(const LevelResult& result)
{
    ensureContentLoaded();
    QString report = buildSummaryReport(result);

    if (result.passed)
        report += buildPassReport();
    else
        report += buildFailReport(result);

    report += "\n" + buildScienceTip(result);
    return report;
}

QString LevelReportBuilder::buildScienceTip(const LevelResult& result)
{
    auto it = lessonRegistry.find(result.lessonCode);
    if (it == lessonRegistry.end())
        return "💡 **Science Tip:** Proper preparation is the key to every successful trek!";

    return "💡 **Science Tip: " + it->second.title + "**\n" + it->second.body;
}

QString LevelReportBuilder::buildSummaryReport(const LevelResult& result)
{
    QString report;
    report += "Backpack Report\n";
    report += "Grade: " + QString::number(result.grade, 'f', 1) + "/100\n";
    report += "Weight: " + QString::number(result.totals.weightLbs, 'f', 1) + " lbs\n";
    report += "Volume: " + QString::number(result.totals.volume, 'f', 1) + " / " + QString::number(result.maxVolume, 'f', 1) + " L\n";
    report += "Calories packed: " + QString::number(result.totals.calories, 'f', 0) + "\n";
    report += "Hydration packed: " + QString::number(result.totals.effectiveHydration, 'f', 1) + " oz\n\n";

    report += "Trip Science (Energy Expenditure):\n";
    report += "- Base Metabolism: " + QString::number(result.bmrBurn, 'f', 0) + " kcal\n";
    report += "- Distance Burn: " + QString::number(result.distanceBurn, 'f', 0) + " kcal\n";
    report += "- Elevation Burn: " + QString::number(result.climbBurn, 'f', 0) + " kcal\n";
    
    double threshold = TrekRules::get("health_thresholds", "load_penalty_display_threshold");
    if (result.loadPenalty > threshold) {
        report += "- Pack Load Penalty: " + QString::number(result.loadPenalty, 'f', 0) + " kcal\n";
    }
    report += "**Total Need: " + QString::number(result.requiredCalories) + " kcal**\n";
    return report;
}

QString LevelReportBuilder::buildPassReport()
{
    QString report;
    report += "Trip Outcome: Victory\n";
    report += feedbackData.value("pass_body").toString() + "\n";
    return report;
}

QString LevelReportBuilder::buildFailReport(const LevelResult& result)
{
    // Mapping from Model Enums to JSON Feedback Keys
    static const std::map<TripCondition, QString> conditionToKey = {
        { TripCondition::STARVING,           "starving" },
        { TripCondition::DEHYDRATED,         "dehydrated" },
        { TripCondition::TOO_HEAVY,          "too_heavy" },
        { TripCondition::TOO_BULKY,          "too_bulky" },
        { TripCondition::HIGH_CAFFEINE,      "high_caffeine" },
        { TripCondition::HIGH_SUGAR,         "high_sugar" },
        { TripCondition::ALCOHOL_PENALTY,    "alcohol_penalty" },
        { TripCondition::MISSING_GEAR,       "missing_gear" },
        { TripCondition::MISSING_ESSENTIALS, "missing_essentials" },
        { TripCondition::LNT_VIOLATION,      "lnt_violation" }
    };

    QString report;
    report += "Trip Outcome: Exhaustion\n";
    report += feedbackData.value("fail_intro").toString();

    QJsonObject scenarios = feedbackData.value("failure_scenarios").toObject();
    
    // Iterate through conditions identified by the Model
    for (TripCondition condition : result.conditions) {
        if (conditionToKey.count(condition)) {
            QString jsonKey = conditionToKey.at(condition);
            if (scenarios.contains(jsonKey)) {
                report += scenarios.value(jsonKey).toString();
            }
        }
    }

    report += feedbackData.value("fail_footer").toString();
    return report;
}
