/**
 * @file levelreportbuilder.h
 * @brief Contains the LevelReportBuilder class used to generate reports.
 */

#ifndef LEVELREPORTBUILDER_H
#define LEVELREPORTBUILDER_H

#include <QString>
#include <map>
#include <QJsonObject>
#include "levelresult.h"

/**
 * @brief Stores the textual content for an educational lesson.
 */
struct LessonContent {
    QString title;
    QString body;
};

/**
 * @brief Responsible for formatting evaluation results into a human-readable report.
 * 
 * Combines summary data, pass/fail narratives, and educational science tips.
 */
class LevelReportBuilder
{
public:
    /**
     * @brief Builds the full report for a level result.
     * @param result - The data container from the Level orchestrator.
     * @return A formatted report string.
     */
    static QString buildReport(const LevelResult& result);

    /**
     * @brief Fetches a specific educational lesson by its string key.
     * @param key The key string (e.g., "COTTON_CLOTHING_WARNING").
     * @return Formatted lesson string with title and body.
     */
    static QString getLessonByKey(const QString& key);

private:
    /**
     * @brief Ensures the lesson registry and feedback data are loaded from JSON.
     */
    static void ensureContentLoaded();

    /**
     * @brief A registry mapping ScienceLesson codes to their educational content.
     */
    static std::map<ScienceLesson, LessonContent> lessonRegistry;

    /**
     * @brief Stores global feedback strings (intro, pass/fail narratives).
     */
    static QJsonObject feedbackData;

    // Report component builders
    static QString buildSummaryReport(const LevelResult& result);
    static QString buildPassReport();
    static QString buildFailReport(const LevelResult& result);
    static QString buildScienceTip(const LevelResult& result);
};

#endif // LEVELREPORTBUILDER_H
