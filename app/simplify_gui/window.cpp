#include "window.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QObject>
#include <QFileDialog>
#include <QDebug>
#include <QTabWidget>
#include <QFileInfo>
#include <QProcess>
#include <QTime>
#include <QInputDialog>



#include <vector>
#include <utility>
#include <iostream>
#include "../../lib/converter.h"
#include "../../lib/turningfunction.h"
#include "../../lib/simplif.h"

#include "../../lib/simpleplots/simpleplotwidget.h"
#include "../../lib/Ramer-Douglas-Peucker-Algorithm/simplifyPath.h"

Window::Window() : QMainWindow()
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
     * Simplified shape visualization tab
     */

    QWidget *visuSimplePanel = new QWidget();
    QHBoxLayout *visuSimpleLayout = new QHBoxLayout;
    visuSimplePanel->setLayout(visuSimpleLayout);

    // Create visualizer for shape and add it to visuLayout




    //visuSimpleLayout->addWidget(visualizerTF);


    SimplePlotWidget* widget2 = new SimplePlotWidget();

    linePlotter = new SPLinePlotter(this);

    linePlotter->setAlpha(0.3);
    linePlotter->setXLabel("Distance (1/Total distance)");
    linePlotter->setYLabel("Angle (rad)");



    widget2->setPlotter(linePlotter);



    visuSimpleLayout->addWidget(widget2);


    // Add shape visualizer to tabs
    tabs->addTab(visuSimplePanel,"Turning function");





    /*
     * log tab
     */

    QWidget *logPanel = new QWidget();
    QHBoxLayout *logLayout = new QHBoxLayout;
    logPanel->setLayout(logLayout);
    logger =  new Logger();
    logLayout->addWidget(logger);

    tabs->addTab(logPanel,"Log");








    model= new Polygon();





    QPushButton *loadShape = new QPushButton("Load Shape");
    layout->addWidget(loadShape);
    QObject::connect(loadShape, SIGNAL(clicked()), this, SLOT(chooseSdpFile()));

}

Window::~Window(){
    delete visualizer;
    delete model;
}

void Window::refresh(){


}

void Window::chooseSdpFile(){
    QString file = QFileDialog::getOpenFileName(this, tr("Open file ..."),"~/",tr("Images (*.png *.pgm) ;; SDP file (*.sdp)"));






    if(file!=NULL){


        // file need to be converted
        if(!file.endsWith(".sdp")){

            // create tmp directory

            QDir dir("./tmp");
            if (!dir.exists()) {
                dir.mkpath(".");
            }

            QTime time = QTime::currentTime();
            qsrand((uint)time.msec());
            int id = qrand() % ((100 + 1)) + 100;;

            QString filename = QString("image-")+QString::number(id);


            if(file.endsWith(".png")){


                //convert to pgm
                QProcess process;
                process.start("/bin/bash",
                              QStringList() << "-c" << QString("./tools/convert "+ file +" -compress none ./tmp/"+filename+".pgm"),
                              QIODevice::ReadWrite | QIODevice::Text);

                if(!process.waitForFinished()){ // beware the timeout default parameter
                    qDebug() << "executing program failed with exit code" << process.exitCode();
                    qDebug() << QString(process.readAllStandardOutput()).split('\n');
                }


                file = QString("./tmp/"+filename+".pgm");

                goto pgm;

            }


            if(file.endsWith(".pgm")){
pgm:


                //convert pgm to freeman
                QProcess process;
                //process.setStandardOutputFile("/tmp/"+filename+".fc");
                process.start("/bin/bash",
                              QStringList() << "-c" << QString("./tools/img2freeman --input "+ file +"> ./tmp/"+filename+".fc"),
                              QIODevice::ReadWrite | QIODevice::Text);

                if(!process.waitForFinished()){ // beware the timeout default parameter
                    qDebug() << "executing program failed with exit code" << process.exitCode();
                    qDebug() << QString(process.readAllStandardOutput()).split('\n');
                }




                //convert freeman to sdp
                QProcess process2;
                //process.setStandardOutputFile("/tmp/"+filename+".fc");
                process2.start("/bin/bash",
                               QStringList() << "-c" << QString("./tools/freeman2sdp -i ./tmp/" + filename +".fc > ./tmp/" + filename +".sdp"),
                               QIODevice::ReadWrite | QIODevice::Text);

                if(!process2.waitForFinished()){ // beware the timeout default parameter
                    qDebug() << "executing program failed with exit code" << process.exitCode();
                    qDebug() << QString(process2.readAllStandardOutput()).split('\n');
                }


                file = QString("./tmp/"+filename+".sdp");

            }
        }




        model->loadSDPFile(file.toStdString());
        QString dp = file.replace(QString(".sdp"),QString(".txt"));

        //if(QFileInfo::exists(dp))





        visualizer->clear();
        linePlotter->clear();

        visualizer->setData(Converter::PolygonToQPolygon(*model),QColor(255,0,0,75));

        std::vector<std::pair<double,double>> t = TurningFunction::compute(*model);

        QList<QPointF> l;
        for(unsigned int i = 0 ; i < t.size() ; i++){
            l << QPointF(t.at(i).first,t.at(i).second);
        }
        linePlotter->setData(l,QColor(255,0,0,255));
        repaint();




        simplify(dp);


    }


}



void Window::simplify(QString file){

    /*
    bool ok;
    int i = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
                                 tr(QString(QString("Number of output points (min 3, max ")+QString::number(model->size())+QString(")")).toStdString().c_str()), 25, 3, model->size(), 10, &ok);
    if (!ok) return;

    /*
    qDebug() << i << QString(QString("Number of output points (min 3, max ")+QString::number(model->size())+QString(")"));

    Polygon shape2 = Simplification::Polygon_Algo_KiMPA(*model, i);
    visualizer->setData(Converter::PolygonToQPolygon(shape2),QColor(0,255,0,125));

    std::vector<std::pair<double,double>> t = TurningFunction::compute(shape2);

    linePlotter->clear();
    QList<QPointF> l;
    for(unsigned int i = 0 ; i < t.size() ; i++){
        l << QPointF(t.at(i).first,t.at(i).second);
    }
    linePlotter->setData(l,QColor(255,0,0,125));



    /*

    std::vector<Point> pts;
    for(unsigned int i = 0 ; i < model->size() ; i++){
        Point p;
        p.x = model->at(i)[0];
        p.y = model->at(i)[1];
        pts.push_back(p);
    }


    */

    simplifyPath sp;
    Polygon np;

    int step = 5;

    for(unsigned int i = 0 ; i < model->size()-step ; i+=step){
        std::vector<Point> pts;
        for(int j = i; j < i+step; j++){
            Point p;
            p.x = model->at(j)[0];
            p.y = model->at(j)[1];
            pts.push_back(p);
        }
        pts = sp.simplifyWithRDP(pts,2);
        for(unsigned int i = 0 ; i < pts.size() ; i++){
            np << DGtal::Z2i::Point((int)(pts[i].x),(int)(pts[i].y));
        }

    }

    //visualizer->setData(Converter::PolygonToQPolygon(np),QColor(255,255,0,255));


    Polygon shape2 = Simplification::Polygon_Algo_KiMPA(np, np.size()/4);
    visualizer->setData(Converter::PolygonToQPolygon(shape2),QColor(0,255,0,125));




    std::vector<std::pair<double,double>> t = TurningFunction::compute(shape2);

    linePlotter->clear();
    QList<QPointF> l;
    for(unsigned int i = 0 ; i < t.size() ; i++){
        l << QPointF(t.at(i).first,t.at(i).second);
    }
    linePlotter->setData(l,QColor(255,0,0,125));





    logger->clear();
    logger->log(QString("Input points : ")+ QString::number(model->size()));
    logger->log(QString("Output points : ")+ QString::number(shape2.size()));




    repaint();
}
