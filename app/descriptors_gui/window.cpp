#include "window.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QObject>
#include <QFileDialog>
#include <QDebug>
#include <QTabWidget>
#include <QFileInfo>



#include <vector>
#include <utility>
#include <iostream>
#include "../../lib/converter.h"
#include "../../lib/turningfunction.h"

#include "../../lib/simpleplots/simpleplotwidget.h"

Window::Window(Polygon *mod) : QMainWindow()
{
    /*
     * Initialize view
     */
    // Set size

    resize(1000,800);
    // Create main panel
    QWidget *panel = new QWidget();
    // Create layout
    QHBoxLayout *layout = new QHBoxLayout;
    // Set layout to the lain panel
    panel->setLayout(layout);
    // Set main panel as central widget
    setCentralWidget(panel);
    // Create tab widget and add it to main panel
    QTabWidget *tabs = new QTabWidget();
    layout->addWidget(tabs);


    /*
     * Shape visualization tab
     */
    // Create panel and layout for shape visualization
    QWidget *visuPanel = new QWidget();
    QHBoxLayout *visuLayout = new QHBoxLayout;
    visuPanel->setLayout(visuLayout);

    // Create visualizer for shape and add it to visuLayout
    SimplePlotWidget* widget = new SimplePlotWidget();

    visualizer = new SPPolygonPlotter(this);

    visualizer->setAlpha(0.3);

    widget->setPlotter(visualizer);
    visuLayout->addWidget(widget);


    // Add shape visualizer to tabs
    tabs->addTab(visuPanel,"Shape");



    /*
     * log tab
     */

    QWidget *logPanel = new QWidget();
    QHBoxLayout *logLayout = new QHBoxLayout;
    logPanel->setLayout(logLayout);
    logger =  new Logger();
    logLayout->addWidget(logger);

    tabs->addTab(logPanel,"Log");





    // Set model
    model = mod;


    QPushButton *loadShape = new QPushButton("Load Shape");
    layout->addWidget(loadShape);
    QObject::connect(loadShape, SIGNAL(clicked()), this, SLOT(chooseSdpFile()));

}

Window::~Window(){
    delete visualizer;
    delete model;
    delete logger;
}

void Window::refresh(){
    visualizer->clear();
    visualizer->setData(Converter::PolygonToQPolygon(*model),QColor(255,0,0,255));



    /*
     * show polygon descriptors
     */
    logger->clear();
    logger->log(QString("Descriptors : "));
    logger->log(QString("Points : ") + QString::number(model->size()));
    DGtal::Z2i::Point p = model->barycentre();
    logger->log(QString("Barycentre: ( ") + QString::number(p[0]) + QString(" , ") +  QString::number(p[1]) + QString(" ) ")  );
    std::vector<double> segments =model->LongSegments();
    logger->log(QString("Longeur des segments: "));
    for(int unsigned i=0;i<segments.size();i++){
        logger->log(QString::number(segments.at(i)));
    }
    logger->log(QString("Perimetre: ") + QString::number(model->Perimetre()));
    logger->log(QString("Distance entre les points et le barycentre:"));
    std::vector<double> dist_P_G =model->Dist_Point_Barycentre();
    for(int unsigned i=0;i<dist_P_G.size();i++){
        logger->log(QString::number(dist_P_G.at(i)));
    }
     logger->log(QString("Aire: ") + QString::number(model->aire()));
     std::vector<double> angles =model->Angles();
     logger->log(QString("Angles: "));
     for(int unsigned i=0;i<angles.size();i++){
         logger->log(QString::number(angles.at(i)));
     }




    //TODO





    repaint();
}

void Window::chooseSdpFile(){
    QString file = QFileDialog::getOpenFileName(this, tr("Open file ..."),"/",tr("SDP file (*.sdp)"));
    if(file!=NULL){
        model->loadSDPFile(file.toStdString());
        refresh();

    }
}

