QT += core
CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = ModelTest
TEMPLATE = app

SOURCES += \
    model_test.cpp \
    level.cpp \
    player.cpp \
    item.cpp \
    fooditem.cpp \
    drinkitem.cpp \
    gearitem.cpp \
    trekphysiology.cpp \
    gearcheck.cpp \
    trekrules.cpp

HEADERS += \
    level.h \
    player.h \
    item.h \
    fooditem.h \
    drinkitem.h \
    gearitem.h \
    levelresult.h \
    tripconditions.h \
    nutritionprofile.h \
    trekphysiology.h \
    gearcheck.h \
    trekrules.h

RESOURCES += \
    resources.qrc
