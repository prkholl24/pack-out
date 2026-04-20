/**
 * @file titlescreen.cpp
 * @brief Implementation of TitleScreen.
 */
#include "titlescreen.h"
#include "ui_titlescreen.h"
#include "theme.h"

#include <QLinearGradient>
#include <QPainter>

TitleScreen::TitleScreen(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TitleScreen)
{
    ui->setupUi(this);
    connect(ui->playButton, &QPushButton::clicked, this, &TitleScreen::playClicked);
}

TitleScreen::~TitleScreen() {
    delete ui;
}

void TitleScreen::applyTheme(const LevelTheme& theme) {
    bgTop    = theme.primary.lighter(115);
    bgBottom = theme.primary.darker(135);

    heroImage = QPixmap();
    if (!theme.imagePath.isEmpty())
        heroImage.load(theme.imagePath);

    const QString fg  = theme.textOnPrimary.name();
    const QString sec = theme.secondary.name();

    ui->titleLabel->setStyleSheet(
        QString("background: transparent; color: %1; letter-spacing: 4px;").arg(fg));
    ui->tagLineLabel->setStyleSheet(
        QString("background: transparent; color: %1;").arg(sec));
    ui->introTextEdit->setStyleSheet(QString(
        "QTextEdit { background: rgba(0,0,0,110); color: %1; "
        "border: 2px solid %2; border-radius: 16px; padding: 28px; }"
        ).arg(fg, sec));
    ui->playButton->setStyleSheet(QString(
        "QPushButton { background: %2; color: %3; border: none; "
        "border-radius: 44px; padding: 18px 56px; font-weight: 700; }"
        "QPushButton:hover { background: %4; }"
        "QPushButton:pressed { background: %5; }"
        ).arg(
            fg,
            sec,
            theme.primary.name(),
            theme.secondary.lighter(110).name(),
            theme.secondary.darker(110).name()));

    update();
}

void TitleScreen::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Vertical gradient wash for the full canvas.
    QLinearGradient grad(0, 0, 0, height());
    grad.setColorAt(0.0, bgTop);
    grad.setColorAt(1.0, bgBottom);
    p.fillRect(rect(), grad);

    // Hero image strip across the top — subtle, overlaid by the gradient.
    if (!heroImage.isNull()) {
        QPixmap scaled = heroImage.scaled(
            width(), height(),
            Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation);
        p.setOpacity(0.40);
        const int x = (width() - scaled.width()) / 2;
        const int y = (height() - scaled.height()) / 2;
        p.drawPixmap(x, y, scaled);
        p.setOpacity(1.0);

        // Dark vignette to ensure text legibility.
        QLinearGradient vg(0, 0, 0, height());
        vg.setColorAt(0.0, QColor(0, 0, 0, 90));
        vg.setColorAt(0.5, QColor(0, 0, 0, 20));
        vg.setColorAt(1.0, QColor(0, 0, 0, 170));
        p.fillRect(rect(), vg);
    }
}
