#include "PythonParser/PythonParser.hpp"
#include <QString>
#include <QRegularExpression>
#include <QList>
#include <QFile>
#include "FunctionDescription/functiondesc.h"

#include <QDebug>
#define cout qDebug()

namespace PyDesc
{

namespace PyFileParser
{

namespace Utils
{
    QList<int> indexesOf(const QString &str,const QRegularExpression &regex)
    {
        int index{0};

        QList<int> outList;

        while(index != -1)
        {
            auto temp = str.indexOf(regex,index+1);

            index = temp;

            if(index != -1)
            {
                outList.append(index);
            }
        }

        return outList;
    }

    QList<int> linesOf(const QString &text, const QRegularExpression &regex, bool keepInComments)
    {
        auto linesList{text.split("\n")};

        QList<int> outList;

        for(int i = 0; i < std::size(linesList);i++)
        {
            auto line{linesList.at(i)};

            auto matchs{regex.match(line)};

            if(matchs.hasMatch())
            {
                if(keepInComments)
                {
                    outList.append(i);
                }
                else // pb here
                {
                    auto commentIndex{line.indexOf("#")};

                    line = line.remove(commentIndex,line.length()-commentIndex);

                    if(line.indexOf(regex) != -1 || commentIndex == -1)
                    {
                        outList.append(i);
                    }
                }
            }
        }

        return outList;
    }

    FunctionDesc getFunctionFromLine(QString line)
    {
        line.remove(QRegularExpression{"[\\t\\s\\r]"});
        line.remove(QRegularExpression{"[#].{0,}"});
        line.remove(std::size(line)-2,2); //remove python ":" from func def and last parenthesis
        auto t{line.split(",")};
        line = "";
        for(const auto& e : t)
        {
            auto eqIndex{e.indexOf("=")};
            line += e.mid(0,eqIndex)+",";
        }
        line.remove(0,3);//remove python "def" keyword

        auto tempSplit{line.split("(")}; //split func name and params

        if(tempSplit.length() != 2)//if there is not a func name and parameters
            return FunctionDesc{};

        FunctionDesc out{tempSplit[0],""};

        //parameters
        tempSplit = tempSplit[1].split(',',QString::SkipEmptyParts);

        QList<Argument> argsList;

        for(auto elem : tempSplit)
        {
            auto arg = elem.remove(QRegularExpression("\\):.{0,}$"));//delete ): at the end of defenition
            arg.remove(QRegularExpression("=.{1,}$"));//delete parameter default value

            if(arg == "self" || std::size(arg) == 0)
                continue;

            argsList.append(Argument{std::move(arg)});
        }

        out.list_args = std::move(argsList);

        return out;
    }

    //linesOfLim contains all lines index containing at least one <"""> (limit of docstring)
    std::tuple<int, int> getDocStringStartEndLines(int defLine,int nextDefLine,const QList<int> &linesOfLim)
    {
        int start{-1},end{-1};

        bool firstDocStringLim{true};
        for(const auto &limLine : linesOfLim)
        {
            if(limLine > defLine)
            {
                if(limLine < nextDefLine)
                {
                    if(firstDocStringLim)
                    {
                        start = limLine;
                        firstDocStringLim = false;
                    }
                    else
                    {
                        end = limLine;
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
        }

        return {std::move(start),std::move(end)};
    }

    bool writeLinesListToFile(const QString &filePath, const QStringList &lines)
    {
        QString outText;

        for(const auto &line : lines)
        {
            outText += line + "\n";
        }

        QFile file{filePath};
        if(!file.open(QIODevice::WriteOnly))
        {
            cout << "Cannot open : <" << filePath << ">   ("<<__func__<<")";
            return false;
        }

        file.write(outText.toUtf8());

        return true;
    }
}

}

}
