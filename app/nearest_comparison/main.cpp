#include <QObject>
#include <QDebug>
#include <QFileInfo>
#include <QProcess>
#include <QTime>
#include <QDir>



#include <vector>
#include <utility>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <tuple>
#include "../../lib/converter.h"
#include "../../lib/turningfunction.h"
#include "../../lib/parser.h"
#include "../../lib/polygon.h"




typedef struct ComparedTF_ {
    Curve tf;
    double dist;
    QString file;
    Polygon polygon;
    Polygon polygons;
} ComparedTF;

Polygon simplify_ATC_IPOL(Polygon input){

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



QString getConvertedFile(QString fileName)
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

void write_comparison(QString shapeFile, QString shapeDirectory, std::ofstream &file)
{
    Polygon shape;
    shape.loadSDPFile(getConvertedFile(shapeFile).toStdString());

    
    if(shape.size() == 0 || shapeDirectory.compare(QString("")) == 0){

        if(shape.size() == 0){
            std::cerr << "Error : A polygon file must be chosen !" << std::endl;

        }else{
            std::cerr << "Error : A polygon directory must be chosen !" << std::endl;
        }
        exit(1);
    }


    Polygon shapes = simplify_ATC_IPOL(shape);


    QList<ComparedTF> compared;
    QDir dir = QDir(shapeDirectory);
    dir.setNameFilters(QStringList() << "*.sdp" << "*.png" << "*.pgm");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList fileList = dir.entryList();

    if(fileList.count() == 0){
        std::cerr << "Error : No allowed file found !" << std::endl;
        exit(1);
    }



    std::cout << QString("Compared file : " + shapeFile ).toStdString() << std::endl;
    std::cout << QString("Base directory : " + shapeDirectory ).toStdString() << std::endl;


    std::cout << "Loading files - Step 1/3" << std::endl;
    for (int i = 0; i < fileList.count(); i++)
    {

        ComparedTF current;
        current.file = fileList[i];
        Polygon shape2;
        shape2.loadSDPFile(getConvertedFile(shapeDirectory + "/" + current.file).toStdString());
        current.polygon = shape2;



        compared.push_back(current);

        std::cout << "#";

    }
    std::cout << std::endl;


    std::cout << "Simplifying shapes - Step 2/3" << std::endl;

    for(int i = 0; i < compared.size(); i++) {

        //ComparedTF current = compared.[i];
        compared[i].polygons = simplify_ATC_IPOL(compared[i].polygon);
        //compared.at(i) = current;
        std::cout << "#";
    }

    std::cout << std::endl;



    std::cout << "Comparing shapes - Step 3/3" << std::endl;

    for(int i = 0; i < compared.size(); i++) {


        std::tuple<double,Curve, Curve> r = TurningFunction::distance(shapes,compared[i].polygons);
        compared[i].dist = std::get<0>(r);

        std::cout << "#";
    }

    std::cout << std::endl;


    QFileInfo fileInfo(shapeFile);
    file << "\"" <<fileInfo.fileName().toStdString() << "\"" << ",";
    foreach (ComparedTF c, compared) {
        file << c.dist << ",";
    }
    file << std::endl;


    QDir tmp("./tmp");
    tmp.removeRecursively();


}




int main(int argc,char *argv[])
{
    if(argc < 2){
        std::cerr << "Too few arguments" << std::endl;
        std::cerr << "Usage : comparison DIRECTORY_1 DIRECTORY_2 [OUTPUT_FILE]" << std::endl;
        return 1;
    }
    std::vector<std::string> args(argv, argv + argc);

    std::string out = args[3];
    std::string in1 = args[1];
    std::string in2 = args[2];




    QDir dir = QDir(QString::fromStdString(in1));
    dir.setNameFilters(QStringList() << "*.sdp" << "*.png" << "*.pgm");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList fileList = dir.entryList();

    if(fileList.count() == 0){
        std::cerr << "Error : No allowed file found !" << std::endl;
        exit(1);
    }

    std::ofstream file;
    file.open (out);
    if (!file.is_open()){
        std::cerr << "Error : Cannot write output file !" << std::endl;
        exit(1);
    }

    for (int i = 0; i < fileList.count(); i++){
        write_comparison(QString::fromStdString(in1)+"/"+fileList[i], QString::fromStdString(in2),file);
    }


    file.close();

    return 0;
}
