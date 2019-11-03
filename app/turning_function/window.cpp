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







    // Set model
    model = mod;


    QPushButton *loadShape = new QPushButton("Load Shape");
    layout->addWidget(loadShape);
    QObject::connect(loadShape, SIGNAL(clicked()), this, SLOT(chooseSdpFile()));

}

Window::~Window(){
    delete visualizer;
    delete model;
}

void Window::refresh(){
    visualizer->clear();
    linePlotter->clear();
    visualizer->setData(Converter::PolygonToQPolygon(*model),QColor(255,0,0,255));

    std::vector<std::pair<double,double>> t = TurningFunction::compute(*model);

    QList<QPointF> l;
    for(unsigned int i = 0 ; i < t.size() ; i++){
        l << QPointF(t.at(i).first,t.at(i).second);
    }
    linePlotter->setData(l,QColor(255,0,0,255));
    repaint();

}

void Window::chooseSdpFile(){
    QString file = QFileDialog::getOpenFileName(this, tr("Open file ..."),"~/",tr("SDP file (*.sdp) ;; Images (*.png *.pgm)"));






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
                              QStringList() << "-c" << QString("./tools/img2freeman --input "+ file +" --min 128 --max 255 > ./tmp/"+filename+".fc"),
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

        if(QFileInfo::exists(dp))
            simplify(dp);

        refresh();


    }


}



void Window::simplify(QString file){

    *model = model->simplify(file.toStdString());

}
