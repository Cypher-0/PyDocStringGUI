#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QList>
#include <QInputDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDateTime>

#include <QDebug>

#include "FunctionDescription/xmlProps.h"
#include "FunctionDescription/functiondesc.h"
#include "UserProject/xmlListManagement.h"
#include "advancedtablewidget.h"
#include "PythonParser/PythonParser.hpp"

#include "ProgSettings/Setting.hpp"

#include "UserProject/UserProject.h"
#include "Utils/DockConsole.h"

#define cout qDebug()


MainWindow::MainWindow(const QStringList paramList, QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_console = new DockConsole(this,2);

    cout << "~~~~~~~~~~~ INIT : " << __func__ << " ~~~~~~~~~~~";

    connect(ui->tw_args,SIGNAL(switchRows(int,int)),this,SLOT(argsSwitchRows(int,int)));
    connect(ui->tw_return,SIGNAL(switchRows(int,int)),this,SLOT(returnArgsSwitchRows(int,int)));

    XML::readObjectListFromXMLFile(m_userProj.funcList,m_saveFileDocType,m_defaultFile);

    actArgsListView();
    actReturnArgsListView();
    actFunctionListBox();

    if(m_userProj.funcList.size() == 0)
    {
        ui->gb_funcDesc->setEnabled(false);
    }

    m_lbl_currentPath = new QLabel{};
    ui->statusbar->addPermanentWidget(m_lbl_currentPath);

    readSettings();

    auto paramListSize{std::size(paramList)};//get number of args passed to the prog

    QString paramPath{}; //we consider that all args given are only one path splitted with spaces
    if(paramListSize > 1)
    {
        paramPath += paramList[1];
        for(int i = 2; i < paramListSize; i++)
        {
            paramPath += " "+paramList[i];
        }
    }

    if(!paramPath.isEmpty())//if there is a file arg given to the program
    {
        QFileInfo tempFile{paramPath};
        if(tempFile.exists())//if the designed file exist
        {
            if(paramPath.indexOf(QRegularExpression{".py$"}) != -1) //if it's a python file
            {
                cout << "Opening Python file : <"<<paramPath<<">";
                m_userProj.funcList = PyDesc::PyFileParser::findFunctions(paramPath);
                m_userProj.associatedPyFile = paramPath;
                refreshAllViews();
            }
            else
            {
                cout << "Opening file : <"<<paramPath<<">";
                loadProjectFromFile(paramPath);
            }

            m_currentSavePath = "";//say that the project is unsaved
        }
    }
    else//if there is only the executable arg passed to the program
    {
        cout << "Last file : <" <<m_set_lastFile.value << ">";
        m_currentSavePath = m_set_lastFile.value;

        if(m_currentSavePath.isEmpty())
            loadProjectFromFile(m_defaultFile);
        else
            loadProjectFromFile(m_currentSavePath);
    }

    //-------------- TESTS

    /*QList<PyDesc::Argument> list_args{{"client","ModbusPLC","Automate sur lequel lire le bit"},{"bit","int","Adresse sur laquelle lire le bit"},
                                      {"Value","int","Adresse sur laquelle lire le bit"},{"Value","int","Adresse sur laquelle lire le bit"}};
    QList<PyDesc::Argument> list_returnArgs{{"Index 0","int","Valeur lue sur l'automate"}};
    QList<PyDesc::Argument> list_returnArgs2{{"Index 0","int","Valeur lue sur l'automate"},{"Index 1","str","nom test"}};

    m_funcList.append({"writeBit","Permet d'écrire un bit sur l'automate",list_args,list_returnArgs});
    m_funcList.append({"writeWord","Permet d'écrire un mot sur l'automate",list_args,list_returnArgs2});
    ui->le_funcName->setText("writeBit");
    ui->te_desc->setText("Permet d'écrire un bit sur l'automate");

    XML::writeObjectListToXMLFile(m_funcList,"FunctionDescriptionFile","default.xml");*/

    //QString pyFile{"/home/mathieu/Documents/TRAVAIL/S4P/Supervision/s4p_supervision/Sources/PLC/Variables.py"};
    //PyDesc::PyFileParser::findFunctionsIndexes(pyFile);


    cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
}

MainWindow::~MainWindow()
{
    saveSettings();

    delete ui;
    delete m_lbl_currentPath;
    delete m_console;
}

//ACCESSORS

void MainWindow::setSavePath(QString newPath)
{
    if(newPath.isEmpty())
        return;

    m_currentSavePath = newPath;
    m_set_lastFile.value = m_currentSavePath;

    m_lbl_currentPath->setText(QFileInfo{m_currentSavePath}.fileName());
    ui->statusbar->setToolTip(m_currentSavePath);
}

void MainWindow::refreshAllViews()
{
    actFunctionListBox();
    actFuncDescAndName();
    actArgsListView();
    actReturnArgsListView();

    ui->le_pyFile->setText(m_userProj.associatedPyFile);
    ui->le_pyFile->setToolTip(m_userProj.associatedPyFile);
}

//MISC

    //project loading/saving
//load from file

void MainWindow::saveProjectToFile(QString path)
{
    UserProject::writeProjectToFile(m_userProj,m_saveFileDocType,path);
}

void MainWindow::loadProjectFromFile(QString path)
{
    if(path.isEmpty())
        return;

    setSavePath(path);
    m_userProj = UserProject::readProjectFromFile(m_saveFileDocType,path);

    refreshAllViews();
}

    //settings

void MainWindow::readSettings()
{
    QDomDocument doc;

    QFile file(m_settingPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open : <" << m_settingPath << ">   (" << __func__<<")";
        return;
    }

    //load datas into the dom doc
    doc.setContent(&file);
    file.close();

    QDomElement rootNode {doc.documentElement()};

    if(
            doc.doctype().name() != m_settingDocType || //if the file is not corresponding to expecting one
            rootNode.tagName() != XML::XMLProps::Settings::node_settingsListRoot
      )
    {
        qDebug() << "Cannot read XML file : root node name not corresponding or doctype not corresponding";
    }

    Config::readFromXmlNode(m_set_lastFile,rootNode);

    Config::readFromXmlNode(m_set_savePyFileBeforeExport,rootNode);
    ui->cb_savePyFileBeforeExport->setChecked(m_set_savePyFileBeforeExport.value);
}

void MainWindow::saveSettings()
{
    using namespace XML;
    //create Doc
    QDomImplementation domImpl = QDomDocument().implementation();
    QDomDocument doc(m_settingDocType);
    QDomProcessingInstruction instruction=doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");

    QDomElement rootNode = doc.createElement(XMLProps::Settings::node_settingsListRoot);
    doc.appendChild(rootNode);

    doc.insertBefore(instruction,rootNode);
    //doc created with root node and header

    Config::writeToXmlNode(m_set_lastFile,doc,rootNode);
    Config::writeToXmlNode(m_set_savePyFileBeforeExport,doc,rootNode);


    //target file to save XML Dom Doc
    auto file = QFile(m_settingPath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Cannot open : <" << m_settingPath << ">   ("<<__func__<<")";
        return;
    }

    QTextStream fileStream(&file);
    doc.save(fileStream,4);

    file.close();
}


    // refresh view

void MainWindow::actArgsListView()
{
    ui->tw_args->setColumnCount(3);

    if(m_userProj.funcList.isEmpty())
        return;

    auto argsList{&(m_userProj.funcList[m_currentFunc].list_args)};

    auto size{std::size(*argsList)};
    ui->tw_args->setRowCount(size);

    for (int i = 0; i < size;i++)
    {
        auto item_name = new QTableWidgetItem{argsList->at(i).name};
        auto item_type = new QTableWidgetItem{argsList->at(i).type};
        auto item_desc = new QTableWidgetItem{argsList->at(i).desc};
        ui->tw_args->setItem(i,0,item_name);
        ui->tw_args->setItem(i,1,item_type);
        ui->tw_args->setItem(i,2,item_desc);
    }
}

void MainWindow::actReturnArgsListView()
{
    ui->tw_args->setColumnCount(3);

    if(m_userProj.funcList.isEmpty())
        return;

    auto returnList{&(m_userProj.funcList[m_currentFunc].list_returnArgs)};

    auto size{std::size(*returnList)};
    ui->tw_return->setRowCount(size);

    for (int i = 0; i < size;i++)
    {
        auto item_type = new QTableWidgetItem{returnList->at(i).type};
        auto item_desc = new QTableWidgetItem{returnList->at(i).desc};
        ui->tw_return->setItem(i,0,item_type);
        ui->tw_return->setItem(i,1,item_desc);
    }
}

    //swap rows

//switch args rows
void MainWindow::argsSwitchRows(int startRow,int endRow)
{
    if(m_userProj.funcList.isEmpty())
        return;

    auto argsList{&(m_userProj.funcList[m_currentFunc].list_args)};

    argsList->swap(startRow,endRow);

    actArgsListView();

    oneElementModified();
}

//switch return args rows
void MainWindow::returnArgsSwitchRows(int startRow,int endRow)
{
    if(m_userProj.funcList.isEmpty())
        return;

    auto returnList{&(m_userProj.funcList[m_currentFunc].list_returnArgs)};

    returnList->swap(startRow,endRow);

    (*returnList)[startRow].name = "Index "+QString::number(startRow);
    (*returnList)[endRow].name = "Index "+QString::number(endRow);

    actReturnArgsListView();

    oneElementModified();
}


    //manage function list view

void MainWindow::actFunctionListBox(int newIndex)
{
    if(ui->cb_funcSelec->count() > 0)
        ui->cb_funcSelec->clear();

    for(const auto &elem : m_userProj.funcList)
    {
        ui->cb_funcSelec->addItem(elem.name);
    }

    if(newIndex < 0 || std::size(m_userProj.funcList) == 0)
        return;

    ui->cb_funcSelec->setCurrentIndex(newIndex);
}

void MainWindow::actFuncDescAndName()
{
    if(m_userProj.funcList.isEmpty())
        return;

    ui->le_funcName->setText(m_userProj.funcList[m_currentFunc].name);
    ui->te_desc->setPlainText(m_userProj.funcList[m_currentFunc].desc);
}

void MainWindow::oneElementModified()
{
    if(m_userProj.funcList.isEmpty())
        return;

    ui->tb_output->setPlainText(PyDesc::getFormattedDesc(m_userProj.funcList[m_currentFunc]));
}
//---------------------------------------------------------------------- Qt SLOTS

    //tableWidget cell changed

void MainWindow::on_tw_args_cellChanged(int row, int column)
{
    auto newText{ui->tw_args->item(row,column)->text()};


    if(m_userProj.funcList.isEmpty())
        return;

    auto argsList{&(m_userProj.funcList[m_currentFunc].list_args)};

    if(column == 0)
        (*argsList)[row].name = newText;
    else if(column == 1)
        (*argsList)[row].type = newText;
    else if(column == 2)
    {
        (*argsList)[row].desc = newText;
        ui->tw_args->item(row,column)->setToolTip(newText);
    }

    oneElementModified();
}

void MainWindow::on_tw_return_cellChanged(int row, int column)
{
    auto newText{ui->tw_return->item(row,column)->text()};

    if(m_userProj.funcList.isEmpty())
        return;

    auto argsList{&(m_userProj.funcList[m_currentFunc].list_returnArgs)};

    (*argsList)[row].name = "Index "+QString::number(row);

    if(column == 0)
        (*argsList)[row].type = newText;
    else if(column == 1)
    {
        (*argsList)[row].desc = newText;
        ui->tw_args->item(row,column)->setToolTip(newText);
    }

    oneElementModified();
}


    //add / del rows

        //function arguments
void MainWindow::on_pb_argsAdd_clicked()
{
    if(m_userProj.funcList.isEmpty())
    {
        QMessageBox::warning(this,"Erreur","Veuillez créer une nouvelle fonction avant de pouvoir ajouter des éléments");
        return;
    }

    m_userProj.funcList[m_currentFunc].list_args.append(PyDesc::Argument{"","",""});
    actArgsListView();
}

void MainWindow::on_pb_argsMinus_clicked()
{
    if(m_userProj.funcList.isEmpty())
    {
        return;
    }


    auto selectedRow{ui->tw_args->getSelectedRow()};
    if(selectedRow == -1)
        return;

    m_userProj.funcList[m_currentFunc].list_args.removeAt(selectedRow);
    actArgsListView();

    if(std::size(m_userProj.funcList[m_currentFunc].list_args) == 0)
    {
        oneElementModified();
    }
}

        //function return values
void MainWindow::on_pb_returnArgsAdd_clicked()
{
    if(m_userProj.funcList.isEmpty())
    {
        QMessageBox::warning(this,"Erreur","Veuillez créer une nouvelle fonction avant de pouvoir ajouter des éléments");
        return;
    }

    auto returnList{&(m_userProj.funcList[m_currentFunc].list_returnArgs)};

    returnList->append(PyDesc::Argument{"Index "+QString::number(std::size(*returnList)),"",""});
    actReturnArgsListView();
}

void MainWindow::on_pb_returnArgsMinus_clicked()
{
    if(m_userProj.funcList.isEmpty())
    {
        return;
    }

    auto selectedRow{ui->tw_return->getSelectedRow()};
    if(selectedRow == -1)
        return;

    m_userProj.funcList[m_currentFunc].list_returnArgs.removeAt(selectedRow);

    for(auto i{selectedRow};i < std::size(m_userProj.funcList[m_currentFunc].list_returnArgs);i++)
    {
        m_userProj.funcList[m_currentFunc].list_returnArgs[i].name = "Index "+QString::number(i);
    }

    actReturnArgsListView();

    if(std::size(m_userProj.funcList[m_currentFunc].list_returnArgs) == 0)
    {
        oneElementModified();
    }
}

    //change function text

void MainWindow::on_te_desc_textChanged()
{
    if(m_userProj.funcList.isEmpty())
    {
        return;
    }

    auto tempDesc{ui->te_desc->toPlainText()};
    m_userProj.funcList[m_currentFunc].desc = tempDesc;

    oneElementModified();
}

void MainWindow::on_le_funcName_textChanged(const QString &arg1)
{
    if(m_userProj.funcList.isEmpty())
    {
        return;
    }

    m_userProj.funcList[m_currentFunc].name = arg1;

    ui->cb_funcSelec->setItemText(ui->cb_funcSelec->currentIndex(),arg1);

    oneElementModified();
}

    //out window
void MainWindow::on_dw_output_visibilityChanged(bool visible)
{
    ui->act_enableOutput->setChecked(visible);
}


    //function management

void MainWindow::on_cb_funcSelec_currentIndexChanged(int index)
{
    if(m_userProj.funcList.isEmpty())
    {
        ui->gb_funcDesc->setEnabled(false);

        return;
    }
    else
    {
        ui->gb_funcDesc->setEnabled(true);
    }

    if(index < 0)
        return;

    m_currentFunc = index;

    actArgsListView();
    actFuncDescAndName();
    actReturnArgsListView();
}

void MainWindow::on_pb_funcAdd_clicked()
{
    auto funcName{QInputDialog::getText(this, "Nouveau", "Rentrez le nom de la nouvelle fonction")};

    if(funcName.isEmpty())
        return;

    m_userProj.funcList.append(PyDesc::FunctionDesc{funcName,"",{},{}});

    int newIndex{std::size(m_userProj.funcList)-1};

    actFunctionListBox(newIndex);

    m_currentFunc = newIndex;
}

void MainWindow::on_pb_funcMinus_clicked()
{
    auto answer{QMessageBox::question(this,"Confirmation","Voulez-vous vraiment supprimer cette fonction ?")};

    if(answer == QMessageBox::No)
        return;

    m_userProj.funcList.removeAt(ui->cb_funcSelec->currentIndex());
    actFunctionListBox();

    if(m_userProj.funcList.isEmpty())
    {
        ui->tw_args->setRowCount(0);

        ui->tw_return->setRowCount(0);

        ui->le_funcName->setText("");
        ui->te_desc->setText("");

        ui->tb_output->setText("");
    }
}

    //Python

void MainWindow::on_cb_savePyFileBeforeExport_stateChanged(int arg1)
{
    m_set_savePyFileBeforeExport.value = bool(arg1);
}

void MainWindow::on_toolBut_pyFile_clicked()
{
    auto basePath{(m_userProj.associatedPyFile.isEmpty())?QFileInfo(m_currentSavePath).absoluteDir().path():QFileInfo(m_userProj.associatedPyFile).absoluteDir().path()};
    QString pyFile{QFileDialog::getOpenFileName(this,"Ouvrir",basePath,QString("python file (*.py);; Tous (*)"))};

    if(pyFile.isEmpty())
        return;

    m_userProj.associatedPyFile = pyFile;

    ui->le_pyFile->setText(pyFile);
}


// ---------------  Actions

void MainWindow::on_act_enableOutput_toggled(bool arg1)
{
    ui->dw_output->setVisible(arg1);
    //actArgsListView();
}

void MainWindow::on_action_saveAs_triggered()
{
    QString saveFile{QFileDialog::getSaveFileName(this,"Enregistrer sous",QFileInfo(m_currentSavePath).absoluteDir().path()+"/Untitled."+QString(PYDESCGUI_FILE_EXT),QString("PyDocString file (*.")+QString(PYDESCGUI_FILE_EXT)+");; Tous (*)")};

    if(saveFile.isEmpty())
        return;

    m_firstSaveAfterStart = false;

    if(!saveFile.endsWith(QString{"."}+PYDESCGUI_FILE_EXT))
    {
        saveFile += QString{"."}+PYDESCGUI_FILE_EXT;
    }

    setSavePath(saveFile);

    saveProjectToFile(saveFile);

    ui->statusbar->showMessage("Sauvegardé à "+QDateTime::currentDateTime().toString("hh:mm:ss"),300000);
}

void MainWindow::on_action_open_triggered()
{
    QString openedFile{QFileDialog::getOpenFileName(this,"Ouvrir",QFileInfo(m_currentSavePath).absoluteDir().path(),QString("PyDocString file (*.")+QString(PYDESCGUI_FILE_EXT)+");; Tous (*)")};

    if(openedFile.isEmpty())
        return;

    m_firstSaveAfterStart = false;
    loadProjectFromFile(openedFile);
}

void MainWindow::on_action_save_triggered()
{
    if(m_currentSavePath.isEmpty())
        on_action_saveAs_triggered();

    if(m_currentSavePath.isEmpty())
        return;

    if(m_firstSaveAfterStart) //if the user is editing the file opened on program start
    {
        //ask for confirmation
        auto answer{QMessageBox::question(this,"Confirmation","Êtes-vous sûr de vouloir écraser le fichier :\n<"+m_currentSavePath+">",QMessageBox::Yes | QMessageBox::No)};

        if(answer == QMessageBox::No) //if the user doesn't want to overwrite the file
            return;

        m_firstSaveAfterStart = false;
    }

    saveProjectToFile(m_currentSavePath);

    ui->statusbar->showMessage("Sauvegardé à "+QDateTime::currentDateTime().toString("hh:mm:ss"),300000);
}

    //Python

void MainWindow::on_action_initFromPyFiles_triggered()
{
    //ask for confirmation
    auto answer{QMessageBox::question(this,"Confirmation","Ceci va réinitialiser le projet actuellement ouvert.\nÊtes-vous sûr de vouloir continuer ?",QMessageBox::Yes | QMessageBox::No)};

    if(answer == QMessageBox::No) //if the user doesn't want to loose unsaved current project
        return;

    auto basePath{(m_lastPyFile.isEmpty())?QFileInfo(m_currentSavePath).absoluteDir().path():m_lastPyFile};
    QString pyFile{QFileDialog::getOpenFileName(this,"Ouvrir",basePath,QString("python file (*.py);; Tous (*)"))};
    if(pyFile.isEmpty())
        return;

    m_lastPyFile = pyFile;

    m_userProj.funcList = PyDesc::PyFileParser::findFunctions(pyFile);
    m_userProj.associatedPyFile = pyFile;
    m_firstSaveAfterStart = true;//ask confirmation on next save

    refreshAllViews();
}

void MainWindow::on_action_exportToPyFile_triggered()
{
    if(m_userProj.associatedPyFile.isEmpty())
        on_toolBut_pyFile_clicked();

    if(m_userProj.associatedPyFile.isEmpty())
        return;

    auto success{PyDesc::PyFileParser::writeFuncDescToPyFile(m_userProj,m_set_savePyFileBeforeExport.value)};

    auto pyFileName{QFileInfo{m_userProj.associatedPyFile}.fileName()};

    if(success)
    {
        ui->statusbar->showMessage("Successfuly exported descriptions to Py file <"+pyFileName+">",300000);
    }
    else
    {
        ui->statusbar->showMessage("Failed exported descriptions to Py file <"+pyFileName+">",300000);
    }
}

void MainWindow::on_action_loadUknownFuncFromPyFile_triggered()
{
    if(m_userProj.associatedPyFile.isEmpty())
        on_toolBut_pyFile_clicked();

    if(m_userProj.associatedPyFile.isEmpty())
        return;

    cout << "~~~~~~~~~~ Loading unknown functions from associated Py File ~~~~~~~~~~\n";

    auto tempList{PyDesc::PyFileParser::findFunctions(m_userProj.associatedPyFile)};

    auto funcAdded{false};

    cout << "Adding following functions : ";

    for(const auto &elem : tempList)
    {
        if(!m_userProj.funcList.contains(elem))
        {
            m_userProj.funcList.append(elem);
            cout << PyDesc::getPrototype(elem);
            funcAdded = true;
        }
    }

    if(!funcAdded)
        cout << "(None)";

    cout << "\n~~~~~~~~~~ END LOADING ~~~~~~~~~~\n";

    refreshAllViews();
}

void MainWindow::on_action_showConsole_triggered()
{
    if(m_console->isHidden())
        m_console->show();
    else
        m_console->hide();
}
