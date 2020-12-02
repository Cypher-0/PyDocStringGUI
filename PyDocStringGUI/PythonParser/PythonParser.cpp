#include "PythonParser.hpp"

#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QRegularExpression>

#include <QDebug>
#define cout qDebug()

namespace PyDesc
{

namespace PyFileParser
{
    QList<FunctionDesc> findFunctions(QString pyFile)
    {
        QFile file{pyFile};

        if(!file.open(QIODevice::ReadOnly))
            return QList<FunctionDesc>{};

        QString text{file.readAll()};

        file.close();

        QRegularExpression regex_pyFunc{regexStr_pyFunction};
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

    bool writeFuncDescToPyFile(const UserProject::UserProject &proj,bool saveFileBefore)
    {
        cout << "\n~~~~~~~~ BEGIN EXPORT ~~~~~~~~\n";

        QFile file{proj.associatedPyFile};
        if(saveFileBefore)
        {
            auto copyFile{QFileInfo{file}.absolutePath()+"/GUIsave_"+QFileInfo{file}.fileName()};

            if(QFile::exists(copyFile))
            {
                QFile::remove(copyFile);
            }

            if(!file.copy(copyFile))
                return false;

            cout <<"Python file saved as : <" <<copyFile<< ">\n";
        }


        if(!file.open(QIODevice::ReadOnly))
        {
            cout << "Cannot open <" << proj.associatedPyFile << ">   (" <<__func__<<")";
            return false;
        }

        QString text{file.readAll()};
        file.close();

        QRegularExpression regex_pyFunc{regexStr_pyFunction};
        regex_pyFunc.setPatternOptions(QRegularExpression::MultilineOption | QRegularExpression::ExtendedPatternSyntaxOption);

        QRegularExpression regex_pyDocStr{"[.]{0,}[\\\"]{3}[.]{0,}"};
        regex_pyDocStr.setPatternOptions(QRegularExpression::MultilineOption | QRegularExpression::ExtendedPatternSyntaxOption);

        auto linesIndexList_pyDef{Utils::linesOf(text,regex_pyFunc,false)};

        auto linesIndexList_pyDocStr{Utils::linesOf(text,regex_pyDocStr)};

        auto linesList{text.split('\n')};
        linesIndexList_pyDef.append(std::size(linesList)-1);

        QList<int> analysedFunctionsFromProj{};

        for(int i = std::size(linesIndexList_pyDef)-2; i >= 0 ;i--)
        {
            auto [start,end]{Utils::getDocStringStartEndLines(linesIndexList_pyDef[i],linesIndexList_pyDef[i+1],linesIndexList_pyDocStr)};

            auto searchedFunc{Utils::getFunctionFromLine(linesList[linesIndexList_pyDef[i]])};
            auto funcName{searchedFunc.name};

            auto funcProjIndex{PyDesc::Utils::findFunctionIndex(proj.funcList,searchedFunc)};


            //cout << funcName << "  " << start <<"  " << end << "  " << funcProjIndex;

            QRegularExpression regex_identLevel{"^[\\s]{0,}[.]{0,}"};
            auto match_identLevel{regex_identLevel.match(linesList[linesIndexList_pyDef[i]])};
            QString functionIden{match_identLevel.capturedTexts()[0]+"\t"};

            if(funcProjIndex == -1)
            {
                cout<<"<"+PyDesc::getPrototype(searchedFunc)+"> exist in Python file but not in desc file";
                continue;
            }
            analysedFunctionsFromProj.append(funcProjIndex);

            auto functionDesc{PyDesc::getFormattedDesc(proj.funcList[funcProjIndex],functionIden)};

            //add new function desc to def line
            linesList[linesIndexList_pyDef[i]] += "\n" + functionDesc;

            if(start == -1) //if there is no existing dosctring
            {
                //nothing
            }
            else if(start != -1 && end == -1) //if the docstring is on only one line
            {
                linesList.removeAt(start);
            }
            else if(start != -1 && end != -1)//if the docstring is on multiple lines
            {
                for(int j = end; j >= start;j--)
                {
                    linesList.removeAt(j);
                }
            }
        }

        QList<int> nonAnalysedFunctionsFromProj{};
        auto tempSize{std::size(proj.funcList)};
        for(int i = 0; i < tempSize;i++)
        {
            if(!analysedFunctionsFromProj.contains(i))
            {
                nonAnalysedFunctionsFromProj.append(i);
            }
        }

        if(std::size(nonAnalysedFunctionsFromProj) != 0)
        {
            cout << "\nFollowing functions are not defined in Python file :";
            for(const auto &elem : nonAnalysedFunctionsFromProj)
            {
                cout << PyDesc::getPrototype(proj.funcList[elem]);
            }
        }

        cout << "~~~~~~ END EXPORT ~~~~~~~\n";

        auto successWrite{Utils::writeLinesListToFile(proj.associatedPyFile,linesList)};

        if(!successWrite)
        {
            return false;
        }

        return true;
    }
}

}
