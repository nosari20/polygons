#include <iostream>
#include <vector>
#include "point.h"
#include "kmeans.h"


using namespace std;

void usage() {
  cout << "Usage: ./kmeans <filename> " << endl;
  cout << "   file should be space-separated points, one point per line."
  << endl;
  return;
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    usage();
    return -1;
  }
  const std::string fpath = argv[1];

  std::vector<Point> points;

  KMeans::loadPoints(fpath, &points);
  for (const auto &p : points) {
    cout << p << endl;
  }


  KMeans kmeans;
  kmeans.init(points);

  kmeans.run();

  kmeans.printErrors();

  kmeans.printMeans();

  //kmeans.writeMeans("means.txt");

  std::vector<double> p(points.at(0).dimensions_);




  /*

  cout << "Enter a new Point" << endl;
  double arr [points.at(0).dimensions_];
  for(int i = 0 ; i < points.at(0).dimensions_ ; i++){
     cin >> arr[i];
  }




  vector<double> vec (arr, arr + sizeof(arr) / sizeof(arr[0]) );

  cout << kmeans.findNearestClass(Point(vec)).first << endl;

  */


  cout << "Finished." << endl;
  return 0;
}
