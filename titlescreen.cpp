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
    bgTop    = theme.accent.lighter(115);
    bgBottom = theme.accent.darker(135);

    heroImage = QPixmap();
    if (!theme.imagePath.isEmpty())
        heroImage.load(theme.imagePath);

    const QString ac = theme.accent.name();

    ui->titleLabel->setStyleSheet(
        QString("background: transparent; color: #FFFFFF; letter-spacing: 4px;"));
    ui->tagLineLabel->setStyleSheet(
        QString("background: transparent; color: rgba(255,255,255,180);"));
    ui->introTextEdit->setStyleSheet(QString(
        "QTextEdit { background: rgba(0,0,0,100); color: #FFFFFF; "
        "border: 2px solid rgba(255,255,255,80); border-radius: 16px; padding: 28px; }"));
    ui->playButton->setStyleSheet(QString(
        "QPushButton { background: #FFFFFF; color: %1; border: none; "
        "border-radius: 44px; padding: 18px 56px; font-weight: 700; }"
        "QPushButton:hover { background: rgba(255,255,255,220); }"
        "QPushButton:pressed { background: rgba(255,255,255,180); }"
        ).arg(ac));

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
