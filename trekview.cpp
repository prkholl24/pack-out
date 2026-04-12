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
   // ui->centralwidget->setStyleSheet("border-image: url("+imageFilePaths[0]+") 0 0 0 0 stretch stretch");


   // backgroundImage = QImage("://DryCanyonTrail.png");

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

    //This works, but is extremely slow
    //QPixmap background(imageFilePaths[playerLevel-1]);
    //painter.drawPixmap(rect(), background);

    painter.setBrush(Qt::blue);

    painter.drawRect(boxX,boxY,100,100);

    update();
}
