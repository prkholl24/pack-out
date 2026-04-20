/**
 * @file debriefview.cpp
 * @brief Implementation of the DebriefView widget.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison, Claude Opus 4.5
 * @date 4/10/2026
 */
#include "debriefview.h"
#include "ui_debriefview.h"
#include "levelreportbuilder.h"
#include "theme.h"

#include <QLinearGradient>
#include <QPainter>
#include <QtGlobal>

DebriefView::DebriefView(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DebriefView)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);

    connect(ui->retryButton,     &QPushButton::clicked, this, &DebriefView::retryLevelClicked);
    connect(ui->nextLevelButton, &QPushButton::clicked, this, &DebriefView::nextLevelClicked);
    connect(ui->menuButton,      &QPushButton::clicked, this, &DebriefView::returnToMenuClicked);
}

DebriefView::~DebriefView() {
    delete ui;
}

void DebriefView::setResults(const LevelResult& result) {
    // Scoring text is passed through verbatim from the evaluation engine.
    ui->gradeLabel->setText(QString::number(result.grade, 'f', 1));
    ui->gradeProgressBar->setValue(qRound(result.grade));

    lastPassed = result.passed;
    ui->statusLabel->setText(result.passed ? "PASSED" : "FAILED");
    refreshStatusColor();

    ui->feedbackTextEdit->setText(LevelReportBuilder::buildReport(result));
}

void DebriefView::applyTheme(const LevelTheme& theme) {
    themePrimary   = theme.primary;
    themeSecondary = theme.secondary;
    themeOnPrim    = theme.textOnPrimary;
    themeOnSurface = theme.textOnSurface;

    const QString primaryName   = themePrimary.name();
    const QString secondaryName = themeSecondary.name();
    const QString onPrim        = themeOnPrim.name();
    const QString onSurface     = themeOnSurface.name();

    ui->titleLabel->setStyleSheet(QString("color: %1; letter-spacing: 6px;").arg(onPrim));

    ui->scoreCard->setStyleSheet(QString(
        "QWidget#scoreCard { background: %1; border: 3px solid %2; "
        "border-radius: 24px; }"
        "QLabel { background: transparent; color: %3; }"
        ).arg(themeSecondary.lighter(125).name(),
              primaryName,
              onSurface));
    ui->gradeTitleLabel->setStyleSheet(
        QString("color: %1; letter-spacing: 6px;").arg(primaryName));
    ui->gradeLabel->setStyleSheet(QString("color: %1;").arg(primaryName));

    ui->gradeProgressBar->setStyleSheet(QString(
        "QProgressBar { background: %1; border: 2px solid %2; border-radius: 18px; }"
        "QProgressBar::chunk { background: %2; border-radius: 16px; }"
        ).arg(themeSecondary.darker(110).name(), primaryName));

    ui->feedbackTitleLabel->setStyleSheet(QString("color: %1;").arg(onPrim));
    ui->feedbackTextEdit->setStyleSheet(QString(
        "QTextEdit { background: #FFFDF8; color: #2A2A2A; "
        "border: 3px solid %1; border-radius: 18px; padding: 28px; }"
        ).arg(primaryName));

    const QString buttonStyle = QString(
        "QPushButton { background: %1; color: %2; border: none; "
        "border-radius: 40px; padding: 16px 40px; font-weight: 700; }"
        "QPushButton:hover { background: %3; }"
        "QPushButton:pressed { background: %4; }"
        ).arg(primaryName, onPrim,
              themePrimary.lighter(115).name(),
              themePrimary.darker(115).name());
    ui->retryButton->setStyleSheet(buttonStyle);
    ui->nextLevelButton->setStyleSheet(buttonStyle);

    // Menu button — softer, secondary-colored
    ui->menuButton->setStyleSheet(QString(
        "QPushButton { background: transparent; color: %1; "
        "border: 3px solid %1; border-radius: 40px; padding: 13px 36px; "
        "font-weight: 700; }"
        "QPushButton:hover { background: %1; color: %2; }"
        ).arg(onPrim, primaryName));

    refreshStatusColor();
    update();
}

void DebriefView::refreshStatusColor() {
    const QString color = lastPassed ? "#2F8F4A" : "#C4392B";
    ui->statusLabel->setStyleSheet(QString("color: %1; letter-spacing: 8px;").arg(color));
}

void DebriefView::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Body surface.
    p.fillRect(rect(), QColor("#F4EFE4"));

    // Primary-color header band behind the title.
    QLinearGradient headerGrad(0, 0, 0, 170);
    headerGrad.setColorAt(0.0, themePrimary.lighter(108));
    headerGrad.setColorAt(1.0, themePrimary.darker(108));
    p.fillRect(QRect(0, 0, width(), 170), headerGrad);
    p.fillRect(QRect(0, 170, width(), 6), themeSecondary);

    // Primary-color band behind the feedback title + button bar.
    p.fillRect(QRect(0, 990, width(), 90),
               themePrimary.darker(108));
}
