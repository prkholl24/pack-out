#ifndef LEVEL_H
#define LEVEL_H

class Level
{
public:
    Level();
    Level(int calories, double water, int duration);
private:
    /**
     * @brief The number of calories that is required to complete the trip.
     */
    int requiredCalories;
    /**
     * @brief The number of ounces required to complete the trip.
     */
    double requiredWater;
    /**
     * @brief The duration of the trip, measured in hours.
     */
    int duration;
};

#endif // LEVEL_H
