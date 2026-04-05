/**
 * @file wateritem.cpp
 * @brief Implementation of the WaterItem class
 */
#include "wateritem.h"

WaterItem::WaterItem(Water waterEnum) {
    WaterStats waterStruct = WaterRegistry.at(waterEnum);

    this->weight      = waterStruct.weight;
    this->capacity    = waterStruct.capacity;
    this->description = &waterStruct.description;
}

std::map<QString, double> WaterItem::getStats() {
    std::map<QString, double> output;

    output.at(QString("Capacity(L)")) = this->capacity;
    output.at(QString("Weight(lbs)")) = this->weight;

    return output;
}
