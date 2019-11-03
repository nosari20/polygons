#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QString>

#include "../../lib/logger.h"
#include "../../lib/polygon.h"

#include "../../lib/simpleplots/splineplotter.h"
#include "../../lib/simpleplots/sppolygonplotter.h"


class Window : public QMainWindow
{
Q_OBJECT
public:
    Window(Polygon *model);
    ~Window();
    void simplify(QString file);

public slots:
    void refresh();
    void chooseSdpFile();


private :
    SPPolygonPlotter *visualizer;
    Polygon *model;
    Logger *logger;

};

#endif // WINDOW_H
