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

    void writeFuncDescToPyFile(const UserProject::UserProject &proj)
    {
        QFile file{proj.associatedPyFile};

        if(!file.open(QIODevice::ReadOnly))
        {
            cout << "Cannot open <" << proj.associatedPyFile << ">";
            return;
        }

        auto text{file.readAll()};
        file.close();

        QRegularExpression regex_pyFunc{"[\\n\\t\\s]{0,}def[\\s]{1,}[\\w_\\=\\(\\),\\s]{1,}[:]{1}"};
        regex_pyFunc.setPatternOptions(QRegularExpression::MultilineOption | QRegularExpression::ExtendedPatternSyntaxOption);

        QRegularExpression regex_pyDocStr{"[.]{0,}[\\\"]{3}[.]{0,}"};
        regex_pyDocStr.setPatternOptions(QRegularExpression::MultilineOption | QRegularExpression::ExtendedPatternSyntaxOption);

        auto linesIndexList_pyDef{Utils::linesOf(text,regex_pyFunc,false)};
        auto linesIndexList_pyDocStr{Utils::linesOf(text,regex_pyDocStr)};

        auto linesList{text.split('\n')};
        linesIndexList_pyDef.append(std::size(linesList)-1);

        cout << linesIndexList_pyDef;

        for(int i = 0; i < std::size(linesIndexList_pyDef)-1;i++)
        {
            auto [start,end]{Utils::getDocStringStartEndLines(linesIndexList_pyDef[i],linesIndexList_pyDef[i+1],linesIndexList_pyDocStr)};

            auto funcName{Utils::getFunctionFromLine(linesList[linesIndexList_pyDef[i]]).name};

            cout << funcName << "  " << start <<"  " << end;
        }
    }
}

}
