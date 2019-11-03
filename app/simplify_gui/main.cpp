#include "../../lib/polygon.h"
/*
#include "../../lib/parser.h"
#include "../../lib/converter.h"

#include <QPolygon>
using namespace std;
using namespace DGtal;
using namespace Z2i;
int main()
{


    Polygon *polygon = new Polygon();
    polygon->loadSDPFile(QString::fromStdString(args[1]));

    Converter::VectorLibBoardPointFromQListQPoint(polygon->getShape());








    return 1;
}
*/

#include <QApplication>

#include "window.h"



int main(int argc,char *argv[])
{


    QApplication app(argc,argv);

    Window window;
    window.show();
    return app.exec();

}
