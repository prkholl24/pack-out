/**
 * @file level.h
 * @brief Header file for the level class
 */
#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <item.h>
#include <QObject>

class Level : public QObject
{
    Q_OBJECT
public:
    Level();
    /**
     * @brief Level constructor sets default params
     * @param calories
     * @param water
     * @param duration
     */
    Level(int calories, double water, int duration);
    /**
     * @brief Grades prepared-ness of the backpack
     *
     *
     *
     * @return A grade of your bag out of 100%
     */
    virtual double gradeMyBag(QObject parent, std::vector<PackableItem> backpack);
private:
    /**
     * @brief The number of calories that is required to complete the trip.
     */
    int requiredCalories = -1;
    /**
     * @brief The number of ounces required to complete the trip.
     */
    double requiredWater = -1;
    /**
     * @brief The duration of the trip, measured in hours.
     */
    int duration = -1;
    /**
     * @brief Describes the level.
     */
    QString description;
    /**
     * @brief Report is a description on how well the user did for this.
     */
    QString report;
signals:
    /**
     * @brief reportReady is emitted when a report (described above) is generated.
     */
    void reportReady(QString report);

};

#endif // LEVEL_H
