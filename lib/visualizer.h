#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPolygon>
#include <QList>
#include <QPoint>

class Visualizer : public QGraphicsView
{
public:
    Visualizer();
    ~Visualizer();
    void showPolygon(const QPolygon polygon);
    void showPolygon(const QPolygonF polygon);
    void showPolyline(const QList<QPointF> polyline);
    void clear();
    void set_ppux(int const x);
    void set_ppuy(int const y);

private:
    QGraphicsScene *scene;
    double ppux;
    double ppuy;
};

#endif // VISUALIZER_H
