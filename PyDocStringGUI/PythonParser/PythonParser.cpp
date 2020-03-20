#include "PythonParser.hpp"

#include <QFile>
#include <QList>
#include <QRegularExpression>

#include <QDebug>
#define cout qDebug()

namespace PyDesc
{

namespace PyFileParser
{
    QList<FunctionDesc> findFunctionsIndexes(QString pyFile)
    {
        QFile file{pyFile};

        if(!file.open(QIODevice::ReadOnly))
            return QList<FunctionDesc>{};

        QString text{file.readAll()};

        file.close();

        QRegularExpression regex_pyFunc{"[\\n\\t\\s]{0,}def[\\s]{1,}[\\w_\\=\\(\\),\\s]{1,}[:]{1}"};
        regex_pyFunc.setPatternOptions(QRegularExpression::MultilineOption | QRegularExpression::ExtendedPatternSyntaxOption);

        auto matchs{regex_pyFunc.match(text)};

        //cout << "Python file <" << pyFile << ">";
        //cout << "Is expression correct : " << regex_pyFunc.isValid();

        if(!text.contains(regex_pyFunc))
            return QList<FunctionDesc>{};

        auto linesIndexList{Utils::linesOf(text,regex_pyFunc)};
        auto linesList{text.split("\n")};

        return getFunctionDescListFromStr(linesList,linesIndexList);
    }

    QList<FunctionDesc> getFunctionDescListFromStr(const QStringList &textLines,const QList<int> &defLinesIndex)
    {
        QList<FunctionDesc> outList;

        for(const auto &elem : defLinesIndex)
        {
            outList.append(Utils::getFunctionFromLine(textLines[elem]));
        }

        return outList;
    }

    void writeFuncDescToPyFile(const UserProject::UserProject proj)
    {

    }
}

}
