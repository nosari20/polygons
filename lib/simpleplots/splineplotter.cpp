/*
    QtSimplePlot a library for simple plotting
    Copyright (C) 2014 Yasunobu OKAMURA

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "splineplotter.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

#include <math.h>
#include "simpleplotcommon.h"

SPLinePlotter::SPLinePlotter(QObject *parent) :
    SPAbstractPlooter(parent), m_xlabel("X axis"), m_ylable("Y axis"), m_alpha(1), m_data()
{

}

void SPLinePlotter::clear(){
    m_data.clear();
}


void SPLinePlotter::setData(QList<QPair<QList<QPointF>,QColor>> data)
{
    m_data = data;
    setDataRange(dataRange(data), true);
}

void SPLinePlotter::setData(QList<QPointF> data,QColor color)
{

    m_data << QPair<QList<QPointF>,QColor>(data,color);
    setDataRange(dataRange(m_data), true);
}


void SPLinePlotter::setXLabel(QString xlabel)
{
    m_xlabel = xlabel;
}

void SPLinePlotter::setYLabel(QString ylabel)
{
    m_ylable = ylabel;
}

void SPLinePlotter::setAlpha(double alpha)
{
    m_alpha = alpha;
}

void SPLinePlotter::plot(QPainter &painter, QRect area) const
{
    plotGrid(painter, area);
    plotAxis(painter, area, m_xlabel, m_ylable);
    plotData(painter, area);
}

QRectF SPLinePlotter::dataRange(const QList<QPair<QList<QPointF>,QColor>> &data)
{
    QRectF range(0, 0, 1, 1);
    if (data.size() == 0)
        return range;
    if(data.at(0).first.size() == 0)
        return range;


    range.setCoords(data.at(0).first.at(0).x(), data.at(0).first.at(0).y(), data.at(0).first.at(0).x(), data.at(0).first.at(0).y());
    QPair<QList<QPointF>,QColor> pair;
    foreach (pair, data) {

        foreach (QPointF p, pair.first) {
            range.setCoords(MIN(p.x(), range.left()),
                                    MIN(p.y(), range.top()),
                                    MAX(p.x(), range.right()),
                                    MAX(p.y(), range.bottom()));
        }

    }
    return range;
}


void SPLinePlotter::plotData(QPainter &painter, QRectF area) const
{

    if(m_data.size() == 2){
        diff(m_data.at(0).first, m_data.at(1).first, painter, area);
    }


    QPair<QList<QPointF>,QColor> pair;
    foreach (pair, m_data) {
        QList<QPointF> l = pair.first;
        QColor c = pair.second;

        QPen pn;
        pn.setColor(c);
        pn.setWidth(3);
        painter.setPen(pn);

        for(int i=0;i<l.size()-1;i++){

            QPointF lt = translatePoint(l.at(i), area);
            QPointF rb = translatePoint(l.at(i+1), area);
            painter.drawLine(QLine(lt.x(), lt.y(), rb.x(), rb.y()));
        }


    }

}

void SPLinePlotter::diff(const QList<QPointF> c1, const QList<QPointF> c2, QPainter &painter, QRectF area ) const
{

    // egalize x
    int ic1 = 1;
    int ic2 = 1;

    QList<QPointF> c1o;
    QList<QPointF> c2o;

    c1o.push_back(c1.at(0));
    c2o.push_back(c2.at(0));


    int i = 0;

    QPointF p1 = c1.at(ic1);
    QPointF p2 = c2.at(ic2);

    while((ic1 < c1.size()  || ic2 < c2.size()) && (p1.x() < 1 || p2.x() < 1)){

        if(ic1 >= c1.size()){
            p1 = c1.at(c1.size()-1);
        }else{
            p1 = c1.at(ic1);
        }

        if(ic2 >= c2.size()){
            p2 = c2.at(c2.size()-1);
        }else{
            p2 = c2.at(ic2);
        }

        if(p1.x() < p2.x())
        while(p1.x() <= p2.x() && ic1 < c1.size()-1){
            c2o.push_back(QPointF(p1.x(),p2.y()));
            c1o.push_back(p1);
            ic1++;
            p1 = c1.at(ic1);
            i++;
        }

        if(p1.x() > p2.x())
        while(p1.x() >= p2.x() && ic2 < c2.size()-1){
            c1o.push_back(QPointF(p2.x(),p1.y()));
            c2o.push_back(p2);
            ic2++;
            p2 = c2.at(ic2);
            i++;
        }

        if(p1.x() == p2.x()){
            c1o.push_back(p1);
            c2o.push_back(p2);
            ic1++;
            ic2++;
            i++;
        }


    }
    c1o.push_back(c1.at(c1.size()-1));
    c2o.push_back(c2.at(c2.size()-1));

    // render rect

    for(int i=0;i<c1o.size()-1;i++){

        QPointF c1oit = translatePoint(c1o.at(i), area);
        QPointF c1oi_t = translatePoint(c1o.at(i+1), area);

        QPointF c2oit = translatePoint(c2o.at(i), area);
        QPointF c2oi_t = translatePoint(c2o.at(i+1), area);

        if(c1oit.y() > c2oit.y()) std::swap(c1oit, c2oit);


        double h = fabs(c1oit.y() - c2oit.y());
        double w = fabs(c1oi_t.x() - c1oit.x());

        QRectF r2(c1oit, QSize(std::ceil(w), h));


        painter.fillRect(r2, QColor(0, 0, 0, 255));


    }


}
