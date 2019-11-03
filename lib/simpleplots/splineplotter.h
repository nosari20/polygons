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

#ifndef SPLINEPLOTTER_H
#define SPLINEPLOTTER_H

#include "spabstractplotter.h"
#include <QList>
#include <QPointF>
#include <QRectF>
#include <QPair>


class SPLinePlotter : public SPAbstractPlooter
{
    Q_OBJECT
public:
    explicit SPLinePlotter(QObject *parent = 0);

    void clear();
    void setData(QList<QPair<QList<QPointF>,QColor>> data);
    void setData(QList<QPointF> data,QColor color);
    void setXLabel(QString xlabel);
    void setYLabel(QString ylabel);
    void setAlpha(double alpha);

    virtual void plot(QPainter &painter, QRect area) const;
    void diff(const QList<QPointF> c1, const QList<QPointF> c2, QPainter &painter, QRectF area ) const;

signals:

public slots:

private:
    QList<QPair<QList<QPointF>,QColor>> m_data;
    QString m_xlabel;
    QString m_ylable;

    double m_alpha;

    static QRectF dataRange(const QList<QPair<QList<QPointF>,QColor>> &data);
    void plotData(QPainter &painter, QRectF area) const;
};

#endif // SPLINEPLOTTER_H
