#ifndef PYTHONPARSERUTILS_H
#define PYTHONPARSERUTILS_H

#include <QRegularExpression>
#include <QList>
#include <QString>

#include "FunctionDescription/functiondesc.h"

namespace PyDesc
{

namespace PyFileParser
{

namespace Utils
{
    QList<int> indexesOf(const QString &str,const QRegularExpression &regex);

    QList<int> linesOf(const QString &text,const QRegularExpression &regex);

    FunctionDesc getFunctionFromLine(QString line);
}

}

}

#endif // PYTHONPARSERUTILS_H
