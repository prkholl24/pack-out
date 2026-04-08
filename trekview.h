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
    void paintEvent(QPaintEvent *event) override;

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
