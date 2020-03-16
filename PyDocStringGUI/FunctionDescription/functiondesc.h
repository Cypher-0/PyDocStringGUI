#ifndef FUNCTIONDESC_H
#define FUNCTIONDESC_H

#include <QString>
#include <QList>
#include <QDomDocument>

#include "xmlProps.h"

namespace PyDesc
{

struct Argument
{
    static const inline QString className{"Argument"};

    QString name{""};
    QString type{""};
    QString desc{""};
};

struct FunctionDesc
{
    static const inline QString className{"FunctionDesc"};

    QString name{""};
    QString desc{""};
    QList<Argument> list_args{};
    QList<Argument> list_returnArgs{};
};


void writeToXml(const FunctionDesc &var,QDomDocument &doc, QDomElement &root);

void writeToXml(const Argument &var,QDomDocument &doc, QDomElement &root);



void readFromXmlNode(FunctionDesc &target,const QDomElement &objectRoot);

void readFromXmlNode(Argument &target,const QDomElement &objectRoot);



QString getFormattedDesc(const FunctionDesc &fDesc);

namespace Utils
{
void addCharXTimes(QString &baseStr,int charNum,QChar wantedChar = '-');
}

}

#endif // FUNCTIONDESC_H
