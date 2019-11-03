#include "polygon.h"

#include "parser.h"
#include "converter.h"

#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QDebug>

#include <algorithm>
#include <cmath>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


#include "DGtal/geometry/tools/Hull2DHelpers.h"
#include "DGtal/geometry/tools/PolarPointComparatorBy2x2DetComputer.h"
#include "DGtal/geometry/tools/determinant/AvnaimEtAl2x2DetSignComputer.h"
#include "DGtal/geometry/tools/determinant/InHalfPlaneBySimple3x3Matrix.h"


Polygon::Polygon()
{

}
Polygon::~Polygon()
{

}



int Polygon::size() const{
    return points.size();
}

DGtal::Z2i::Point Polygon::barycentre() const {
    int x=0;
    int y=0;
    for (int i=0; i<points.size();i++) {
        x=x+points.at(i)[0];
        y=y+points.at(i)[1];
    }
    int xbary=x/points.size();
    int ybary=y/points.size();
    return DGtal::Z2i::Point(xbary,ybary);
}

std::vector<double> Polygon::LongSegments() const {
    double l;
    std ::vector<double> Longeurs;
    for (int i=0;i<points.size()-1;i++) {
        l=0;
        DGtal::Z2i::Point a=points.at(i);
        int ax=a[0];
        int ay=a[1];
        DGtal::Z2i::Point b=points.at(i+1);
        int bx=b[0];
        int by=b[1];
        l=sqrt(pow((double)ax-(double)bx,2)+pow((double)ay-(double)by,2));
        Longeurs.push_back(l);
    }
    DGtal::Z2i::Point c=points.at(points.size()-1);
    int cx=c[0];
    int cy=c[1];
    DGtal::Z2i::Point d=points.at(0);
    int dx=d[0];
    int dy=d[1];
    l=sqrt(pow((double)cx-(double)dx,2)+pow((double)cy-(double)dy,2));
    Longeurs.push_back(l);
    return Longeurs;
}

double Polygon::Perimetre () const{
    double perimetre=0;
    std::vector<double> Longeurs;
    Longeurs=LongSegments();
    for(std::vector<double>::iterator i=Longeurs.begin();i!=Longeurs.end();++i){
        perimetre=perimetre+ *i;
    }
    return perimetre;
}

std::vector<double> Polygon::Dist_Point_Barycentre() const{
    std::vector<double> dist_P_G;
    DGtal::Z2i::Point g=barycentre();
    DGtal::Z2i::Point p;
    int gx=g[0];
    int gy=g[1];
    double d;
    for(int i=0;i<points.size();i++){
        d=0;
        p=points.at(i);
        int px=p[0];
        int py=p[1];
        d=sqrt(pow((double)px-(double)gx,2)+pow((double)py-(double)gy,2));
        dist_P_G.push_back(d);
    }
    return dist_P_G;
}

double Polygon::aire() const{
    double aire=0;
    DGtal::Z2i::Point g=barycentre();
    DGtal::Z2i::Point p1;
    DGtal ::Z2i::Point p2;
    int gx=g[0];
    int gy=g[1];
    for(int i=0;i<points.size()-1;i++) {
        p1=points.at(i);
        int p1x=p1[0];
        int p1y=p1[1];
        p2=points.at(i+1);
        int p2x=p2[0];
        int p2y=p2[1];
        double a=0;
        double b=0;
        double c=0;
        double p=0;
        double s=0;
        a=sqrt(pow((double)p1x-(double)gx,2)+pow((double)p1y-(double)gy,2));
        b=sqrt(pow((double)p1x-(double)p2x,2)+pow((double)p1y-(double)p2y,2));
        c=sqrt(pow((double)p2x-(double)gx,2)+pow((double)p2y-(double)gy,2));
        p=(a+b+c)/2;
        s=sqrt(p*(p-a)*(p-b)*(p-c));
        aire=aire+s;
    }
    p1=points.at(points.size()-1);
    int p1x=p1[0];
    int p1y=p1[1];
    p2=points.at(0);
    int p2x=p2[0];
    int p2y=p2[1];
    double a=0;
    double b=0;
    double c=0;
    double p=0;
    double s=0;
    a=sqrt(pow((double)p1x-(double)gx,2)+pow((double)p1y-(double)gy,2));
    b=sqrt(pow((double)p1x-(double)p2x,2)+pow((double)p1y-(double)p2y,2));
    c=sqrt(pow((double)p2x-(double)gx,2)+pow((double)p2y-(double)gy,2));
    p=(a+b+c)/2;
    s=sqrt(p*(p-a)*(p-b)*(p-c));
    aire=aire+s;
    return aire;
}

std::vector<double> Polygon::Angles()const{

    std::vector<double> angles;

    auto calc_angle = [](const DGtal::Z2i::Point p0, const DGtal::Z2i::Point p1, const DGtal::Z2i::Point p2) {

        auto dist2Points = [](const DGtal::Z2i::Point p0, const DGtal::Z2i::Point p1) {
            return sqrt(
                        pow((double)p0[0] - (double)p1[0], 2) +
                    pow((double)p0[1] - (double)p1[1], 2)
                    );
        };

        double a = dist2Points(p1, p2);
        double b = dist2Points(p0, p1);
        double c = dist2Points(p0, p2);
        double cos, angle;
        if(a == 0 | b == 0 | c ==0){
            angle = 0;

        }else{


            cos = (pow(a, 2) + pow(b, 2) - pow(c, 2)) / (2 * a * b);

            if(cos < 0){
                if (cos < -1.0) cos = -1.0 ;
                angle = M_PI - acos(-cos);
            }else{
                if (cos > 1.0) cos = 1.0 ;
                angle = acos(cos);
            }
        }
        return angle;
    };



    //pour le premmier point

    angles.push_back(calc_angle(points.at(points.size()-1), points.at(0), points.at(1)));


    // autres points

    for (int i = 1; i < points.size()-2; ++i) {

        angles.push_back(calc_angle(points.at(i-1), points.at(i), points.at(i+1)));

    }
    //dernier point

    angles.push_back(calc_angle(points.at(points.size()-2), points.at(points.size()-1), points.at(0)));



    return angles;
}



Polygon Polygon::convexHull() const{

    typedef DGtal::InHalfPlaneBySimple3x3Matrix<DGtal::Z2i::Point, DGtal::int64_t> Functor;
    Functor functor;

    //convex hull of a simple polygonal line that is not weakly externally visible
    std::vector<DGtal::Z2i::Point> polygonalLine = points;



    std::vector<DGtal::Z2i::Point> resGraham, res;
    typedef DGtal::PredicateFromOrientationFunctor2<Functor, false, false> StrictPredicate;
    StrictPredicate predicate( functor );


    DGtal::functions::Hull2D::closedGrahamScanFromAnyPoint( polygonalLine.begin(), polygonalLine.end(), back_inserter( resGraham ), predicate );



    DGtal::MelkmanConvexHull<DGtal::Z2i::Point, Functor> ch( functor );




    for (std::vector<DGtal::Z2i::Point>::const_iterator
         it = polygonalLine.begin(),
         itEnd = polygonalLine.end();
         it != itEnd; ++it)
        ch.add( *it );



    DGtal::functions::Hull2D::melkmanConvexHullAlgorithm( polygonalLine.begin(), polygonalLine.end(), back_inserter( res ), functor );


    Polygon poly;
    foreach (DGtal::Z2i::Point p, res) {
        poly << p;
    }
    return poly;




}

DGtal::Z2i::Point  Polygon::at(const int index) const{
    return points.at(index);
}

std::vector<DGtal::Z2i::Point> Polygon::getPoints() const{
    return points;
}

void Polygon::setPoints(std::vector<DGtal::Z2i::Point> l){
    points = l;
}

Polygon Polygon::simplify(const std::string fileName) const{

    Polygon simplifiedShape;
    std::vector<int> dp = Parser::parseDominantPointFile(fileName);


    foreach( int pos, dp ){
        simplifiedShape << points.at(pos);
    }

    //TODO

    return simplifiedShape;
}


void Polygon::loadSDPFile(const std::string  fileName){
    points.clear();

    std::vector<DGtal::Z2i::Point > list = Parser::parseSDPFile(fileName);
    int minx = 10000000;
    int miny = 10000000;
    foreach( DGtal::Z2i::Point  p, list ){
        minx = std::min(minx, p[0]);
        miny = std::min(miny, p[1]);
    }
    foreach( DGtal::Z2i::Point  p, list ){
        p[0] = p[0] - minx;
        p[1] = p[1] - miny;
        *this << p;
    }

}


std::vector<std::pair<double,Polygon>> Polygon::orderedByDistance(const std::string dirName) const{
    std::vector<std::pair<double,Polygon>> output;

    //create list
    QDir dir(QString::fromStdString(dirName));
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    dir.setNameFilters(QStringList() << "*.sdp");

    //Parcours ton répertoire
    foreach(QFileInfo fileInfo, dir.entryInfoList())
    {

        std::pair<double, Polygon> polyres = compareTo(fileInfo.absoluteFilePath().toStdString());
        output.push_back(polyres);


    }


    //order list

    std::sort(output.begin(), output.end(),
              [](const std::pair<double,Polygon> a, const std::pair<double,Polygon> b) -> bool { return a.first < b.first; });


    return output;

}

std::pair<double,Polygon> Polygon::compareTo(const std::string polygonFile) const{
    Polygon p;
    p.loadSDPFile(polygonFile);
    return std::make_pair(compareTo(p),p);


}

double Polygon::compareTo(Polygon p) const{

    return (((double) rand() / (RAND_MAX)));
}





