#include <QString>
#include <QList>
#include "FunctionDescription/functiondesc.h"
#include "UserProject/UserProject.h"
#include "UserProject/xmlListManagement.h"
#include "UserProject/xmlPropsProj.h"

#include <QDebug>
#define cout qDebug()

namespace UserProject
{

void writeProjectToFile(const UserProject &proj,QString iDocType,QString iPath)
{
    //create Doc
    QDomImplementation domImpl = QDomDocument().implementation();
    QDomDocument doc(iDocType);
    QDomProcessingInstruction instruction=doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");

    QDomElement rootNode = doc.createElement(XML::XMLProps::UserProject::node_userProjectRoot);
    rootNode.setAttribute(XML::XMLProps::UserProject::attrs_pyFile,proj.associatedPyFile);

    doc.appendChild(rootNode);

    doc.insertBefore(instruction,rootNode);
    //doc created with root node and header

    XML::writeObjectListToXMLNode/*<PyDesc::FunctionDesc>*/(proj.funcList,doc,rootNode);


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

}
