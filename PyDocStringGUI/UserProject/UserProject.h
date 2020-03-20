#ifndef PYTHONPARSERUTILS_H
#define PYTHONPARSERUTILS_H

#include <QRegularExpression>
#include <QList>
#include <QString>

#include "FunctionDescription/functiondesc.h"

namespace UserProject
{

struct UserProject
{
    QList<PyDesc::FunctionDesc> funcList{};
    QString associatedPyFile{};
};

void writeProjectToFile(const UserProject &proj,QString iDocType,QString iPath);

UserProject readProjectFromFile(QString iDocType,QString iPath);

}

#endif // PYTHONPARSERUTILS_H
