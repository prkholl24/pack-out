#include "trekview.h"
#include "qevent.h"
#include "ui_trekview.h"
#include <QPainter>

TrekView::TrekView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TrekView)
{
    ui->setupUi(parent);
    setMouseTracking(true);
   // setAttribute(Qt::WA_Hover, true);

   // setFocusPolicy(Qt::StrongFocus);

    //Connects the signal, sendBodyLocation from physics to slot
    //bodyDisplay in trekView
    connect(phys,&TrekBox2DPhysics::sendBodyLocation,this,&TrekView::bodyDisplay);

    //Connects updateMousePosition in view, to setMousePosition in physics
    connect(this, &TrekView::updatePhysicsMousePosition, phys, &TrekBox2DPhysics::setTargetFromMouse);

    connect(this, &TrekView::leftMouseButtonClickedSignal, phys, &TrekBox2DPhysics::leftMouseButtonClickedSlot);
    playerLevel = 1;

    imageFilePaths.append("://BryceArchesBackground.jpg");

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
    update();
}

void TrekView::mousePressEvent(QMouseEvent *event){

    if(event ->buttons()== Qt::LeftButton){
        emit updatePhysicsMousePosition(event->position());
    }else {
        emit leftMouseButtonClickedSignal(false);
    }

}

void TrekView::mouseMoveEvent(QMouseEvent *event) {
    if(event ->buttons()== Qt::LeftButton){
        emit updatePhysicsMousePosition(event->position());
    } else {
        leftMouseButtonClickedSignal(false);
    }

}

void TrekView::loadVisualObjectsSlot(QVector<PhysicsObjectDef>& objectsVector){


}




void TrekView::paintEvent(QPaintEvent *event){
    QPainter painter(this);



   painter.drawPixmap(rect(), backgroundPixmap);

    painter.setBrush(Qt::blue);

    painter.drawEllipse(boxX - 50, boxY - 50, 90, 90);

    painter.setBrush(Qt::red);
    painter.drawEllipse(boxX - 50, boxY - 50, 10,10);
   // painter.drawEllipse(boxX,boxY,100,100);

  //  update();
}
