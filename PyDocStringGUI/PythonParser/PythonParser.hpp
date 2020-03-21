/*
 * Regexp :
 * Déclaration de fonction python :
 *      - Options : Multiline
 *      [\n\t\s]{0,}def[\s]{1,}[\w_\=\(\),\s]{1,}[:]{1}
 *
 *
*/

#include "FunctionDescription/functiondesc.h"
#include "UserProject/UserProject.h"

#include <QString>
#include <QList>
#include <QStringList>

namespace PyDesc
{

namespace PyFileParser
{
    QList<FunctionDesc> findFunctionsIndexes(QString pyFile);

    QList<FunctionDesc> getFunctionDescListFromStr(const QStringList &textLines, const QList<int> &defLinesIndex);

    void writeFuncDescToPyFile(const UserProject::UserProject &proj);


//---------------------------------------

namespace Utils
{
    QList<int> indexesOf(const QString &str,const QRegularExpression &regex);

    QList<int> linesOf(const QString &text,const QRegularExpression &regex,bool keepInComments = false);

    FunctionDesc getFunctionFromLine(QString line);

    std::tuple<int, int> getDocStringStartEndLines(int defLine,int nextDefLine,const QList<int> &linesOfLim);

    void writeLinesListToFile(const QString &file,const QStringList &lines);
}
}

}
