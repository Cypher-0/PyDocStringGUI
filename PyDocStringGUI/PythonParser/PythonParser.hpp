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
inline const static QString regexStr_pyFunction{"[\\n\\t\\s]{0,}def[\\s]{1,}[\\w_\\=\\(\\)\\[\\],\\s]{1,}[:]{1}"};

    QList<FunctionDesc> findFunctions(QString pyFile);

    QList<FunctionDesc> getFunctionDescListFromStr(const QStringList &textLines, const QList<int> &defLinesIndex);

    bool writeFuncDescToPyFile(const UserProject::UserProject &proj, bool saveFileBefore = true);


//---------------------------------------

namespace Utils
{
    QList<int> indexesOf(const QString &str,const QRegularExpression &regex);

    QList<int> linesOf(const QString &text,const QRegularExpression &regex,bool keepInComments = false);

    FunctionDesc getFunctionFromLine(QString line);

    std::tuple<int, int> getDocStringStartEndLines(int defLine,int nextDefLine,const QList<int> &linesOfLim);

    bool writeLinesListToFile(const QString &file,const QStringList &lines);
}
}

}
