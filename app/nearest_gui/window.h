#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QString>
#include <QPushButton>
#include <QProgressBar>

#include "../../lib/logger.h"
#include "../../lib/polygon.h"

#include "../../lib/simpleplots/splineplotter.h"
#include "../../lib/simpleplots/sppolygonplotter.h"
#include "../../lib/simpleplots/simpleplotwidget.h"


class Window : public QMainWindow
{
Q_OBJECT
public:
    Window();
    ~Window();
    void simplify(QString file);
    QString getConvertedFile(QString file);
    Polygon simplify_ATC_IPOL(Polygon input);

public slots:
    void loadShape();
    void loadShapeDirectory();

    void compare();


private :
    SPPolygonPlotter *visuPolygon;
    SPLinePlotter *visuTurning;
    Polygon shape;
    Polygon shapes;
    QString shapeFile;
    QString shapeDirectory;
    Logger *logger;

    QProgressBar *progress;
    QPushButton *BloadShape1;
    QPushButton *BloadShape2;
    QPushButton *Bcompare;

    SimplePlotWidget *widgetTurning;
    SimplePlotWidget *widgetPolygon;


};

#endif // WINDOW_H
