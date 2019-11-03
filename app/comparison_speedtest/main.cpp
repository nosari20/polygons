#include <QObject>
#include <QDebug>
#include <QFileInfo>
#include <QProcess>
#include <QTime>
#include <QDir>
#include <QString>
#include <QtAlgorithms>



#include <vector>
#include <utility>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <tuple>
#include <string>
#include <map>
#include <ctime>
#include "../../lib/converter.h"
#include "../../lib/turningfunction.h"
#include "../../lib/parser.h"
#include "../../lib/polygon.h"
#include "../../lib/simplif.h"
#include "../../lib/Ramer-Douglas-Peucker-Algorithm/simplifyPath.h"





typedef struct ComparedTF_ {
    double dist;
    QString file;
	Polygon polygon;
    QString class_;
    int num;
} ComparedTF;
 bool compareTF(const ComparedTF s1, const ComparedTF s2)
 {
     return s1.dist < s2.dist;
 }


std::string getClass(std::string file){

    QFileInfo info(QString::fromStdString(file));
    QString filename = info.completeBaseName();
    QStringList l = filename.split('_', QString::SkipEmptyParts);
    l.removeLast();
    l.removeFirst();
    l.removeFirst();
    return l.join("_").toStdString();
}



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


Polygon simplify_KiMPA(Polygon input){


    Polygon output;


    // Preprocessing
    simplifyPath sp;
    int step = 5;
    for(unsigned int i = 0 ; i < input.size()-step ; i+=step){
        std::vector<Point> pts;
        for(int j = i; j < i+step; j++){
            Point p;
            p.x = input.at(j)[0];
            p.y = input.at(j)[1];
            pts.push_back(p);
        }
        pts = sp.simplifyWithRDP(pts,2);
        for(unsigned int i = 0 ; i < pts.size() ; i++){
            output << DGtal::Z2i::Point((int)(pts[i].x),(int)(pts[i].y));
        }

    }


    //output = Simplification::Polygon_Algo_KiMPA(output, output.size()/2);

    return Simplification::Polygon_Algo_KiMPA(output, output.size()/2);

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

void write_stats(QString class_, std::vector<ComparedTF> shapes, std::vector<std::string> classes, std::ofstream &file)
{


	QList<ComparedTF> compared;
    int s = 0;

	std::map<std::string, int> stats;
	for(int i = 0; i < classes.size(); i++){
		stats[classes.at(i)] = 0;
	}
	
	for (int c = 0; c < shapes.size(); c++){
		
        //std::cerr << shapes.at(c).class_.toStdString() << " " <<class_.toStdString() << std::endl;
        if(shapes.at(c).class_ == class_){
			
			compared.clear();

			ComparedTF shape1 = shapes[c];
			for (int i = 0; i < shapes.size(); i++){
                std::cout << s << "/"<< shapes.size()/classes.size() << " " << i << "/" << shapes.size() <<  "                        " << '\r';
                fflush(stdout);

				ComparedTF shape2 = shapes[i];

			if(shape2.num == shape1.num) continue;

				std::tuple<double,Curve, Curve> r = TurningFunction::distance(shape1.polygon,shape2.polygon);

				ComparedTF current;
		    	current.file = shape1.file;
                current.class_ = shape2.class_;
				current.dist = std::get<0>(r);
				compared.push_back(current);
				
			}


            s++;

			std::cout << "Sorting...                                      " << std::endl;
			fflush(stdout);
			qSort(compared.begin(), compared.end(), compareTF);


			for (int i = 0; i < std::min(10,compared.size()); i++){
				stats[compared.at(i).class_.toStdString()] = stats[compared.at(i).class_.toStdString()] + 1;
			}
		}

        
    }



	
	file << "\"" << class_.toStdString() << "\"" << ",";
    for( const auto& p : stats ){
        file << p.second << ",";
	}
    file << std::endl;

    QDir tmp("./tmp");
    tmp.removeRecursively();


}




int main(int argc,char *argv[])
{
    if(argc < 2){
        std::cerr << "Too few arguments" << std::endl;
        std::cerr << "Usage : comparison_stats DIRECTORY" << std::endl;
        return 1;
    }
    std::vector<std::string> args(argv, argv + argc);

    std::string in = args[1];




    QDir dir = QDir(QString::fromStdString(in));
    dir.setNameFilters(QStringList() << "*.sdp" << "*.png" << "*.pgm");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList fileList = dir.entryList();

    if(fileList.count() == 0){
        std::cerr << "Error : No allowed file found !" << std::endl;
        exit(1);
    }

    // Loading files and simplifying shapes
    std::cout << "Loading files and simplifying shapes..."<< std::endl;

	std::vector<Polygon> listPoly;

    for (int i = 0; i < fileList.count(); i++){


        std::cout << i+1 << "/" << fileList.count() << "                        " << '\r';
        fflush(stdout);
        Polygon shape;
        QString file = QString::fromStdString(in)+"/"+fileList[i];
        shape.loadSDPFile(getConvertedFile(file).toStdString());
        shape = simplify_KiMPA(shape);

		

		listPoly.push_back(shape);

	}    
    std::cout << std::endl;


	// Comparing
	std::cout << "Start comparison..."<< std::endl;
	std::clock_t begin = std::clock();

  	int points = 0;
	for (int i = 0; i < listPoly.size()-1; i++){
		std::cout << i << "/"<< listPoly.size() <<  "                        " << '\r';
		points+=listPoly.at(i).size();
		std::tuple<double,Curve, Curve> r = TurningFunction::distance(listPoly.at(i),listPoly.at(i+1));
	}
	std::cout << listPoly.size()-1 << "/"<< listPoly.size() <<  "                        " << '\r';
	points+=listPoly.at(listPoly.size()-1).size();
	std::tuple<double,Curve, Curve> r = TurningFunction::distance(listPoly.at(listPoly.size()-1),listPoly.at(0));

	std::clock_t end = std::clock();
  	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    
	std::cout << "Elapsed time : " << elapsed_secs << "secs" << std::endl;
	std::cout << "Mean size : " << points/listPoly.size()  << std::endl;
  

    return 0;
}
