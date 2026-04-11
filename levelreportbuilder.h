/**
 * @file levelreportbuilder.h
 * @brief Contains the LevelReportBuilder class used to generate reports.
 *
 * Defines LevelReportBuilder, which is responsible for formatting
 * and generating user-facing reports based on LevelResult data.
 */

#ifndef LEVELREPORTBUILDER_H
#define LEVELREPORTBUILDER_H

#include <QString>
#include <map>
#include "levelresult.h"

/**
 * @brief Stores the textual content for an educational lesson.
 */
struct LessonContent {
    QString title;
    QString body;
};

/**
 * @brief Builds a formatted report for a LevelResult.
 */
class LevelReportBuilder
{
public:
    /**
     * @brief Builds the full report for a level result.
     */
    static QString buildReport(const LevelResult& result);

private:
    /**
     * @brief A registry mapping ScienceLesson codes to their educational content.
     */
    static const std::map<ScienceLesson, LessonContent> LessonRegistry;

    /**
     * @brief Builds the summary portion of the report.
     */
    static QString buildSummaryReport(const LevelResult& result);

    /**
     * @brief Builds the pass message.
     */
    static QString buildPassReport();

    /**
     * @brief Builds the fail message.
     */
    static QString buildFailReport(const LevelResult& result);

    /**
     * @brief Builds an educational science tip based on the lesson code.
     */
    static QString buildScienceTip(const LevelResult& result);
};

#endif // LEVELREPORTBUILDER_H
