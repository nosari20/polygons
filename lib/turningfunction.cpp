#include "turningfunction.h"
#include "QDebug"

#include <math.h>
#include <algorithm>
#include <tuple>


const int coeff = 1;

TurningFunction::TurningFunction()
{

}


Curve TurningFunction::compute(const Polygon polygon, int s){
    double x = 0;
    double y = 0;
    Curve turning;


    // calcul le perimetre
    double perimetre = 0;
    for(unsigned int i = 0 ; i < polygon.size() - 1; i++){
        perimetre += dist2Points(
                    polygon.at(i),
                    polygon.at(i+1)
                    );
    }
    perimetre += dist2Points(
                polygon.at(polygon.size() - 1),
                polygon.at(0)
                );


    //point 0
    turning.push_back( std::make_pair(x,y));





    // points 1 a n-1
    for(int i = 1 ; i < polygon.size() - 1 ; i++){

        x += dist2Points(
                    polygon.at(i-1),
                    polygon.at(i)
                    )/perimetre;
        turning.push_back( std::make_pair(x,y));


        
        y +=  s *angle(
                    polygon.at(i-1),
                    polygon.at(i),
                    polygon.at(i+1)
                    );
        turning.push_back( std::make_pair(x,y));

    }
    x += dist2Points(
                polygon.at(polygon.size() - 2),
                polygon.at(polygon.size() - 1)
                )/perimetre;
    turning.push_back( std::make_pair(x,y));

    // point N-1

    y += s *  angle(
                polygon.at(polygon.size() - 2),
                polygon.at(polygon.size() - 1),
                polygon.at(0)
                );
    turning.push_back( std::make_pair(x,y));


    x += dist2Points(
                polygon.at(polygon.size() - 1),
                polygon.at(0)
                )/perimetre;
    turning.push_back( std::make_pair(1,y));


    return turning;
}


double TurningFunction::area(const Curve c1, const Curve c2 ){


    // egalize x
    int ic1 = 1;
    int ic2 = 1;

    Curve c1o;
    Curve c2o;

    c1o.push_back(c1.at(0));
    c2o.push_back(c2.at(0));

    int i = 0;

    std::pair<double,double> p1 = c1.at(ic1);
    std::pair<double,double> p2 = c2.at(ic2);

    while((ic1 < c1.size()  || ic2 < c2.size()) && (p1.first < 1 || p2.first < 1)){

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

        if(p1.first < p2.first)
            while(p1.first <= p2.first && ic1 < c1.size()-1){
                c2o.push_back(std::make_pair(p1.first,p2.second));
                c1o.push_back(p1);
                ic1++;
                p1 = c1.at(ic1);
                i++;
            }

        if(p1.first > p2.first)
            while(p1.first >= p2.first && ic2 < c2.size()-1){
                c1o.push_back(std::make_pair(p2.first,p1.second));
                c2o.push_back(p2);
                ic2++;
                p2 = c2.at(ic2);
                i++;
            }

        if(p1.first == p2.first){
            c1o.push_back(p1);
            c2o.push_back(p2);
            ic1++;
            ic2++;
            i++;
        }


    }
    c1o.push_back(c1.at(c1.size()-1));
    c2o.push_back(c2.at(c2.size()-1));


    double a = 0;
    for(int i = 1; i < c1o.size(); i++){

        double s1 = fabs(c1o.at(i).second - c2o.at(i).second);
        double s2 = fabs(c1o.at(i).first - c1o.at(i-1).first);

        a += s1*s2;
    }


    return a;

}
double TurningFunction::dist2Points(const DGtal::Z2i::Point p0, const DGtal::Z2i::Point p1){
    return sqrt(
                pow((double)p0[0] - (double)p1[0], 2) +
                pow((double)p0[1] - (double)p1[1], 2)
            );
}






std::tuple<double,Curve, Curve> TurningFunction::distance(const Polygon p1, const Polygon p2){
    Polygon cp2, cp1;


    if(p1.size() >= p2.size()){
        cp2 = p1;
		cp1 = p2;
	}else{
        cp2 = p2;
		cp1 = p1;

	}



    std::vector<DGtal::Z2i::Point> v2 = cp2.getPoints();
    double min = 100000;
    Curve minc1, minc2;
	
    Curve c1;
    Curve c2;
    double d;
	for (int i = 0; i < cp1.size(); i++){
        c1 = TurningFunction::compute(cp1);

		for (int i = 0; i < cp2.size(); i++){
		    c2 = TurningFunction::compute(cp2);

		    d = TurningFunction::area(c1,c2);

		    if(d < min){
		        min = d;
				minc1 = c1;
		        minc2 = c2;
		    }

		    std::vector<DGtal::Z2i::Point> v1 = cp2.getPoints();
		    v1.insert(v1.begin(), (DGtal::Z2i::Point &&) v1.at(v1.size() - 1));

		    v1.pop_back();
		    cp2.setPoints(v1);

		}

        std::vector<DGtal::Z2i::Point> v2 = cp1.getPoints();
        v2.insert(v2.begin(), (DGtal::Z2i::Point &&) v2.at(v2.size() - 1));

        v2.pop_back();
        cp1.setPoints(v2);

    }
    
    return std::tuple<double,Curve, Curve>(min,minc1, minc2);
}

double TurningFunction::angle(const DGtal::Z2i::Point p0, const DGtal::Z2i::Point p1, const DGtal::Z2i::Point p2){

    double a = dist2Points(p1, p2);
    double b = dist2Points(p0, p1);
    double c = dist2Points(p0, p2);
    if(a == 0 | b == 0 | c ==0)
        return 0;


    double cos = (pow(a, 2) + pow(b, 2) - pow(c, 2)) / (2 * a * b);

    double angle;
    


    if(cos < 0){
        if (cos < -1.0) cos = -1.0 ;
        angle = M_PI - acos(-cos);
    }else{
        if (cos > 1.0) cos = 1.0 ;
        angle = acos(cos);
    }


    double d = (p1[0]-p0[0])*(p2[1]-p0[1])-(p1[1]-p0[1])*(p2[0]-p0[0]);
    double o;
    if(d<0){
        o = 1;
    }else if(d>0){
        o = -1;
    }else{
        return 0;
    }

    return (M_PI - angle)* o;

}


double TurningFunction::sens(const DGtal::Z2i::Point p0, const DGtal::Z2i::Point p1, const DGtal::Z2i::Point p2){

    double d = (p1[0]-p0[0])*(p2[1]-p0[1])-(p1[1]-p0[1])*(p2[0]-p0[0]);
    if(d<0)
        return 1;

    return -1;
}



