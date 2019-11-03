#include "parser.h"
#include <iostream>
#include <fstream>

#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QStringList>
#include <QDebug>



Parser::Parser()
{

}

std::vector<DGtal::Z2i::Point> Parser::parseSDPFile(std::string  url){


    std::vector<DGtal::Z2i::Point> listPoint;

    QFile inputFile(QString::fromStdString(url));
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList list = line.split(" ");
            if(list.size() == 2){
                bool okx = false;
                bool oky = false;

                double x = list.at(0).toDouble(&okx);
                double y = list.at(1).toDouble(&oky);

                if(okx && oky){
                    listPoint.push_back(DGtal::Z2i::Point(x,y));
                }



            }
        }
        inputFile.close();
    }


    if( listPoint.size() <= 1) return parseInlineSDPFile(url);

    return listPoint;

}

std::vector<DGtal::Z2i::Point> Parser::parseInlineSDPFile(std::string  url){


    std::vector<DGtal::Z2i::Point> listPoint;

    QFile inputFile(QString::fromStdString(url));
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList list = line.split(" ");
            for(int i = 0; i < list.size()-1; i+=2){

                bool okx = false;
                bool oky = false;

                double x = list.at(i).toDouble(&okx);
                double y = list.at(i+1).toDouble(&oky);

                if(okx && oky){
                    listPoint.push_back(DGtal::Z2i::Point(x,y));
                }


            }
        }
        inputFile.close();
    }

    return listPoint;

}


bool Parser::writeSDPFile(std::vector<DGtal::Z2i::Point> points, std::string url, std::string separator){
    //QFile file(QString::fromStdString(url));
    std::ofstream myfile;
    myfile.open (url);
    if (!myfile.is_open()) return false;
    myfile << "# grid curve 1/1 closed" << '\n';
    for (int i = 0; i < points.size()-1; i++){
        myfile << points.at(i)[0] << " " << points.at(i)[1] << separator;
    }
     myfile << '\n';

    myfile.close();

    return true;
}


std::vector<int> Parser::parseDominantPointFile(std::string  url){
    std::vector<int> listPoint;

    QFile inputFile(QString::fromStdString(url));
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList list = line.split(" ");
            if(list.size() == 2){
                bool okx = false;
                bool oky = false;

                int x = list.at(0).toInt(&okx);
                int y = list.at(1).toInt(&oky);

                if(okx && oky){
                    listPoint.push_back(y);
                }



            }
        }
        inputFile.close();
    }

    return listPoint;
}

