#include "simplification.h"

<<<<<<< HEAD
Simplification::Simplification()
{
}
    Polygon Algo_KiMPA(Polygon,Polygon,int,DGtal::Z2i::Point){
        Polygon V,M;
        int K;
        std::vector<double> distance;
        std::vector<double> angles;
        std::vector<double> velocite;
        std::vector<double> acceleration;
        std::vector<DGtal::Z2i::Point> V1;
        DGtal::Z2i::Point C=V.barycentre();
        for (int i=0;i<V.size();i++){
            DGtal::Z2i::Point p=V.at(i);
            double d=0;
            d=sqrt(pow((double)p[0]-(double)C[0],2)+pow((double)p[1]-(double)C[0],2));
            distance.push_back(d);
=======
Simplification::Simplification(){
}
Polygon Algo_KiMPA(Polygon,Polygon,int,DGtal::Z2i::Point){
    Polygon V,M;
    int K;
    std::vector<double> distance;
    std::vector<double> angles;
    std::vector<double> velocite;
    std::vector<double> acceleration;
    std::vector<DGtal::Z2i::Point> V1;
    DGtal::Z2i::Point C=V.barycentre();
    for (int i=0;i<V.size();i++){
        DGtal::Z2i::Point p=V.at(i);
        double d=0;
        d=sqrt(pow((double)p[0]-(double)C[0],2)+pow((double)p[1]-(double)C[0],2));
        distance.push_back(d);
    }
    for(int i=0;i<V.size();i++) {
        DGtal::Z2i::Point p2=V.at(i);
        int p1x=C[0];
        int p1y=C1[1];
        int p2x=p2[0];
        int p2y=p2[1];
        int p3x=p3[0];
        int p3y=p3[1]+1;
        double a=sqrt(pow((double)p2x-(double)p3x,2)+pow((double)p2y-(double)p3y,2));
        double b=sqrt(pow((double)p1x-(double)p2x,2)+pow((double)p1y-(double)p2y,2));
        double c=sqrt(pow((double)p1x-(double)p3x,2)+pow((double)p1y-(double)p3y,2));
        double cosi=(-pow(a,2)+pow(b,2)+pow(c,2))/(2*b*c);
        angles.push_back(cosi);
        for(i=0;i<V.size();i++) {
            double dv=0;
            double da=0;
            dv=distance[i+1]-distance[i];
            da=angles[i+1]-angles[i];
            velocite.push_back(dv/da);
>>>>>>> 5c8011cf17e30b0a79a684e47e4b392f847fa86d
        }
        for(int i=0;i<V.size();i++) {
            double dv =velocite[i+1]-velocite[i];
            double da=angles[i+1]-angles[i];
            double sign=dv/abs(dv);
            double acc=sign*dv/da;
            acceleration.push_back(acc);
        }
<<<<<<< HEAD
        for (int i=0;i<K;i++) {
            int j=velocite.size();
            std::sort(velocite.begin(),velocite.end());
            V1[i]=V[j];
            j--;
            }
=======
>>>>>>> 5c8011cf17e30b0a79a684e47e4b392f847fa86d

    }
<<<<<<< HEAD

=======
    for (int i=0;i<K;i++) {
        int j=velocite.end();
        std::sort(velocite.begin(),velocite.end());
        V1[i]=V[j];
        j--;
    }

    return M;
}
>>>>>>> 5c8011cf17e30b0a79a684e47e4b392f847fa86d
