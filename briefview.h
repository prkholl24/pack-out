/**
 * @file briefview.h
 * @brief Pre-level briefing widget displaying authored trip descriptions.
 *
 * BriefView shows the player information about an upcoming level
 * before they start packing. It displays an authored mission
 * description along with a few broad trip details.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison, Claude Opus 4.5
 * @date 4/10/2026
 */
#ifndef BRIEFVIEW_H
#define BRIEFVIEW_H

#include <QColor>
#include <QPixmap>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class BriefView; }
QT_END_NAMESPACE

class Level;
struct LevelTheme;

class BriefView : public QWidget {
    Q_OBJECT
public:
    explicit BriefView(QWidget* parent = nullptr);
    ~BriefView() override;

    void setLevel(const Level& level, int trekNumber);

    /// Applies per-level theme colors and location image.
    void applyTheme(const LevelTheme& theme);

signals:
    void startPackingClicked();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::BriefView* ui;
    QColor themeAccent = QColor("#3E7A4F");
    QPixmap locationPixmap;
};

#endif // BRIEFVIEW_H
