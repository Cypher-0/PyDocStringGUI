#include "functiondesc.h"

#include "xmlProps.h"

#include <QDebug>
#define cout qDebug()

namespace PyDesc
{

//----------------------------------------------------------------------------- XML - Write

void writeToXml(const FunctionDesc &var,QDomDocument &doc, QDomElement &root)
{
    using namespace XML::XMLProps;

    QDomElement node_varRoot = doc.createElement(Func::node_funcDescRoot);
    node_varRoot.setAttribute(Func::attrs_name,var.name);
    root.appendChild(node_varRoot);

    QDomElement node_desc = doc.createElement(Func::node_desc);
    node_desc.appendChild(doc.createTextNode(var.desc));
    node_varRoot.appendChild(node_desc);

    QDomElement node_list_args = doc.createElement(Func::node_list_args);
    for(const auto &elem : var.list_args)
    {
        node_varRoot.appendChild(node_list_args);

        writeToXml(elem,doc,node_list_args);
    }

    QDomElement node_list_returnArgs = doc.createElement(Func::node_list_returnArgs);
    for(const auto &elem : var.list_returnArgs)
    {
        node_varRoot.appendChild(node_list_returnArgs);

        writeToXml(elem,doc,node_list_returnArgs);
    }
}

void writeToXml(const Argument &var,QDomDocument &doc, QDomElement &root)
{
    using namespace XML::XMLProps;

    QDomElement node_varRoot = doc.createElement(Arg::node_argRoot);
    node_varRoot.setAttribute(Arg::attrs_name,var.name);
    node_varRoot.setAttribute(Arg::attrs_type,var.type);
    root.appendChild(node_varRoot);

    QDomElement node_desc = doc.createElement(Arg::node_desc);
    node_desc.appendChild(doc.createTextNode(var.desc));
    node_varRoot.appendChild(node_desc);
}

//-------------------------------------------------------------------------------- XML - READ

void readFromXmlNode(FunctionDesc &target,const QDomElement &objectRoot)
{
    using namespace XML::XMLProps;

    QString name{objectRoot.attribute(Func::attrs_name,"")};
    QString desc{""};

    QList<Argument> argsList{};
    QList<Argument> returnList{};


    if(objectRoot.tagName() == Func::node_funcDescRoot)
    {
        using namespace XML::XMLProps;


        auto currentNode {objectRoot.firstChild().toElement()};
        while(!currentNode.isNull())
        {
            auto currentNodeName{currentNode.tagName()};

            if(currentNodeName == Func::node_desc)
            {
                desc = currentNode.text();
            }
            else if(currentNodeName == Func::node_list_args)
            {
                auto currentArgNode{currentNode.firstChild().toElement()};
                while(!currentArgNode.isNull())
                {
                    Argument tempArg;
                    readFromXmlNode(tempArg,currentArgNode);
                    argsList.append(tempArg);

                    currentArgNode = currentArgNode.nextSiblingElement();
                }
            }
            else if(currentNodeName == Func::node_list_returnArgs)
            {
                auto currentArgNode{currentNode.firstChild().toElement()};
                while(!currentArgNode.isNull())
                {
                    Argument tempArg;
                    readFromXmlNode(tempArg,currentArgNode);
                    returnList.append(tempArg);

                    currentArgNode = currentArgNode.nextSiblingElement();
                }
            }


            currentNode = currentNode.nextSiblingElement();
        }
    }

    target = FunctionDesc{std::move(name),std::move(desc),std::move(argsList),std::move(returnList)};
}

void readFromXmlNode(Argument &target,const QDomElement &objectRoot)
{
    using namespace XML::XMLProps;

    QString name{objectRoot.attribute(Arg::attrs_name,"")};
    QString type{objectRoot.attribute(Arg::attrs_type,"")};
    QString argDesc{""};

    if(objectRoot.tagName() == Arg::node_argRoot)
    {
        using namespace XML::XMLProps;

        auto currentNode {objectRoot.firstChild().toElement()};
        while(!currentNode.isNull())
        {
            if(currentNode.tagName() == Arg::node_desc)
            {
                argDesc = currentNode.text();
            }
            currentNode = currentNode.nextSiblingElement();
        }
    }

    target = Argument{std::move(name),std::move(type),std::move(argDesc)};
}


//-------------------------------------------------------------------------------- OTHERS

QString getPrototype(const FunctionDesc &fDesc)
{
    QString vars{"("};
    for(const auto &elem : fDesc.list_args)
    {
        vars += elem.name + ((elem.type.isEmpty())?"":" : "+elem.type)+",";
    }
    if(std::size(fDesc.list_args) > 0)
        vars.remove(std::size(vars)-1,1);
    vars += ")";
    return fDesc.name+vars;
}

QString getFormattedDesc(const FunctionDesc &fDesc, const QString &identation)
{
    QString title_desc{"Descriptif"};
    QString title_args{"Arguments"};
    QString title_return{"Retourne"};

    QString result{identation+"\"\"\"\n"};

    //description

    QString formatedTitle{"**"+title_desc+"**"};
    result += identation + formatedTitle + "\n" + identation;
    Utils::addCharXTimes(result,formatedTitle.size());
    result += "\n";

    auto tempDesc{fDesc.desc};

    tempDesc.replace("\n","\n"+identation);
    result += identation + tempDesc + "\n\n";

    //arguments

    formatedTitle = "**"+title_args+"**";
    result += identation + formatedTitle + "\n" + identation;
    Utils::addCharXTimes(result,formatedTitle.size());
    result += "\n";

    if(fDesc.list_args.size() == 0)
    {
        result += identation + "( ***void*** )\n\n";
    }
    else
    {
        for(const auto &elem : fDesc.list_args)
        {
            auto elemDesc{elem.desc};
            result += identation + "- **`"+elem.name+"`** ( **<i> "+((elem.type.isEmpty())?"-":elem.type)
                      +" </i>** ): "+((elem.desc.isEmpty())?"-":elemDesc)+"\n\n";
        }
    }

    result += "\n";

    //return values

    formatedTitle = "**"+title_return+"**";
    result += identation + formatedTitle + "\n" + identation;
    Utils::addCharXTimes(result,formatedTitle.size());
    result += "\n\n";

    if(fDesc.list_returnArgs.size() == 0)
    {
        result += identation + "( ***void*** )";
    }
    else if(fDesc.list_returnArgs.size() == 1)
    {
        QString desc{(fDesc.list_returnArgs[0].desc.isEmpty())?"-":fDesc.list_returnArgs[0].desc};
        QString type{(fDesc.list_returnArgs[0].type.isEmpty())?"-":fDesc.list_returnArgs[0].type};
        result += identation + "( ***"+type+"*** ) : "+desc;
    }
    else if(fDesc.list_returnArgs.size() > 1)
    {
        result += identation + "( ***tuple*** ) : \n\n";

        for(const auto &elem : fDesc.list_returnArgs)
        {
            QString desc{(elem.desc.isEmpty())?"-":elem.desc};
            QString type{(elem.type.isEmpty())?"-":elem.type};
            result += identation + "- "+elem.name+" : ( ***"+type+"*** ): "+desc+"\n";
        }
    }

    result += "\n\n"+ identation + "\"\"\"";

    return result;
}

namespace Utils
{

void addCharXTimes(QString &baseStr,int charNum,QChar wantedChar)
{
    for(int i = 0; i < charNum;i++)
        baseStr += wantedChar;
}

int findFunctionIndex(const QList<FunctionDesc> funcList,const FunctionDesc &func)
{
    auto size{std::size(funcList)};
    for(int i = 0; i < size;i++)
    {
        if(funcList[i] == func)
            return i;
    }

    return -1;
}

}

bool operator==(const FunctionDesc &func0,const FunctionDesc &func1)
{
    //name + args list
    if(func0.name == func1.name &&
       func0.list_args == func1.list_args
      )
    {
        return true;
    }

    return false;
}

bool operator==(const Argument &arg0,const Argument &arg1)
{
    if(arg0.name == arg1.name)
    {
        return true;
    }

    return false;
}

}
