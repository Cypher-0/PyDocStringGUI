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



QString getPrototype(const FunctionDesc &fDesc);

QString getFormattedDesc(const FunctionDesc &fDesc,const QString &identation = "");

namespace Utils
{
void addCharXTimes(QString &baseStr,int charNum,QChar wantedChar = '-');

int findFunctionIndex(const QList<FunctionDesc> funcList,const FunctionDesc &func);
}

//name based equalitys
bool operator==(const FunctionDesc &func0,const FunctionDesc &func1);
bool operator==(const Argument &arg0,const Argument &arg1);

}

#endif // FUNCTIONDESC_H
