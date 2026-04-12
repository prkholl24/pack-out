#include "trekview.h"
#include "ui_trekview.h"
#include <QPainter>

TrekView::TrekView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TrekView)
{
    ui->setupUi(parent);
    connect(phys,&TrekBox2DPhysics::sendBodyLocation,this,&TrekView::bodyDisplay);
    playerLevel = 1;

    imageFilePaths.append("://DryCanyonTrail.png");

    backgroundPixmap.load(imageFilePaths[0]);

}

TrekView::~TrekView()
{
    delete ui;
}

void TrekView::currentLevel(int level){
    playerLevel = level;
}

void TrekView::bodyDisplay(float x, float y){
    boxX = x;
    boxY = y;
}




void TrekView::paintEvent(QPaintEvent *event){
    QPainter painter(this);



    painter.drawPixmap(rect(), backgroundPixmap);

    painter.setBrush(Qt::blue);

    painter.drawEllipse(boxX,boxY,100,100);

    update();
}
