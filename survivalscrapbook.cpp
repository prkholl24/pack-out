#include "survivalscrapbook.h"

// The use of Claude AI was utilized to generate the visual for this Widget.
SurvivalScrapbook::SurvivalScrapbook(QWidget* parent) : QWidget(parent)
{
    pages = { titlePage, page1 };
    currentPage = 0;

    leftButton  = new QPushButton("◀", this);
    rightButton = new QPushButton("▶", this);
    textDisplay = new QLabel(pages[0], this);

    textDisplay->setAlignment(Qt::AlignCenter);
    textDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(leftButton);
    layout->addWidget(textDisplay);
    layout->addWidget(rightButton);
    setLayout(layout);

    leftButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    rightButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    leftButton->setFlat(true);   // removes the button border/frame
    rightButton->setFlat(true);

    leftButton->setFixedWidth(40);
    rightButton->setFixedWidth(40);

    textDisplay->setWordWrap(true);
    textDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect( leftButton,  &QPushButton::clicked, this, &SurvivalScrapbook::onLeftClicked );
    connect( rightButton, &QPushButton::clicked, this, &SurvivalScrapbook::onRightClicked );

    this->setStyleSheet("border: 1px solid black; border-radius: 8px;");

    setFixedSize(400, 340);
}

void SurvivalScrapbook::setText( const QString &text )
{
    textDisplay->setText( text );
}

void SurvivalScrapbook::onLeftClicked()
{
    if ( currentPage > 0)
    {
        setText( pages.at( --currentPage ) );
    }
    else
    {
        return;
    }
}

void SurvivalScrapbook::onRightClicked()
{
    if (currentPage < (int) pages.size()-1) {
        setText( pages[ ++currentPage ] );
    }
    else
    {
        return;
    }
}

void SurvivalScrapbook::toggleVisibility()
{
    setVisible(!isVisible());
}
