/*
 * Regexp :
 * Déclaration de fonction python :
 *      - Options : Multiline
 *      [\n\t\s]{0,}def[\s]{1,}[\w_\=\(\),\s]{1,}[:]{1}
 *
 *
*/

#include "FunctionDescription/functiondesc.h"

#include <QString>
#include <QList>
#include <QStringList>

namespace PyDesc
{

namespace PyFileParser
{
    QList<FunctionDesc> findFunctionsIndexes(QString pyFile);

    QList<FunctionDesc> getFunctionDescListFromStr(const QStringList &textLines, const QList<int> &defLinesIndex);
}

}
