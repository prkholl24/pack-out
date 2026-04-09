/**
 * @file
 * @brief The TrekView is a MainWindow subclass to display user's current Trek/Level simulation.
 *
 *
 * Team: Green Goombas
 * GitHub: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *                  Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/08/2026
 */
#ifndef TREKVIEW_H
#define TREKVIEW_H

#include <QMainWindow>
#include <Box2D/Box2D.h>

namespace Ui {
class TrekView;
}

class TrekView : public QMainWindow
{
    Q_OBJECT

public:
    explicit TrekView(QWidget *parent = nullptr);
    ~TrekView();

private:
    /**
     * @brief ui
     */
    Ui::TrekView *ui;

    /**
     * @brief Override QT paint to display user/trek information to window.
     * @param event
     */
    // void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Box2D World
     */
    b2World world;

    /**
     * @brief body
     */
    b2Body* body;

    /**
     * @brief timer
     */
    QTimer* timer;
};

#endif // TREKVIEW_H
