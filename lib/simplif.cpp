#include "simplif.h"
#include <QDebug>
#include <cmath>

typedef struct {
    DGtal::Z2i::Point point;
    int position;
    double angle;
    double distance;
    double velocite;
    double acceleration;

}Kpoint;
Simplification::Simplification()
{
}
double Calcul_distance (DGtal::Z2i::Point p1,DGtal::Z2i::Point p2) {
    double d;
    d=sqrt(pow((double)p1[0]-(double)p2[0],2)+pow((double)p1[1]-(double)p2[1],2));
    return d;
}
double Calcul_angle (DGtal::Z2i::Point p1,DGtal::Z2i::Point p2) {
    double angle ;
    int p1x=p1[0];
    int p1y=p1[1];
    int p2x=p2[0];
    int p2y=p2[1];
    int p3x=p1x;
    int p3y=p1y+1;
    double a=sqrt(pow((double)p2x-(double)p3x,2)+pow((double)p2y-(double)p3y,2))*10;
    double b=sqrt(pow((double)p1x-(double)p2x,2)+pow((double)p1y-(double)p2y,2))*10;
    double c=sqrt(pow((double)p1x-(double)p3x,2)+pow((double)p1y-(double)p3y,2))*10;
    if (p1x==p2x+1||p1x==p3x+1||p2x==p1x+1||p2x==p3x+1||p3x==p1x+1||p3x==p2x+1)
        angle =0;
    if((a == 0) | (b == 0) | (c ==0)){
        angle = 0;}
    else{
        double cosi=(-pow(a,2)+pow(b,2)+pow(c,2))/(2*b*c);
        if(cosi < 0){
            if (cosi < -1.0) cosi = -1.0 ;
            angle = /*M_PI - */acos(-cosi); }
        else {
            if (cosi > 1.0) cosi = 1.0 ;
            angle = acos(cosi);
        }

    }

    return angle;
}

double Calcul_velocite(Kpoint p1, Kpoint p2) {
    double dv=0;
    double da=0;
    dv=std::abs(p1.distance-p2.distance);
    da=(double)std::abs((double)(p1.angle-p2.angle));
    if (da==0)
        return 0;
    else
        return dv/da;
}
double Calcul_acceleration(Kpoint p1, Kpoint p2) {
    double dv=std::abs(p1.distance-p2.distance);;
    double da=p1.angle-p2.angle;
    double sign=dv/((double)std::abs(dv));
    double acc=sign*(dv/((double)std::abs(da)));

    if (da==0){
        return 0;
    }else{
        //qDebug() << p1.velocite <<" " <<p2.velocite;
         //qDebug() << dv << " " << da << " " << sign << " " << acc << " ";
        return acc;
    }
}
bool Compare_velocite(Kpoint p1,Kpoint p2) {
    return (p1.velocite>p2.velocite);
}
bool Compare_acceleration(Kpoint p1,Kpoint p2) {

    return (p1.acceleration>p2.acceleration);
}
bool Compare_position(Kpoint p1,Kpoint p2) {
    return (p1.position>p2.position);
}
Polygon Simplification::Polygon_Algo_KiMPA(Polygon V,int K) {
    int n=V.size();
    Kpoint Tpoint[n];
    Polygon V1;
    DGtal::Z2i::Point C=V.barycentre();
    for (int i=0;i<V.size();i++) {
        Tpoint[i].point=V.at(i);
    }
    for (int i=0;i<V.size();i++){
        double d=0;
        d=Calcul_distance(C,V.at(i));
        Tpoint[i].distance=d;
    }
    for(int i=0;i<V.size();i++) {
        double cosi=0;
        cosi=Calcul_angle(C,V.at(i));
        Tpoint[i].angle=cosi;
    }
    Tpoint[0].velocite=Calcul_velocite(Tpoint[0],Tpoint[n-1]);
    for(int i=1;i<V.size();i++) {
        Tpoint[i].velocite=Calcul_velocite(Tpoint[i],Tpoint[i-1]);
        //Affichage pour l'exemple main.sdp dans Shapes (les point 8;[223:243]
        //if (i==60) {
        /*
            double dv=std::abs(Tpoint[i].distance-Tpoint[i-1].distance);
            double da=std::abs(Tpoint[i].angle-Tpoint[i-1].angle);
            //qDebug() <<"Velocit"<< Tpoint[i].velocite <<"dv:"<<dv<<"da"<<da;
            //qDebug() <<"angle1:"<<Tpoint[i].angle<<"angle2:"<<Tpoint[i-1].angle;
            //qDebug()<<"angle:"<< (Tpoint[i].angle - Tpoint[i-1].angle);
            //qDebug() <<"Point1"<<Tpoint[i].point[0]<<"::"<<Tpoint[i].point[1];
            //qDebug() <<"Point2"<<Tpoint[i-1].point[0]<<"::"<<Tpoint[i-1].point[1];
            //qDebug() <<"barycentre"<<C[0]<<"::"<<C[1];
            */

        //}
    }
    Tpoint[0].acceleration=Calcul_acceleration(Tpoint[0],Tpoint[n]);
    for(int i=2;i<V.size();i++) {
        Tpoint[i].acceleration=Calcul_acceleration(Tpoint[i],Tpoint[i-2]);
    }
    for (int i=0;i<n;i++) {
        Tpoint[i].position=i;
    }
    std::vector<Kpoint> Vpoint;
    for (int i=0;i<n;i++) {
        Vpoint.push_back((Kpoint()));
        Vpoint[i].point=Tpoint[i].point;
        Vpoint[i].distance=Tpoint[i].distance;
        Vpoint[i].angle=Tpoint[i].angle;
        Vpoint[i].velocite=Tpoint[i].velocite;
        Vpoint[i].acceleration=Tpoint[i].acceleration;
        Vpoint[i].position=Tpoint[i].position;
    }
    std::sort(Vpoint.begin(),Vpoint.end(),Compare_velocite);
    std::vector<Kpoint>::const_iterator first = Vpoint.begin();
    std::vector<Kpoint>::const_iterator last = Vpoint.begin()+2*K;
    std::vector<Kpoint> Vpoint1(first, last);
    std::sort(Vpoint1.begin(),Vpoint1.end(),Compare_acceleration);
    std::vector<Kpoint>::const_iterator first1 = Vpoint1.begin();
    std::vector<Kpoint>::const_iterator last1 = Vpoint1.begin()+K;
    std::vector<Kpoint> Vpoint2(first1, last1);
    std::sort(Vpoint2.begin(),Vpoint2.end(),Compare_position);
    for(int i=0;i<K;i++){
        V1<<Vpoint2[i].point;
    }

    for (int i=0;i<n;i++) {
        //qDebug() <<"P:"<< Tpoint[i].position << "==> V:" << Tpoint[i].velocite <<"==>A:"<< Tpoint[i].acceleration;
    }

    //qDebug() << V.size();
    return V1;

}

