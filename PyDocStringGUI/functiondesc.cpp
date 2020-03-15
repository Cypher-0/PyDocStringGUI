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


QString getFormattedDesc(const FunctionDesc &fDesc)
{
    QString title_desc{"Descriptif"};
    QString title_args{"Arguments"};
    QString title_return{"Retourne"};

    QString result{"\"\"\"\n"};

    QString formatedTitle{"**"+title_desc+"**"};
    result += formatedTitle + "\n";
    Utils::addCharXTimes(result,std::size(formatedTitle));
    result += "\n";

    result += fDesc.desc + "\n\n";

    formatedTitle = "**"+title_args+"**";
    result += formatedTitle + "\n";
    Utils::addCharXTimes(result,std::size(formatedTitle));
    result += "\n";

    for(const auto &elem : fDesc.list_args)
    {
        result += "- **`"+elem.name+"`** ( *"+elem.type+"* ): "+elem.desc+"\n\n";
    }

    result += "\n";
    formatedTitle = "**"+title_return+"**";
    result += formatedTitle + "\n";
    Utils::addCharXTimes(result,std::size(formatedTitle));
    result += "\n\n";

    if(std::size(fDesc.list_returnArgs) == 1)
    {
        QString desc{(fDesc.list_returnArgs[0].desc.isEmpty())?"-":fDesc.list_returnArgs[0].desc};
        result += "( ***"+fDesc.list_returnArgs[0].type+"*** ) : "+desc+"\n\n";
    }
    else if(std::size(fDesc.list_returnArgs) > 1)
    {
        result += "( ***tuple*** ) : \n\n";

        for(const auto &elem : fDesc.list_returnArgs)
        {
            QString desc{(elem.desc.isEmpty())?"-":elem.desc};
            result += "- "+elem.name+" : ( ***"+elem.type+"*** ): "+desc+"\n\n";
        }
    }
    else{}

    result += "\"\"\"";

    return result;
}

namespace Utils
{

void addCharXTimes(QString &baseStr,int charNum,QChar wantedChar)
{
    for(int i = 0; i < charNum;i++)
        baseStr += wantedChar;
}

}

}
