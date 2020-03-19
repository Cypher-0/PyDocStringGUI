#ifndef CONFIG_SETTINGS_H
#define CONFIG_SETTINGS_H

#include <utility>
#include <QString>
#include <QDomDocument>
#include <QDomElement>

#include "xmlPropsSet.h"

#include <QDebug>
#define cout qDebug()

namespace Config
{

template<typename T>
struct Setting
{
    QString name{""};
    T value{};
};

template<typename T>
void writeToXmlNode(const Setting<T> &var,QDomDocument &doc, QDomElement &root)
{
    using namespace XML;

    QDomElement node_varRoot = doc.createElement(XMLProps::Settings::node_settingRoot);
    node_varRoot.setAttribute(XMLProps::Settings::attrs_value,QString("%1").arg(var.value));
    node_varRoot.setAttribute(XMLProps::Settings::attrs_name,var.name);
    root.appendChild(node_varRoot);
}

template<typename T>
void readFromXmlNode(Setting<T> &target,const QDomElement &objectRoot)
{
    using namespace XML;

    auto currentNode {objectRoot.firstChild().toElement()};
    while(!currentNode.isNull())
    {
        if(currentNode.tagName() == XMLProps::Settings::node_settingRoot)
        {
            QString nodeName{currentNode.attribute(XMLProps::Settings::attrs_name,"")};
            if(nodeName == target.name)
            {
                target.value = currentNode.attribute(XMLProps::Settings::attrs_value,T{});
            }
        }
        currentNode = currentNode.nextSiblingElement();
    }
}

}

#endif
