#include "functiondesc.h"

#include "xmlProps.h"

namespace PyDesc
{

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
        result += "( ***"+fDesc.list_returnArgs[0].type+"*** ) : "+fDesc.list_returnArgs[0].desc+"\n\n";
    }
    else if(std::size(fDesc.list_returnArgs) > 1)
    {
        result += "( ***tuple*** ) : \n\n";

        for(const auto &elem : fDesc.list_returnArgs)
        {
            result += "- "+elem.name+" : ( ***"+elem.type+"*** ): "+elem.desc+"\n\n";
        }
    }

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
