#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QString>
#include <QPushButton>
#include <QProgressBar>

#include "../../lib/visualizer.h"
#include "../../lib/polygon.h"

#include "../../lib/logger.h"

#include "../../lib/simpleplots/splineplotter.h"
#include "../../lib/simpleplots/sppolygonplotter.h"
#include "../../lib/simpleplots/simpleplotwidget.h"

#include "../../lib/kmeans/src/kmeans.h"
#include "../../lib/kmeans/src/point.h"


class Window : public QMainWindow
{
    Q_OBJECT
public:
    Window();
    ~Window();
    QString getConvertedFile(QString file);
    Point PolygonToPoint(Polygon p);
    Polygon simplify_ATC_IPOL(Polygon input);

public slots:
    void loadShape();
    void loadShapeDirectory();
    void learn();
    void getNearest();



private :
    Polygon shape;
    Polygon shapes;
    QString shapeFile;

    QString shapeBaseDirectory;
    QString shapePointsFile;

    Logger *logger;

    QProgressBar *progress;
    QPushButton *BloadShape1;
    QPushButton *BloadShape2;
    QPushButton *BloadShape3;
    QPushButton *Bcompare;

    SPPolygonPlotter *visuPolygon;
    SimplePlotWidget *widgetPolygon;

    KMeans kmeans;
};

#endif // WINDOW_H
