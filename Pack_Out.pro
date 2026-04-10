QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    fooditem.cpp \
    item.cpp \
    level.cpp \
    levelreportbuilder.cpp \
    main.cpp \
    mainwindow.cpp \
    nutritionprofile.cpp \
    player.cpp \
    wateritem.cpp

HEADERS += \
    fooditem.h \
    item.h \
    level.h \
    levelreportbuilder.h \
    levelresult.h \
    mainwindow.h \
    nutritionprofile.h \
    player.h \
    wateritem.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    ../../../../../Downloads/banana2.png \
    ../../../../../Downloads/potato2.png \
    ../../../../../Downloads/waterbottle2.png
