#include "bigpic.h"
#include "ui_bigpic.h"
 #include <QWheelEvent>

BigPic::BigPic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BigPic)
{
    ui->setupUi(this);
}

BigPic::BigPic(QGraphicsScene *sc, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BigPic)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(sc);
}

BigPic::~BigPic()
{
    delete ui;
}

void BigPic::wheelEvent(QWheelEvent *event){

        ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        // Scale the view / do the zoom
        double scaleFactor = 1.15;
        if(event->delta() > 0) {
            // Zoom in
            ui->graphicsView-> scale(scaleFactor, scaleFactor);

        } else {
            // Zooming out
             ui->graphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }


        //ui->graphicsView->setTransform(QTransform(h11, h12, h21, h22, 0, 0));
}

void BigPic::setSc(QGraphicsScene *sc){
    this->ui->graphicsView->setScene(sc);
}
