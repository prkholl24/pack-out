#include "trekview.h"
#include "qevent.h"
#include "ui_trekview.h"
#include <QPainter>
#include <iostream>

TrekView::TrekView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TrekView)
{
    ui->setupUi(parent);
    setMouseTracking(true);
   // setAttribute(Qt::WA_Hover, true);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    //Connects the signal, sendBodyLocation from physics to slot
    //bodyDisplay in trekView
    connect(phys,&TrekBox2DPhysics::sendBodyLocation,this,&TrekView::bodyDisplay);

    connect(phys, &TrekBox2DPhysics::loadVisualObjectsSignal, this, &TrekView::loadVisualObjectsSlot);
    //Connects   send level from physics to current level in view. Updates current level
    connect(phys, &TrekBox2DPhysics::sendCurrentLevelSignal, this, &TrekView::currentLevel);

    //Connects getColor to openColorSelectorSlot in the view
    connect(phys, &TrekBox2DPhysics::getColorSignal, this, &TrekView::openColorSelectorSlot);

    connect(phys, &TrekBox2DPhysics::moveToNextFrame, this, &TrekView::nextSceneSlot);

    //Connects updateMousePosition in view, to setMousePosition in physics
    connect(this, &TrekView::updatePhysicsMousePosition, phys, &TrekBox2DPhysics::setTargetFromMouse);
    //Connects: Sends key pressed to slot worldEditorSlot in TrekBox2DPhysics
    connect(this,&TrekView::sendKeyPressedSignal, phys, &TrekBox2DPhysics::worldEditorSlot);

    //Connects sendSelectedColorSignal to setColorSlot in TrekBox2DPhysics
    connect(this, &TrekView::sendSelectedColorSignal, phys, &TrekBox2DPhysics::setColorSlot);

    connect(this, &TrekView::nextLevelSignal, phys, &TrekBox2DPhysics::nextLevelSlot);



    connect(this, &TrekView::leftMouseButtonClickedSignal, phys, &TrekBox2DPhysics::leftMouseButtonClickedSlot);
    playerLevel = 1;

    imageFilePaths.append("://BryceArchesBackground.jpg");
    imageFilePaths.append("://images/FishLakeLoop.png");
    imageFilePaths.append("://images/LakeBlanche.png");
    imageFilePaths.append("://images/GlacierNP.png");
    imageFilePaths.append("://images/OlympicNP.png");

    backgroundPixmap.load(imageFilePaths[0]);

}

TrekView::~TrekView()
{
    delete ui;
}

void TrekView::currentLevel(int level){
    playerLevel = level;
    //std::cout << "Updated Level to: " << level << std::endl;

    if(imageFilePaths.length() >= playerLevel){
        backgroundPixmap.load(imageFilePaths[playerLevel - 1]);
        //std::cout << "Updated Level to: " << level << std::endl;

    }
}

void TrekView::nextLevelSlot(int level){
    emit nextLevelSignal(level);
}

void TrekView::nextSceneSlot(){
    emit nextSceneSignal();
}

void TrekView::bodyDisplay(float x, float y){
    boxX = x;
    boxY = y;
    update();
}

void TrekView::mousePressEvent(QMouseEvent *event){

    if(event ->buttons()== Qt::LeftButton){
        emit updatePhysicsMousePosition(event->position());
    }

}

void TrekView::mouseMoveEvent(QMouseEvent *event) {
    if(event ->buttons()== Qt::LeftButton){
        emit updatePhysicsMousePosition(event->position());
    }

}

void TrekView::mouseReleaseEvent(QMouseEvent *event){

    emit leftMouseButtonClickedSignal();
}

void TrekView::keyPressEvent(QKeyEvent* event){

    int key = event->key();

    emit sendKeyPressedSignal(key);
}

void TrekView::openColorSelectorSlot(){
    QColor color = QColorDialog::getColor(Qt::white, this, "Pick Color",QColorDialog::ShowAlphaChannel);


    QVector<int> colorVector = {color.red(),color.green(),color.blue(),color.alpha()};

    emit sendSelectedColorSignal(colorVector);
}

void TrekView::loadVisualObjectsSlot(QVector<PhysicsObjectDef>& objectsVector){
    visualVector.clear();
    for (PhysicsObjectDef physics : objectsVector){
        VisualObjectDef visual;
        visual.shapeType = physics.shapeType;
        visual.x = physics.x;
        visual.y = physics.y;
        visual.width = physics.width;
        visual.height = physics.height;
        visual.angle = physics.angle;
        visual.color = physics.color;
        visualVector.append(visual);
    }
    update();
}




void TrekView::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    //Paints the current background
    painter.drawPixmap(rect(), backgroundPixmap);
    painter.setBrush(Qt::blue);
    // painter.drawEllipse(boxX - 50, boxY - 50, 90, 90);
    painter.setBrush(Qt::red);

    // if (!visualVector.empty()) {
        for (VisualObjectDef visual : visualVector) {

            QColor color(visual.color[0],visual.color[1],visual.color[2]);
            painter.setBrush(color);
            //painter.setPen(Qt::NoPen);

            painter.save();

            painter.translate(visual.x, visual.y);
            //float angleDegrees = visual.angle * 180.0f / 3.14159269f;
            painter.rotate(-visual.angle);

            if(visual.shapeType == "box"){
                painter.drawRect(-visual.width * 0.5f,
                                 -visual.height * 0.5f,
                                 visual.width,
                                 visual.height);
            }else {
                painter.drawEllipse(-visual.width * 0.5f,
                                    -visual.height * 0.5f,
                                    visual.width,
                                    visual.height);

            }

            painter.restore();


        }



}
