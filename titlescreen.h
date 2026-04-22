/**
 * @file titlescreen.h
 * @brief Main-menu title card shown at launch and after "Menu" on debrief.
 */
#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class TitleScreen; }
QT_END_NAMESPACE

struct LevelTheme;

class TitleScreen : public QWidget {
    Q_OBJECT
public:
    explicit TitleScreen(QWidget* parent = nullptr);
    ~TitleScreen() override;

    /// Applies a theme to the screen's colors and background image.
    void applyTheme(const LevelTheme& theme);

signals:
    /// Emitted when the player clicks "Play".
    void playClicked();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::TitleScreen* ui;
    QColor bgTop    = QColor("#1F3A2E");
    QColor bgBottom = QColor("#0F1D17");
    QPixmap heroImage;
};

#endif // TITLESCREEN_H
