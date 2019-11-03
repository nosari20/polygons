#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QString>

#include "../../lib/visualizer.h"
#include "../../lib/polygon.h"
#include "../../lib/logger.h"

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
    void chooseSdpFile();


private :
    SPPolygonPlotter *visualizer;
    Polygon *model;
    SPLinePlotter *linePlotter;
    Logger *logger;
};

#endif // WINDOW_H
