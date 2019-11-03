#include "window.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QObject>
#include <QFileDialog>
#include <QDebug>
#include <QTabWidget>
#include <QFileInfo>
#include <QProcess>
#include <QTime>
#include <QMessageBox>



#include <vector>
#include <utility>
#include <iostream>
#include <tuple>

#include <stdio.h>
#include "../../lib/converter.h"
#include "../../lib/turningfunction.h"
#include "../../lib/parser.h"



typedef struct ComparedTF_ {
    Curve tf1;
    Curve tf2;
    double dist;
    QString file;
    Polygon polygon;
    Polygon polygons;
} ComparedTF;

Window::Window() : QMainWindow()
{
    /*
     * Initialize view
     */
    // Set size

    resize(670,800);
    // Create main panel
    QWidget *panel = new QWidget();
    // Create layout
    QVBoxLayout *layout = new QVBoxLayout;
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

    // Create visuPolygon for shape and add it to visuLayout
    widgetPolygon = new SimplePlotWidget();

    visuPolygon = new SPPolygonPlotter(this);

    widgetPolygon->setPlotter(visuPolygon);
    visuLayout->addWidget(widgetPolygon);


    // Add shape visuPolygon to tabs
    tabs->addTab(visuPanel,"Shape");



    /*
     * TF visualization tab
     */

    QWidget *visuSimplePanel = new QWidget();
    QHBoxLayout *visuSimpleLayout = new QHBoxLayout;
    visuSimplePanel->setLayout(visuSimpleLayout);


    widgetTurning = new SimplePlotWidget();

    visuTurning = new SPLinePlotter(this);
    visuTurning->setXLabel("Distance (1/Total distance)");
    visuTurning->setYLabel("Angle (rad)");

    widgetTurning->setPlotter(visuTurning);

    visuSimpleLayout->addWidget(widgetTurning);

    // Add shape visuPolygon to tabs
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



    /*
     * Buttons
     */


    QWidget *buttonPanel = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonPanel->setLayout(buttonLayout);
    layout->addWidget(buttonPanel);

    BloadShape1 = new QPushButton("Load Shape");
    buttonLayout->addWidget(BloadShape1);
    QObject::connect(BloadShape1, SIGNAL(clicked()), this, SLOT(loadShape()));

    BloadShape2 = new QPushButton("Load Shape Directory");
    buttonLayout->addWidget(BloadShape2);
    QObject::connect(BloadShape2, SIGNAL(clicked()), this, SLOT(loadShapeDirectory()));

    Bcompare = new QPushButton("Start Ordering");
    buttonLayout->addWidget(Bcompare);
    QObject::connect(Bcompare, SIGNAL(clicked()), this, SLOT(compare()));




    /*
     * Progressbar
     */

    progress = new QProgressBar();
    progress->setVisible(true);
    progress->setMaximum(1);
    progress->setMinimum(0);
    progress->reset();
    progress->setTextVisible(true);
    progress->setFormat("Loading...");
    progress->setAlignment(Qt::AlignCenter);

    layout->addWidget(progress);



    shape = Polygon();
    shapeDirectory = QString("");

}

Window::~Window()
{
    delete visuPolygon;
    delete visuTurning;
    delete logger;
    delete progress;
    delete BloadShape1;
    delete BloadShape2;
    delete Bcompare;
    delete widgetTurning;
    delete widgetPolygon;

}

void Window::loadShape()
{
    shape = Polygon();
    QString file = QFileDialog::getOpenFileName(this, tr("Open file ..."),"~/",tr("Images (*.png *.pgm) ;; SDP file (*.sdp)"));
    if(file==NULL) return;
    visuPolygon->clear();
    visuTurning->clear();
    shapeFile = file;
    file = getConvertedFile(file);
    shape.loadSDPFile(file.toStdString());

    visuPolygon->setData(Converter::PolygonToQPolygon(shape),QColor(255,0,0,50));

    shapes = simplify_ATC_IPOL(shape);


    visuPolygon->setData(Converter::PolygonToQPolygon(shapes),QColor(255,0,0,125));
    Curve t = TurningFunction::compute(shapes);
    QList<QPointF> l;
    for(unsigned int i = 0 ; i < t.size() ; i++){
        l << QPointF(t.at(i).first,t.at(i).second);
    }
    visuTurning->setData(l,QColor(255,0,0,125));
    widgetPolygon->repaint();




}

void Window::loadShapeDirectory()
{
    shapeDirectory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"~/",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
}

QString Window::getConvertedFile(QString fileName)
{
    QString file = fileName;
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
}

void Window::compare()
{
    progress->setFormat("Loading...");
    if(shape.size() == 0 || shapeDirectory.compare(QString("")) == 0){
        QMessageBox messageBox;
        if(shape.size() == 0){
            messageBox.critical(0,"Error","A polygon file must be chosen !");
        }else{
            messageBox.critical(0,"Error","A polygon directory must be chosen !");
        }
        messageBox.setFixedSize(500,200);
        messageBox.show();
        return;
    }




    QList<ComparedTF> compared;
    QDir dir = QDir(shapeDirectory);
    dir.setNameFilters(QStringList() << "*.sdp" << "*.png" << "*.pgm");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList fileList = dir.entryList();

    if(fileList.count() == 0){
        QMessageBox messageBox;
        messageBox.critical(0,"Error","No allowed file found !");
        messageBox.setFixedSize(500,200);
        messageBox.show();
        return;
    }


    logger->clear();

    logger->log(QString("Compared file : " + shapeFile ));
    logger->log(QString("Base directory : " + shapeDirectory ));
    logger->log(QString("\n"));


    progress->setValue(0);
    progress->setMaximum(fileList.count());
    progress->setFormat("Loading files - Step 1/3");
    for (int i = 0; i < fileList.count(); i++)
    {

        ComparedTF current;
        current.file = fileList[i];
        Polygon shape2;
        shape2.loadSDPFile(getConvertedFile(shapeDirectory + "/" + current.file).toStdString());
        current.polygon = shape2;



        compared.push_back(current);

        progress->setValue(i);

    }


    progress->setValue(0);
    progress->setFormat("Simplifying shapes - Step 2/3");

    for(int i = 0; i < compared.size(); i++) {

        //ComparedTF current = compared.[i];
        compared[i].polygons = simplify_ATC_IPOL(compared[i].polygon);
        //compared.at(i) = current;
        progress->setValue(i);
    }







    progress->setValue(0);
    progress->setFormat("Comparing shapes - Step 3/3");



    for(int i = 0; i < compared.size(); i++) {


        std::tuple<double,Curve, Curve> r = TurningFunction::distance(shapes,compared[i].polygons);
        compared[i].dist = std::get<0>(r);
        compared[i].tf1 = std::get<1>(r);
        compared[i].tf2 = std::get<2>(r);

        progress->setValue(i);
    }




    std::sort(compared.begin(), compared.end(),
              [](const ComparedTF a, const ComparedTF b) -> bool { return a.dist < b.dist; });

    logger->log(QString("Polygon list (ordered by distance) : \n"));
    foreach (ComparedTF c, compared) {
        logger->log(
                    QString(
                        c.file + " :\n"
                                 "\t distance = " + QString::number(c.dist)
                        )
                    );
    }


    visuPolygon->setData(Converter::PolygonToQPolygon(compared.at(0).polygon),QColor(0,255,0,50));
    visuPolygon->setData(Converter::PolygonToQPolygon(compared.at(0).polygons),QColor(0,255,0,125));

    visuTurning->clear();

    Curve t = compared.at(0).tf1;
    QList<QPointF> l;
    for(unsigned int i = 0 ; i < t.size() ; i++){
        l << QPointF(t.at(i).first,t.at(i).second);
    }
    visuTurning->setData(l,QColor(255,0,0,125));


    t = compared.at(0).tf2;
    l.clear();
    for(unsigned int i = 0 ; i < t.size() ; i++){
        l << QPointF(t.at(i).first,t.at(i).second);
    }
    visuTurning->setData(l,QColor(0,255,0,125));

    progress->setValue(progress->maximum());

    widgetPolygon->repaint();


    QDir tmp("./tmp");
    tmp.removeRecursively();


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


