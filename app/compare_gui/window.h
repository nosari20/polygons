#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QString>
#include <QPushButton>

#include "../../lib/logger.h"
#include "../../lib/polygon.h"

#include "../../lib/simpleplots/splineplotter.h"
#include "../../lib/simpleplots/sppolygonplotter.h"


class Window : public QMainWindow
{
Q_OBJECT
public:
    Window();
    ~Window();
    void simplify(QString file);

public slots:
    void refresh();
    void loadShape1();
    void loadShape2();
    QString chooseFile();
    Polygon simplify_ATC_IPOL(Polygon input);
    void reset();



private :
    SPPolygonPlotter *visualizer;
    Polygon *shape1;
    Polygon *shape2;
    Logger *logger;
    SPLinePlotter *linePlotter;

    QPushButton *BloadShape1;
    QPushButton *BloadShape2;

    QPushButton *Breset;


};

#endif // WINDOW_H
