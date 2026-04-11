/**
 * @file levelreportbuilder.cpp
 * @brief Implementation of the LevelReportBuilder class.
 */

#include "levelreportbuilder.h"

/**
 * @brief Registry initialization with educational content.
 */
const std::map<ScienceLesson, LessonContent> LevelReportBuilder::LessonRegistry = {
    { ScienceLesson::BMR_BASE_METABOLISM, 
      { "Base Metabolism", "Your body burns calories just to keep your heart beating and brain running. Never forget to pack for your resting needs!" }},
    { ScienceLesson::PANDOLF_WEIGHT_PENALTY, 
      { "The Pandolf Penalty", "The Pandolf Equation shows that carrying a pack increases effort exponentially. A heavy pack can feel twice as heavy on steep terrain!" }},
    { ScienceLesson::HYDRATION_DIURETICS, 
      { "Hydration Interference", "Diuretics like caffeine and alcohol tell your kidneys to release water. They can effectively 'cancel out' your hydration!" }},
    { ScienceLesson::COTTON_CLOTHING_WARNING, 
      { "Cotton Kills", "Cotton absorbs water and loses all insulation value when wet. In rain or snow, it leads to hypothermia. Choose Wool or Synthetic!" }},
    { ScienceLesson::FIBER_ENERGY_STABILITY, 
      { "Energy Stability", "High sugar without fiber causes insulin crashes. According to NutritionFacts.org, fiber-rich whole foods provide the most stable fuel." }},
    { ScienceLesson::SODIUM_HYDRATION_LOAD, 
      { "The Salt Trap", "Excessive sodium (>2300mg) increases thirst and strains kidneys. Choose lower-sodium whole foods for optimal hydration." }},
    { ScienceLesson::WHOLE_FOOD_FUEL, 
      { "Whole Food Power", "Minimally processed plant foods are nutrient-dense and high in antioxidants, helping your muscles recover faster from the trail!" }},
    { ScienceLesson::PACK_WEIGHT_RATIO, 
      { "The 20% Rule", "Try to keep your total pack weight under 20% of your body weight. This reduces the mechanical strain on your joints." }}
};

QString LevelReportBuilder::buildReport(const LevelResult& result)
{
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
    auto it = LessonRegistry.find(result.lessonCode);
    if (it == LessonRegistry.end())
        return "💡 **Science Tip:** Proper preparation is the key to every successful trek!";

    return "💡 **Science Tip: " + it->second.title + "**\n" + it->second.body;
}

QString LevelReportBuilder::buildSummaryReport(const LevelResult& result)
{
    QString report;
    report += "Backpack Report\n";
    report += "Grade: " + QString::number(result.grade, 'f', 1) + "/100\n";
    report += "Weight: " + QString::number(result.totalWeight, 'f', 1) + " lbs\n";
    report += "Calories packed: " + QString::number(result.totalCalories, 'f', 0) + "\n";
    report += "Hydration packed: " + QString::number(result.effectiveHydration, 'f', 1) + " oz\n\n";

    report += "Trip Science (Energy Expenditure):\n";
    report += "- Base Metabolism: " + QString::number(result.bmrCost, 'f', 0) + " kcal\n";
    report += "- Distance Burn: " + QString::number(result.horizontalBurn, 'f', 0) + " kcal\n";
    report += "- Elevation Burn: " + QString::number(result.verticalBurn, 'f', 0) + " kcal\n";
    if (result.packWeightPenalty > 5.0) {
        report += "- Pack Load Penalty: " + QString::number(result.packWeightPenalty, 'f', 0) + " kcal\n";
    }
    report += "**Total Need: " + QString::number(result.requiredCalories) + " kcal**\n";
    
    return report;
}

QString LevelReportBuilder::buildPassReport()
{
    QString report;
    report += "Trip Outcome: Victory\n";
    report += "The sun sets behind the ridge as you reach your final camp. Because you packed well, your energy levels stayed high throughout the climb. You successfully completed the trek and learned how your body handles the trail!\n";
    return report;
}

QString LevelReportBuilder::buildFailReport(const LevelResult& result)
{
    QString report;
    report += "Trip Outcome: Exhaustion\n";
    report += "As the trail got steeper, the reality of your planning set in. ";

    if (result.isStarving)
        report += "Without enough calories, your muscles began to fatigue halfway through the distance. ";

    if (result.isDehydrated)
        report += "Severe dehydration left you feeling lightheaded and unable to focus on the terrain. ";

    if (result.hasMissingGear)
        report += "Missing critical safety gear forced you to turn back before things got dangerous. ";

    if (result.isTooHeavy)
        report += "The excessive weight of your pack made every step a struggle, quickly draining your reserves. ";

    report += "\n\nYou were forced to call off the trip. Use the 'Trip Science' breakdown below to improve your pack for the next attempt.\n";

    return report;
}
