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
#include <tuple>

#include <stdio.h>
#include "../../lib/converter.h"
#include "../../lib/turningfunction.h"

#include "../../lib/simpleplots/simpleplotwidget.h"
#include "../../lib/parser.h"

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

    //visualizer->setAlpha(0.3);

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


    SimplePlotWidget* widget2 = new SimplePlotWidget();

    linePlotter = new SPLinePlotter(this);

    //linePlotter->setAlpha(0.3);
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









    // Set model
    shape1 = new Polygon();
    shape2 = new Polygon();


    BloadShape1 = new QPushButton("Load Shape 1");
    layout->addWidget(BloadShape1);
    QObject::connect(BloadShape1, SIGNAL(clicked()), this, SLOT(loadShape1()));

    BloadShape2 = new QPushButton("Load Shape 2");
    layout->addWidget(BloadShape2);
    QObject::connect(BloadShape2, SIGNAL(clicked()), this, SLOT(loadShape2()));

    Breset = new QPushButton("Reset");
    layout->addWidget(Breset);
    QObject::connect(Breset, SIGNAL(clicked()), this, SLOT(reset()));

}

Window::~Window(){
    delete visualizer;
    delete shape1;
    delete shape2;
    delete BloadShape1;
    delete BloadShape2;
    delete logger;

    delete Breset;
}

void Window::refresh(){
    Curve t1, t2;
    if(shape1->size()>0){
        visualizer->setData(Converter::PolygonToQPolygon(*shape1),QColor(255,0,0,125));
        t1 = TurningFunction::compute(*shape1);
        QList<QPointF> l1;
        for(unsigned int i = 0 ; i < t1.size() ; i++){
            l1 << QPointF(t1.at(i).first,t1.at(i).second);
        }
		linePlotter->clear();
        linePlotter->setData(l1,QColor(255,0,0,125));
    }
    if(shape2->size()>0){
        visualizer->setData(Converter::PolygonToQPolygon(*shape2),QColor(0,255,0,125));

    }

    if(shape1->size()>0 && shape2->size()>0){

        //TurningFunction::area(TurningFunction::compute(*shape1),TurningFunction::compute(*shape2));


        linePlotter->clear();
        std::tuple<double,Curve, Curve> r = TurningFunction::distance(simplify_ATC_IPOL(*shape1),simplify_ATC_IPOL(*shape2));

        logger->clear();
        logger->log(QString("Distance : ") + QString::number(std::get<0>(r)));

		
		QList<QPointF> l1;
        for(unsigned int i = 0 ; i < std::get<1>(r).size() ; i++){
            l1 << QPointF(std::get<1>(r).at(i).first,std::get<1>(r).at(i).second);
        }
        linePlotter->setData(l1,QColor(255,0,0,125));


        QList<QPointF> l2;
        for(unsigned int i = 0 ; i < std::get<2>(r).size() ; i++){
            l2 << QPointF(std::get<2>(r).at(i).first,std::get<2>(r).at(i).second);
        }
        linePlotter->setData(l2,QColor(0,255,0,125));


    }

    repaint();

}

void Window::loadShape1(){
    BloadShape1->setEnabled(false);
    QString file = chooseFile();
    if(file!=NULL){
        shape1->loadSDPFile(file.toStdString());
        QString dp = (QString &&) file.replace(QString(".sdp"), QString(".txt"));

        if(QFileInfo::exists(dp)){
            *shape1 = shape1->simplify(dp.toStdString());
        }
        refresh();
    }

}

void Window::loadShape2(){
    BloadShape2->setEnabled(false);
    QString file = chooseFile();
    if(file!=NULL){
        shape2->loadSDPFile(file.toStdString());
        QString dp = (QString &&) file.replace(QString(".sdp"), QString(".txt"));

        if(QFileInfo::exists(dp)){
            *shape2 = shape2->simplify(dp.toStdString());
        }
        refresh();
    }

}

QString Window::chooseFile(){
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
                              QStringList() << "-c" << QString("./tools/img2freeman --input "+ file +" > ./tmp/"+filename+".fc"),
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



        return file;


        refresh();


    }


}


Polygon Window::simplify_ATC_IPOL(Polygon input){

    QDir dir("./tmp");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    int id = qrand() % ((100 + 1)) + 100;;
    QString file = QString("./tmp/in_")+QString::number(id)+QString(".sdp");
    QString outfile = QString("./tmp/out_")+QString::number(id);


    bool ok = Parser::writeSDPFile(input.getPoints(), file.toStdString());


    if(ok){
        //simplify with ATC_IPOL
        QProcess process;
        process.start("/bin/bash",
                      QStringList() << "-c" << QString("./tools/ATC_IPOL/build/testContourDecom -i "+file+" -o " +outfile+" -d ./tools/ATC_IPOL/ImaGene-forIPOL"));

        //qDebug() << QString("./tools/ATC_IPOL/build/testContourDecom -i "+file+" -o " +outfile+" -d ./tools/ATC_IPOL/ImaGene-forIPOL");

        if(!process.waitForFinished()){ // beware the timeout default parameter
            qDebug() << "executing program failed with exit code" << process.exitCode();
            qDebug() << QString(process.readAllStandardOutput()).split('\n');
            qDebug() << QString(process.readAllStandardError()).split('\n');


        }

        Polygon out;
        QFile f(outfile+QString("DP.sdp"));
        if(!f.exists()){
            return input;
        }
        out.loadSDPFile((outfile+QString("DP.sdp")).toStdString());
        return out;

    }else{
        qDebug() << "Error while creating file";
        return input;
    }
}



void Window::reset(){
    visualizer->clear();
    linePlotter->clear();
    BloadShape1->setEnabled(true);
    BloadShape2->setEnabled(true);
    shape1 = new Polygon();
    shape2 = new Polygon();

    repaint();
}

