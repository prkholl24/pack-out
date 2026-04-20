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
    themePrimary    = theme.primary;
    themeSecondary  = theme.secondary;
    themeTextOnPrim = theme.textOnPrimary;
    themeTextOnSurf = theme.textOnSurface;

    locationPixmap = QPixmap();
    if (!theme.imagePath.isEmpty())
        locationPixmap.load(theme.imagePath);

    const QString primaryName  = themePrimary.name();
    const QString secondaryName = themeSecondary.name();
    const QString onPrim       = themeTextOnPrim.name();

    // Badge and title live in the header strip (primary-colored band).
    ui->trekBadgeLabel->setStyleSheet(
        QString("background: transparent; color: %1; letter-spacing: 4px;").arg(themeSecondary.lighter(125).name()));
    ui->titleLabel->setStyleSheet(
        QString("background: transparent; color: %1;").arg(onPrim));

    // Metadata card — surface-colored panel with primary accent.
    ui->metadataCard->setStyleSheet(QString(
        "QWidget#metadataCard { background: %1; border: 2px solid %2; "
        "border-radius: 18px; }"
        "QLabel { background: transparent; color: %3; }"
        ).arg(themeSecondary.lighter(130).name(),
              primaryName,
              themeTextOnSurf.name()));

    // Titles on metadata card — softer, primary-colored
    const QString titleColor = themePrimary.darker(110).name();
    for (auto* lbl : { ui->adventureTitleLabel, ui->terrainTitleLabel,
                       ui->weatherTitleLabel, ui->milesTitleLabel,
                       ui->elevationTitleLabel, ui->durationTitleLabel }) {
        lbl->setStyleSheet(QString("color: %1; letter-spacing: 2px;").arg(titleColor));
    }

    // Right column — mission brief
    ui->briefingTitleLabel->setStyleSheet(QString("color: %1;").arg(primaryName));
    ui->briefingTextEdit->setStyleSheet(QString(
        "QTextEdit { background: #FFFDF8; color: #2A2A2A; "
        "border: 3px solid %1; border-radius: 16px; padding: 28px; }"
        ).arg(primaryName));

    // Start packing button — themed
    ui->startPackingButton->setStyleSheet(QString(
        "QPushButton { background: %1; color: %2; border: none; "
        "border-radius: 44px; padding: 18px 52px; font-weight: 700; }"
        "QPushButton:hover { background: %3; }"
        "QPushButton:pressed { background: %4; }"
        ).arg(primaryName, onPrim,
              themePrimary.lighter(115).name(),
              themePrimary.darker(115).name()));

    // Image slot placeholder styling
    ui->locationImageLabel->setStyleSheet(QString(
        "background: %1; border: 3px solid %2; border-radius: 18px;"
        ).arg(themePrimary.darker(140).name(), primaryName));

    if (!locationPixmap.isNull()) {
        ui->locationImageLabel->setPixmap(
            locationPixmap.scaled(ui->locationImageLabel->size() - QSize(12, 12),
                                  Qt::KeepAspectRatioByExpanding,
                                  Qt::SmoothTransformation));
    } else {
        ui->locationImageLabel->setText(
            QString("<span style='color:%1; font-size:20pt; letter-spacing:3px;'>"
                    "&#91; LOCATION PHOTO &#93;</span>").arg(themeSecondary.name()));
    }

    update();
}

void BriefView::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Body background — warm paper surface.
    p.fillRect(rect(), QColor("#F4EFE4"));

    // Top header strip, primary color gradient band.
    QLinearGradient grad(0, 0, 0, 180);
    grad.setColorAt(0.0, themePrimary.lighter(108));
    grad.setColorAt(1.0, themePrimary.darker(108));
    p.fillRect(QRect(0, 0, width(), 180), grad);

    // Secondary-color accent stripe under the header.
    p.fillRect(QRect(0, 180, width(), 6), themeSecondary);
}
