#ifndef CONVERTER_H
#define CONVERTER_H

#include <QPointF>
#include <QList>
#include <QPolygonF>

#include "polygon.h"
#include <vector>
#include "DGtal/base/Common.h"
#include "DGtal/base/BasicTypes.h"
#include "DGtal/helpers/StdDefs.h"


class Converter
{
public:
    Converter();
    



    static QList<QPointF> PolygonToQPointF(const Polygon input);
    static QPolygonF PolygonToQPolygon(const Polygon input);

    static  QList<QPointF> DGtalPointsToQPoints(const std::vector<DGtal::Z2i::Point> input);




};

#endif // CONVERTER_H
