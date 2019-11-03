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

#include "sppolygonplotter.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

#include <math.h>
#include "simpleplotcommon.h"

SPPolygonPlotter::SPPolygonPlotter(QObject *parent) :
    SPAbstractPlooter(parent), m_xlabel("X axis"), m_ylable("Y axis"), m_alpha(1), m_data()
{
}

void SPPolygonPlotter::clear(){
    m_data.clear();
}


void SPPolygonPlotter::setData(QList<QPair<QList<QPointF>,QColor>> data)
{
    m_data = data;
    setDataRange(dataRange(data), true);
}

void SPPolygonPlotter::setData(QPolygonF data,QColor color)
{
    QList<QPointF> list;
    for(int i=0;i<data.size();i++){
        list << data.at(i);
    }
    setData(list, color);
}
void SPPolygonPlotter::setData(QList<QPointF> data,QColor color)
{

    m_data << QPair<QList<QPointF>,QColor>(data,color);
    setDataRange(dataRange(m_data), true);
}

void SPPolygonPlotter::setXLabel(QString xlabel)
{
    m_xlabel = xlabel;
}

void SPPolygonPlotter::setYLabel(QString ylabel)
{
    m_ylable = ylabel;
}

void SPPolygonPlotter::setAlpha(double alpha)
{
    m_alpha = alpha;
}

void SPPolygonPlotter::plot(QPainter &painter, QRect area) const
{
    plotGrid(painter, area);
    plotAxis(painter, area, m_xlabel, m_ylable);
    plotData(painter, area);
}

QRectF SPPolygonPlotter::dataRange(const QList<QPair<QList<QPointF>,QColor>> &data)
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
            range.setCoords(MIN( MIN(p.x(), range.left()),MIN(p.y(), range.top())),
                            range.left(),

                            MAX( MAX(p.x(), range.right()),MAX(p.y(), range.bottom())),
                            range.right());
        }

    }
    return range;
}


void SPPolygonPlotter::plotData(QPainter &painter, QRectF area) const
{


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

            //painter.drawText( QPoint(lt.x(), lt.y()), QString::number(i));

        }
        if(l.size()>0){
            QPointF ltl = translatePoint(l.at(l.size()-1), area);
            QPointF rbl = translatePoint(l.at(0), area);

            painter.drawLine(QLine(ltl.x(), ltl.y(), rbl.x(), rbl.y()));
        }



    }



}


