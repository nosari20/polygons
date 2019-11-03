#ifndef LOGGER_H
#define LOGGER_H

#include <QTextEdit>
#include <QList>
#include <QString>

class Logger : public QTextEdit
{
    Q_OBJECT
public:
    explicit Logger(QWidget *parent = 0);
    void log(QString s);

signals:

public slots:

};

#endif // LOGGER_H
