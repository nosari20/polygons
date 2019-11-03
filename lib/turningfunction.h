#ifndef TURNINGFUNCTION_H
#define TURNINGFUNCTION_H

#include <vector>
#include <utility>
#include "polygon.h"
#include "DGtal/base/Common.h"
#include "DGtal/base/BasicTypes.h"
#include "DGtal/helpers/StdDefs.h"

typedef std::vector<std::pair<double,double>> Curve ;

class TurningFunction
{
public:
    TurningFunction();
    static Curve compute(const Polygon polygon, int s = -1);
    static double area(const Curve c1, const Curve c2 );
    static std::tuple<double,Curve, Curve> distance(const Polygon p1, const Polygon p2);

private:
    static double angle(const DGtal::Z2i::Point p0, const DGtal::Z2i::Point p1, const DGtal::Z2i::Point p2);
    static double dist2Points(const DGtal::Z2i::Point p0, const DGtal::Z2i::Point p1);
    static double sens(const DGtal::Z2i::Point p0, const DGtal::Z2i::Point p1, const DGtal::Z2i::Point p2);

};

#endif // TURNINGFUNCTION_H
