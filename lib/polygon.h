#ifndef POLYGON_H
#define POLYGON_H

#include <string>
#include <utility>
#include <vector>

#include "DGtal/base/Common.h"
#include "DGtal/base/BasicTypes.h"
#include "DGtal/helpers/StdDefs.h"

class Polygon{

public:
    /*
     * Add point to Polygon
     */
    inline Polygon *operator<< (const DGtal::Z2i::Point p){ points.push_back(p); return this; }

public:
    /*
     * Constructor
     */
    explicit Polygon();

    /*
     * Destructor
     */
    ~Polygon();

    /*
     * size method
     *
     * @return size
     */
    int size() const;

     /*
      * barycentre method
      *
      * @return barycentre
      */


     DGtal::Z2i::Point barycentre() const;

     std::vector<double> LongSegments() const;

     double Perimetre() const;

     std::vector<double>Dist_Point_Barycentre() const;

     double aire() const;
     std::vector<double> Angles() const;
     
     
     Polygon convexHull() const;
     
     
     
     
     



     /*
     Polygon Algo_KiMPA(Polygon,Polygon,int,DGtal::Z2i::Point);
     */

    /*
     * at method
     *
     * @param index
     *
     * @return point at index i
     */
    DGtal::Z2i::Point at(const int index) const;

    /*
      * getPoints method
      *
      * @return points
      */
    std::vector<DGtal::Z2i::Point> getPoints() const;

    /*
      * setPoints method
      *
      * @param new points
      *
      */
    void setPoints(std::vector<DGtal::Z2i::Point> l);

    /*
     * simplify method
     *
     * @param fileName : dominant points file
     *
     * @return simplified Polygon
     */
    Polygon simplify(const std::string fileName) const;

    /*
     * compareTo method
     *
     * @param polygonFile : polygon file
     *
     * @return pair<distance between comparated Polygon and this, comparated Polygon>
     * @return d
     */
    std::pair<double,Polygon> compareTo(const std::string polygonFile) const;

    /*
     * compareTo method
     *
     * @param polygon : polygon
     *
     * @return pair<distance between comparated Polygon and this, comparated Polygon>
     */
    double compareTo(const Polygon polygon) const;

    /*
     * orderedByDistance method
     *
     * @param dirName : directory of polygon files
     *
     * @return list of all comparated polygons with distances
     */
    std::vector<std::pair<double,Polygon>> orderedByDistance(const std::string dirName) const;

    /*
     * loadSDPFile method
     *
     * @param fileName
     *
     * load .sdp file and add point to this
     */
    void loadSDPFile(const std::string fileName);




private:
    /*
     * representation of the polygon with QPolygonF
     */
    std::vector<DGtal::Z2i::Point> points;

};

#endif // POLYGON_H
