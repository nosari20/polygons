#include "logger.h"

Logger::Logger(QWidget *parent) : QTextEdit(parent)
{
    setReadOnly(true);
}


void Logger::log(QString s){
    insertPlainText(s + QString("\n"));
}
