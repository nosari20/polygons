#include "visualizer.h"

#include <QPolygonF>
#include <QPointF>
#include <QGraphicsLineItem>

#include <QDebug>


#include <math.h>

#include <QDebug>

Visualizer::Visualizer() : QGraphicsView()
{
    scene = new QGraphicsScene();
    this->setScene(scene);

    ppux = 1;
    ppuy = 1;
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

Visualizer::~Visualizer()
{
    delete scene;
}

void Visualizer::set_ppux(int const x){
    ppux = x;
}

void Visualizer::set_ppuy(int const y){
    ppuy = y;
}


void Visualizer::showPolygon(const QPolygon polygon)
{
    scene->addPolygon(polygon);
}

void Visualizer::showPolygon(const QPolygonF polygon)
{
    scene->addPolygon(polygon);
}

void Visualizer::showPolyline(const QList<QPointF> polyline)
{
    QPolygonF polygon;

    for(int i=0;i<polyline.size()-1;i++){
        //polygon->append(QPointF(polyline->at(i)->x()*ppux, polyline->at(i)->y()*ppuy));

<<<<<<< HEAD

        scene->addLine(QLine(polyline.at(i).x(), polyline.at(i).y(), polyline.at(i+1).x(), polyline.at(i+1).y()));
=======
    for(int i=0;i<polyline->size()-1;i++){
        //polygon->append(QPointF(polyline->at(i)->x()*ppux, polyline->at(i)->y()*ppuy));


        scene->addLine(polyline->at(i)->x(), polyline->at(i)->y(),polyline->at(i+1)->x(), polyline->at(i+1)->y());
>>>>>>> cf4aae88d48af7ccb954e5570d10d6d723f662ee
        //qDebug() << polyline->at(i)->x() << " " << polyline->at(i)->y();
    }



    scene->addPolygon(polygon);
}

void Visualizer::clear(){
    this->repaint();

    scene->clear();

}
