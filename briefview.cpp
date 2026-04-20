/**
 * @file briefview.cpp
 * @brief Implementation of the BriefView widget.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison, Claude Opus 4.5
 * @date 4/10/2026
 */
#include "briefview.h"
#include "ui_briefview.h"
#include "level.h"
#include "theme.h"
#include "tripconditions.h"

#include <QLinearGradient>
#include <QPainter>

namespace {

QString displayTerrain(TerrainType t) {
    switch (t) {
        case TerrainType::DESERT:   return "Desert";
        case TerrainType::FOREST:   return "Forest";
        case TerrainType::MOUNTAIN: return "Mountain";
        case TerrainType::ROCKY:    return "Rocky";
        default: return "Open Choice";
    }
}

QString displayWeather(WeatherType w) {
    switch (w) {
        case WeatherType::COLD:  return "Cold";
        case WeatherType::HOT:   return "Hot";
        case WeatherType::RAINY: return "Rainy";
        case WeatherType::WINDY: return "Windy";
        case WeatherType::SNOW:  return "Snow";
        default: return "Clear";
    }
}

QString displayAdventure(AdventureType a) {
    switch (a) {
        case AdventureType::BACKPACKING: return "Backpacking";
        case AdventureType::CAMPING:     return "Camping";
        case AdventureType::DAY_HIKE:    return "Day Hike";
        default: return "Open Choice";
    }
}

} // namespace

BriefView::BriefView(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::BriefView)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);

    connect(ui->startPackingButton, &QPushButton::clicked,
            this, &BriefView::startPackingClicked);
}

BriefView::~BriefView() {
    delete ui;
}

void BriefView::setLevel(const Level& level, int trekNumber) {
    ui->trekBadgeLabel->setText(QString("TREK %1").arg(trekNumber));
    ui->titleLabel->setText(level.getLocationName());

    ui->adventureLabel->setText(displayAdventure(level.getAdventure()));
    ui->terrainLabel->setText(displayTerrain(level.getTerrain()));
    ui->weatherLabel->setText(displayWeather(level.getWeather()));
    ui->milesLabel->setText(QString::number(level.getMiles(), 'g', 4) + " mi");
    ui->elevationLabel->setText(QString::number(level.getElevationGain(), 'g', 5) + " ft");
    ui->durationLabel->setText(QString::number(level.getDuration()) + " hrs");

    ui->briefingTextEdit->setText(level.getDescription());
}

void BriefView::applyTheme(const LevelTheme& theme) {
    themeAccent = theme.accent;

    locationPixmap = QPixmap();
    if (!theme.imagePath.isEmpty())
        locationPixmap.load(theme.imagePath);

    const QString ac  = themeAccent.name();

    // Header strip — level identity picks up the active theme color.
    ui->trekBadgeLabel->setStyleSheet(
        QString("background: transparent; color: %1; letter-spacing: 4px;").arg(ac));
    ui->titleLabel->setStyleSheet(
        QString("background: transparent; color: %1;").arg(ac));

    // Metadata card — white surface, accent border and category labels.
    ui->metadataCard->setStyleSheet(QString(
        "QWidget#metadataCard { background: #FFFFFF; border: 2px solid %1; "
        "border-radius: 18px; }"
        "QLabel { background: transparent; color: #2A2A2A; }"
        ).arg(ac));

    for (auto* lbl : { ui->adventureTitleLabel, ui->terrainTitleLabel,
                       ui->weatherTitleLabel, ui->milesTitleLabel,
                       ui->elevationTitleLabel, ui->durationTitleLabel }) {
        lbl->setStyleSheet(QString("color: %1; letter-spacing: 2px;").arg(ac));
    }

    // Right column — mission brief
    ui->briefingTitleLabel->setStyleSheet(QString("color: %1;").arg(ac));
    ui->briefingTextEdit->setStyleSheet(QString(
        "QTextEdit { background: #FFFDF8; color: #2A2A2A; "
        "border: 3px solid %1; border-radius: 16px; padding: 28px; }"
        ).arg(ac));

    ui->startPackingButton->setStyleSheet(QString(
        "QPushButton { background: %1; color: #FFFFFF; border: none; "
        "border-radius: 44px; padding: 18px 52px; font-weight: 700; }"
        "QPushButton:hover { background: %2; }"
        "QPushButton:pressed { background: %3; }"
        ).arg(ac,
              themeAccent.lighter(120).name(),
              themeAccent.darker(120).name()));

    ui->locationImageLabel->setStyleSheet(QString(
        "background: %1; border: 3px solid %2; border-radius: 18px;"
        ).arg(themeAccent.darker(150).name(), ac));

    if (!locationPixmap.isNull()) {
        ui->locationImageLabel->setPixmap(
            locationPixmap.scaled(ui->locationImageLabel->size() - QSize(12, 12),
                                  Qt::KeepAspectRatioByExpanding,
                                  Qt::SmoothTransformation));
    } else {
        ui->locationImageLabel->setText(
            QString("<span style='color:#FFFFFF; font-size:20pt; letter-spacing:3px;'>"
                    "&#91; LOCATION PHOTO &#93;</span>"));
    }

    update();
}

void BriefView::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), QColor("#F4EFE4"));

    QLinearGradient grad(0, 0, 0, 180);
    grad.setColorAt(0.0, themeAccent.lighter(112));
    grad.setColorAt(1.0, themeAccent.darker(112));
    p.fillRect(QRect(0, 0, width(), 180), grad);

    // Thin accent line under the header.
    p.fillRect(QRect(0, 180, width(), 4), themeAccent.darker(140));
}
