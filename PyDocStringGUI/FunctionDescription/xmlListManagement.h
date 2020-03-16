#ifndef XMLMANAGEMENT_H
#define XMLMANAGEMENT_H

#include "functiondesc.h"

#include <QDomImplementation>
#include <QDomDocument>
#include <QTextStream>
#include <QFile>

#include <QDebug>

namespace XML
{

/* If a type is used with these functions, there is some constraints to respect :
 * type <T> have to have following functions defined :
 *      - void writeToXml(const T &var,QDomDocument &doc, QDomElement &root);
 *      - T readFromXmlNode(const QDomElement &objectRoot);
 *
 * and a public static member called className of type QString (T::className)
*/


/* Write a QList of type T objectList into an xml file with a doctype "iDocType" into the file iPath
 * /!\ T type have to have :
 *  - a public static member called className of type QString (T::className)
 *  - a public writeToXml(QDomDocument &doc, QDomElement &root) function
*/

template<typename T>
inline static void writeObjectListToXMLNode(const QList<T> &objectList,QDomDocument &doc,QDomElement &rootNode)
{
    for(const auto &curObject : objectList)
    {
        writeToXml(curObject,doc,rootNode);
    }
}


template<typename T>
static void writeObjectListToXMLFile(const QList<T> &objectList,QString iDocType,QString iPath)
{
    //create Doc
    QDomImplementation domImpl = QDomDocument().implementation();
    QDomDocument doc(iDocType);
    QDomProcessingInstruction instruction=doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");

    QDomElement rootNode = doc.createElement(T::className);
    doc.appendChild(rootNode);

    doc.insertBefore(instruction,rootNode);
    //doc created with root node and header

    writeObjectListToXMLNode<T>(objectList,doc,rootNode);


    //target file to save XML Dom Doc
    auto file = QFile(iPath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Cannot open : <" << iPath << ">";
        return;
    }

    QTextStream fileStream(&file);
    doc.save(fileStream,4);

    file.close();
}

/* Read a QList of type T objectList into an xml file with a doctype "iDocType" into the file iPath
 * /!\ T type have to have :
 *  - a public readFromXMLNode(QDomElement &objectRoot) function
*/

template<typename T>
static void readObjectListFromXMLNode(QList<T> &outList,QDomElement &rootNode)
{
    outList.clear();//make sure this list is clear

    QDomElement currentNode = rootNode.firstChild().toElement();

    while(!currentNode.isNull())
    {
        T tempElement{};
        readFromXmlNode(tempElement,currentNode);
        outList.append(tempElement);

        currentNode = currentNode.nextSiblingElement();
    }
}


template<typename T>
static void readObjectListFromXMLFile(QList<T> &outList,QString iDocType,QString iPath)
{
    QDomDocument doc;

    QFile file(iPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open : <" << iPath << ">";
        return;
    }

    //load datas into the dom doc
    doc.setContent(&file);
    file.close();

    QDomElement rootNode {doc.documentElement()};

    if(
            doc.doctype().name() != iDocType || //if the file is not corresponding to expecting one
            rootNode.tagName() != T::className
      )
    {
        qDebug() << "Cannot read XML file : root node name not corresponding or doctype not corresponding";
    }

    readObjectListFromXMLNode<T>(outList,rootNode);
}

}


#endif // XMLMANAGEMENT_H
