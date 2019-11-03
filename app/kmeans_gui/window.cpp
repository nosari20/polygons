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
#include <QMessageBox>



#include <vector>
#include <utility>
#include <iostream>
#include <math.h>       /* sqrt */
#include "../../lib/converter.h"
#include "../../lib/turningfunction.h"
#include "../../lib/simplif.h"
#include "../../lib/parser.h"

#include "../../lib/simpleplots/simpleplotwidget.h"

Window::Window() : QMainWindow(), kmeans()
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

    BloadShape2 = new QPushButton("Generate points file");
    buttonLayout->addWidget(BloadShape2);
    QObject::connect(BloadShape2, SIGNAL(clicked()), this, SLOT(loadShapeDirectory()));

    BloadShape3 = new QPushButton("Learn");
    buttonLayout->addWidget(BloadShape3);
    QObject::connect(BloadShape3, SIGNAL(clicked()), this, SLOT(learn()));


    Bcompare = new QPushButton("Get nearest");
    buttonLayout->addWidget(Bcompare);
    //Bcompare->setEnabled(false);
    QObject::connect(Bcompare, SIGNAL(clicked()), this, SLOT(getNearest()));




    /*
     * Progressbar
     */

    progress = new QProgressBar();
    progress->setVisible(true);
    progress->reset();
    progress->setTextVisible(true);
    progress->setFormat("Loading...");
    progress->setAlignment(Qt::AlignCenter);

    layout->addWidget(progress);



    shape = Polygon();
    shapeBaseDirectory = QString("");
    shapePointsFile = QString("");


    kmeans.quiet(true);

}

Window::~Window()
{
    delete visuPolygon;
    delete logger;
    delete progress;
    delete BloadShape1;
    delete BloadShape2;
    delete Bcompare;
    delete widgetPolygon;

}

void Window::loadShape()
{
    visuPolygon->clear();
    shape = Polygon();
    QString file = QFileDialog::getOpenFileName(this, tr("Open file ..."),"~/",tr("Images (*.png *.pgm) ;; SDP file (*.sdp)"));
    shapeFile = file;
    file = getConvertedFile(file);
    shape.loadSDPFile(file.toStdString());

    progress->setRange(0, 0);



    visuPolygon->clear();
    visuPolygon->setData(Converter::PolygonToQPolygon(shape),QColor(255,0,0,50));


    shapes = simplify_ATC_IPOL(shape);
    visuPolygon->setData(Converter::PolygonToQPolygon(shapes),QColor(255,0,0,125));


    double perimeter = shapes.Perimetre();
    std::vector<double> segments = shapes.LongSegments();
    int sumin = 0;
    int sumax = 0;
    for(int i = 0; i < segments.size(); i++){
        if(segments.at(i)/perimeter<0.05) sumin++;
        if(segments.at(i)/perimeter>0.1) sumax++;
        qDebug() << segments.at(i)/perimeter;
    }

    qDebug() << sumin << " " << sumax;


    widgetPolygon->repaint();


    progress->setRange(0, 1);


}

void Window::loadShapeDirectory()
{
    shapeBaseDirectory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"~/",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);


    QDir dir = QDir(shapeBaseDirectory);
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


    QString filename = shapeBaseDirectory + "/" +"dataset.dat";
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Cannot write file !");
        messageBox.setFixedSize(500,200);
        messageBox.show();
        return;
    }


    QTextStream stream(&file);



    logger->clear();
    progress->setValue(0);
    progress->setMaximum(fileList.count() + 1);




    for (int i = 0; i < fileList.count(); i++)
    {


        Polygon s;



        s.loadSDPFile(getConvertedFile(shapeBaseDirectory + "/" + fileList[i]).toStdString());


        s = simplify_ATC_IPOL(s);

        QFileInfo info(shapeBaseDirectory + "/" + fileList[i]);

        QString filename = info.completeBaseName();
        QStringList l = filename.split('_', QString::SkipEmptyParts);
        l.removeLast();
        filename = l.join("_");
        stream << filename;


        for (const auto &v : PolygonToPoint(s).data_) {
            stream << " " << v;
        }

        stream << endl;


        progress->setValue(i);

    }
    file.close();
    progress->reset();

    QMessageBox messageBox(this);
    messageBox.information(0,"File created", filename);
    messageBox.setFixedSize(500,200);
    messageBox.show();


}

void Window::learn(){
    QString file = QFileDialog::getOpenFileName(this, tr("Open file ..."),"~/",tr("Data file (*.dat)"));

    if(file==NULL) return;

    progress->setRange(0, 0);

    std::vector<Point> points;
    KMeans::loadPoints(file.toStdString(), &points);

    kmeans.init(points);

    kmeans.run();

    BloadShape3->setEnabled(true);
    progress->setRange(0, 1);

    kmeans.printErrors();
    kmeans.printMeans();

}


void Window::getNearest()
{
    if(!kmeans.hasLearned()){
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Learning not done !");
        messageBox.setFixedSize(500,200);
        messageBox.show();
        return;
    }
    if(shape.size() == 0){
        QMessageBox messageBox;
        messageBox.critical(0,"Error","No shape loaded!");
        messageBox.setFixedSize(500,200);
        messageBox.show();
        return;
    }
    progress->setRange(0, 0);

    Point pp = PolygonToPoint(shape);
    std::cout << pp << std::endl;
    std::pair<std::string, int> res = kmeans.findNearestClass(pp);
    
    logger->log(QString::fromStdString(res.first));

    QDir dir = QDir(shapeBaseDirectory);
    dir.setNameFilters(QStringList() << "*.sdp" << "*.png" << "*.pgm");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList fileList = dir.entryList();


    qDebug() << QString::fromStdString(res.first) << " " << res.second;
    for (int i = 0; i < fileList.count(); i++)
    {

        QFileInfo info(shapeBaseDirectory + "/" + fileList[i]);

        QString filename = info.completeBaseName();

        if(filename.startsWith(QString::fromStdString(res.first))){
			
			Polygon s;
        	s.loadSDPFile(getConvertedFile(shapeBaseDirectory + "/" + fileList[i]).toStdString());

            visuPolygon->clear();
            visuPolygon->setData(Converter::PolygonToQPolygon(s),QColor(0,255,0,50));
            s = simplify_ATC_IPOL(s);
            visuPolygon->setData(Converter::PolygonToQPolygon(s),QColor(0,255,0,125));


            visuPolygon->setData(Converter::PolygonToQPolygon(shape),QColor(255,0,0,50));
            visuPolygon->setData(Converter::PolygonToQPolygon(shapes),QColor(255,0,0,125));


			break;
		}
    }

    widgetPolygon->repaint();

    progress->setRange(0, 1);


}


Point Window::PolygonToPoint(Polygon p){
    std::vector<double> v;

    double area = p.aire();

    double perimeter = p.Perimetre();

	std::vector<double> segments = p.LongSegments();

	std::vector<double> angles = p.Angles();

    std::vector<double> distBar = p.Dist_Point_Barycentre();

	
	Polygon ch = p.convexHull();
	
	double area_ch = ch.aire();

    double perimeter_ch = ch.Perimetre();

	

	/*
	 * Descriptors
	 */


	// per(p)/area(p)
    v.push_back(perimeter/area);


	// per(ch)/area(ch)
    v.push_back(perimeter_ch/area_ch);

	
	// area(ch)/area(p)
    v.push_back(area_ch/area);
	

	// mean value of angles
	double sum = 0;
	for(int i = 0; i < angles.size(); i++){
        sum += angles.at(i);
    }
	double mean_angle = sum/angles.size();
    v.push_back(mean_angle);


    // variance value of angles
	sum = 0;
	for(int i = 0; i < angles.size(); i++){
        sum += std::pow(angles.at(i) - mean_angle,2);
    }
    v.push_back(sum/angles.size());


	// min and max distance from centroid and difference between min and max
	double max = 0;
    double min = 1000000;

    for(int i = 0; i < distBar.size(); i++){
        max = std::max(max,distBar.at(i));
		min = std::min(min,distBar.at(i));
    }
    v.push_back(min);
    v.push_back(max);
    v.push_back((max-min));


    // variance value of segment
	sum = 0;
	for(int i = 0; i < segments.size(); i++){
        sum += segments.at(i)/perimeter;
    }
	double mean_segment = sum/angles.size();
	
	sum = 0;
	for(int i = 0; i < segments.size(); i++){
        sum += std::pow(segments.at(i) - mean_segment,2);
    }
    v.push_back(sum/segments.size());
	

    // peaks

    /*
    int sumin = 0;
    int sumax = 0;
    for(int i = 0; i < segments.size(); i++){
        if(segments.at(i)/perimeter<0.05) sumin++;
        if(segments.at(i)/perimeter>0.1) sumax++;
    }

    //v.push_back(sumin);
    //v.push_back(sumax);

    v.push_back(p.size());
    */

    sum = 0;
    for(int i = 0; i < angles.size(); i++){
        if(angles.at(i) < M_PI/3) sum++;
    }
    v.push_back(sum/angles.size());




    /*

    // Aire
    v.push_back(ec.aire());

    // Perimetre
    perimeter = ec.Perimetre();
    v.push_back(perimeter);

    // Barycentre
    bar = ec.barycentre();
    v.push_back((double)bar[0]);
    v.push_back((double)bar[1]);




    // Segments
    segments = ec.LongSegments();
    maxValue = 50;
    minValue = 0;
    step = std::fabs(maxValue-minValue)/10;
    i = 0;
    for(i = minValue; i < maxValue; i+=step){
        double count = 0;
        for (const auto &seg : segments) {
            if(seg >= i && seg < i+step) count++;
        }
        v.push_back(count);
    }
    count = 0;
    for (const auto &seg : segments) {
        if(seg >= maxValue) count++;
    }



    // Distance barycentre
    distBar = ec.Dist_Point_Barycentre();
    maxValue = 50;
    minValue = 0;
    step = std::fabs(maxValue-minValue)/10;
    i = 0;
    for(i = minValue; i < maxValue; i+=step){
        double count = 0;
        for (const auto &seg : distBar) {
            if(seg >= i && seg < i+step) count++;
        }
        v.push_back(count);
    }
    count = 0;
    for (const auto &seg : distBar) {
        if(seg >= maxValue) count++;
    }

    // Angles
    angles = ec.Angles();
    maxValue = M_PI;
    minValue = 0;
    step = std::fabs(maxValue-minValue)/8;
    i = 0;
    for(i = minValue; i < maxValue; i+=step){
        double count = 0;
        for (const auto &val : angles) {
            if(val >= i && val < i+step) count++;
        }
        v.push_back(count);
    }
    count = 0;
    for (const auto &seg : angles) {
        if(seg >= maxValue) count++;
    }

    */






    return Point(v);

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
        int id = qrand() % ((100 + 1)) + 100;

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
                qDebug() << QString(process.readAllStandardError()).split('\n');
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
                      QStringList() << "-c" << QString("./tools/ATC_IPOL/build/testContourDecom -i "+file+" -o " +outfile+" -d ./tools/ATC_IPOL/ImaGene-forIPOL"),
                      QIODevice::ReadWrite | QIODevice::Text);

        if(!process.waitForFinished()){ // beware the timeout default parameter
            qDebug() << "executing program failed with exit code" << process.exitCode();
            qDebug() << QString(process.readAllStandardOutput()).split('\n');

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

