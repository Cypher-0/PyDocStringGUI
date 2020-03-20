#include <QString>
#include <QList>
#include "FunctionDescription/functiondesc.h"
#include "UserProject/UserProject.h"

#include <QDebug>
#define cout qDebug()

namespace UserProject
{

struct UserProject
{
    QList<PyDesc::FunctionDesc> funcList{};
    QString associatedPyFile{};
};

}
