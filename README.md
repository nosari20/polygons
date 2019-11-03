# Polygon project

## Tech 
- Qt 5
- DGtal

## Apps

- compare_gui : compute the distance between 2 shapes (GUI)
- descriptors_gui : list the descriptors of a shape (GUI)
- kmeans_gui : clustering shapes using k-means (GUI)
- nearest_gui : get the nearest shape in a directory of shapes (GUI)
- simplify_gui : simplify a shape using KiMPA algorithm (GUI)
- turning_function : compute the turning function of a shape (GUI)
- nearest_comparison : create a csv file whit all distance resulting of a comparison of 2 directory- 

## Build

All apps are in  the `app` directory

```
$ cd app/app-name
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./app-name
```

## For devs

Install all requiered libs following the instruction in `install.txt`
