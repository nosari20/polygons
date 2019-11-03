#include <iostream>
#include <string>
#include <vector>


#include "../../lib/polygon.h"
#include "../../lib/parser.h"
#include "../../lib/converter.h"

#include <QPolygon>
#include <QPointF>
#include <QFile>
#include <QIODevice>
#include <QTextStream>



int main(int argc,char *argv[])
{
    if(argc < 3){
        std::cerr << "Too few arguments" << std::endl;
        std::cerr << "Usage : simplify SDP_FILE DOMINANT_POINTS_FILE [OUTPUT_FILE]" << std::endl;
        return 1;
    }

    std::vector<std::string> args(argv, argv + argc);

    Polygon *polygon = new Polygon();

    polygon << QPointF(0,0);


    return 0;

}
