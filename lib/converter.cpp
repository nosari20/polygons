#include "converter.h"

Converter::Converter()
{

}

QList<QPointF> Converter::PolygonToQPointF(const Polygon input){

    QList<QPointF> output;


    for(int i=0;i<input.size();i++)
    {
        output.push_back(QPointF(input.at(i)[0],input.at(i)[1]));
    }

    return output;

}

QPolygonF Converter::PolygonToQPolygon(const Polygon input){

    QPolygonF output;


    for(int i=0;i<input.size();i++)
    {
        output << QPointF(input.at(i)[0],input.at(i)[1]);
    }

    return output;

}

QList<QPointF> Converter::DGtalPointsToQPoints(const std::vector<DGtal::Z2i::Point> input){

    QList<QPointF> output;


    for(int i=0;i<input.size();i++)
    {
        output.push_back(QPointF(input[i][0],input[i][1]));
    }

    return output;
}
