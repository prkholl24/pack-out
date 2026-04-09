#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "level.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    /**
     * @brief Intro level focused mainly on hydration.
     *
     * This level is meant to teach the player to pack
     * enough water for a short hike.
     */
    Level levelOne;

    /**
     * @brief Second level focused on both food and hydration.
     *
     * This level is meant to teach the player to pack
     * enough calories and water for a longer outing.
     */
    Level levelTwo;
};
#endif // MAINWINDOW_H
