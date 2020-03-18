#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QList>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>

#include <QDebug>

#include "FunctionDescription/xmlProps.h"
#include "FunctionDescription/functiondesc.h"
#include "FunctionDescription/xmlListManagement.h"
#include "advancedtablewidget.h"

#define cout qDebug()

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->tw_args,SIGNAL(switchRows(int,int)),this,SLOT(argsSwitchRows(int,int)));
    connect(ui->tw_return,SIGNAL(switchRows(int,int)),this,SLOT(returnArgsSwitchRows(int,int)));


    /*QList<PyDesc::Argument> list_args{{"client","ModbusPLC","Automate sur lequel lire le bit"},{"bit","int","Adresse sur laquelle lire le bit"},
                                      {"Value","int","Adresse sur laquelle lire le bit"},{"Value","int","Adresse sur laquelle lire le bit"}};
    QList<PyDesc::Argument> list_returnArgs{{"Index 0","int","Valeur lue sur l'automate"}};
    QList<PyDesc::Argument> list_returnArgs2{{"Index 0","int","Valeur lue sur l'automate"},{"Index 1","str","nom test"}};

    m_funcList.append({"writeBit","Permet d'écrire un bit sur l'automate",list_args,list_returnArgs});
    m_funcList.append({"writeWord","Permet d'écrire un mot sur l'automate",list_args,list_returnArgs2});
    ui->le_funcName->setText("writeBit");
    ui->te_desc->setText("Permet d'écrire un bit sur l'automate");

    XML::writeObjectListToXMLFile(m_funcList,"FunctionDescriptionFile","default.xml");*/

    XML::readObjectListFromXMLFile(m_funcList,m_saveFileDocType,"default.xml");

    actArgsListView();
    actReturnArgsListView();
    actFunctionListBox();

    if(m_funcList.size() == 0)
    {
        ui->gb_funcDesc->setEnabled(false);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::actArgsListView()
{
    ui->tw_args->setColumnCount(3);

    if(m_funcList.isEmpty())
        return;

    auto argsList{&(m_funcList[m_currentFunc].list_args)};

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

    if(m_funcList.isEmpty())
        return;

    auto returnList{&(m_funcList[m_currentFunc].list_returnArgs)};

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

//---------------------------------------------------------------------- SLOTS

    //tableWidget cell changed

void MainWindow::on_tw_args_cellChanged(int row, int column)
{
    auto newText{ui->tw_args->item(row,column)->text()};

    if(m_funcList.isEmpty())
        return;

    auto argsList{&(m_funcList[m_currentFunc].list_args)};

    if(column == 0)
        (*argsList)[row].name = newText;
    else if(column == 1)
        (*argsList)[row].type = newText;
    else if(column == 2)
        (*argsList)[row].desc = newText;

    oneElementModified();
}

void MainWindow::on_tw_return_cellChanged(int row, int column)
{
    auto newText{ui->tw_return->item(row,column)->text()};

    if(m_funcList.isEmpty())
        return;

    auto argsList{&(m_funcList[m_currentFunc].list_returnArgs)};

    (*argsList)[row].name = "Index "+QString::number(row);

    if(column == 0)
        (*argsList)[row].type = newText;
    else if(column == 1)
        (*argsList)[row].desc = newText;

    oneElementModified();
}

    //swap rows

void MainWindow::argsSwitchRows(int startRow,int endRow)
{
    if(m_funcList.isEmpty())
        return;

    auto argsList{&(m_funcList[m_currentFunc].list_args)};

    argsList->swapItemsAt(startRow,endRow);

    actArgsListView();

    oneElementModified();
}

void MainWindow::returnArgsSwitchRows(int startRow,int endRow)
{
    if(m_funcList.isEmpty())
        return;

    auto returnList{&(m_funcList[m_currentFunc].list_returnArgs)};

    returnList->swapItemsAt(startRow,endRow);

    (*returnList)[startRow].name = "Index "+QString::number(startRow);
    (*returnList)[endRow].name = "Index "+QString::number(endRow);

    actReturnArgsListView();

    oneElementModified();
}


    //manage function list view

void MainWindow::actFunctionListBox(int newIndex)
{
    if(ui->cb_funcSelec->count() < 0)
        return;
    ui->cb_funcSelec->clear();
    for(const auto &elem : m_funcList)
    {
        ui->cb_funcSelec->addItem(elem.name);
    }

    if(newIndex < 0)
        return;

    ui->cb_funcSelec->setCurrentIndex(newIndex);
}

void MainWindow::actFuncDescAndName()
{
    if(m_funcList.isEmpty())
        return;

    ui->le_funcName->setText(m_funcList[m_currentFunc].name);
    ui->te_desc->setPlainText(m_funcList[m_currentFunc].desc);
}

void MainWindow::oneElementModified()
{
    if(m_funcList.isEmpty())
        return;

    ui->tb_output->setPlainText(PyDesc::getFormattedDesc(m_funcList[m_currentFunc]));
}

    //add / del rows

        //function arguments
void MainWindow::on_pb_argsAdd_clicked()
{
    if(m_funcList.isEmpty())
    {
        QMessageBox::warning(this,"Erreur","Veuillez créer une nouvelle fonction avant de pouvoir ajouter des éléments");
        return;
    }

    m_funcList[m_currentFunc].list_args.append(PyDesc::Argument{"","",""});
    actArgsListView();
}

void MainWindow::on_pb_argsMinus_clicked()
{
    if(m_funcList.isEmpty())
    {
        return;
    }


    auto selectedRow{ui->tw_args->getSelectedRow()};
    if(selectedRow == -1)
        return;

    m_funcList[m_currentFunc].list_args.removeAt(selectedRow);
    actArgsListView();

    if(std::size(m_funcList[m_currentFunc].list_args) == 0)
    {
        oneElementModified();
    }
}

        //function return values
void MainWindow::on_pb_returnArgsAdd_clicked()
{
    if(m_funcList.isEmpty())
    {
        QMessageBox::warning(this,"Erreur","Veuillez créer une nouvelle fonction avant de pouvoir ajouter des éléments");
        return;
    }

    auto returnList{&(m_funcList[m_currentFunc].list_returnArgs)};

    returnList->append(PyDesc::Argument{"Index "+QString::number(std::size(*returnList)),"",""});
    actReturnArgsListView();
}

void MainWindow::on_pb_returnArgsMinus_clicked()
{
    if(m_funcList.isEmpty())
    {
        return;
    }

    auto selectedRow{ui->tw_return->getSelectedRow()};
    if(selectedRow == -1)
        return;

    m_funcList[m_currentFunc].list_returnArgs.removeAt(selectedRow);

    for(auto i{selectedRow};i < std::size(m_funcList[m_currentFunc].list_returnArgs);i++)
    {
        m_funcList[m_currentFunc].list_returnArgs[i].name = "Index "+QString::number(i);
    }

    actReturnArgsListView();

    if(std::size(m_funcList[m_currentFunc].list_returnArgs) == 0)
    {
        oneElementModified();
    }
}

    //change function text

void MainWindow::on_te_desc_textChanged()
{
    if(m_funcList.isEmpty())
    {
        return;
    }

    m_funcList[m_currentFunc].desc = ui->te_desc->toPlainText();

    oneElementModified();
}

void MainWindow::on_le_funcName_textChanged(const QString &arg1)
{
    if(m_funcList.isEmpty())
    {
        return;
    }

    m_funcList[m_currentFunc].name = arg1;

    ui->cb_funcSelec->setItemText(ui->cb_funcSelec->currentIndex(),arg1);

    oneElementModified();
}

        //misc
void MainWindow::on_dw_output_visibilityChanged(bool visible)
{
    ui->act_enableOutput->setChecked(visible);
}



void MainWindow::on_cb_funcSelec_currentIndexChanged(int index)
{
    if(m_funcList.isEmpty())
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

    actFuncDescAndName();
    actArgsListView();
    actReturnArgsListView();
}

void MainWindow::on_pb_funcAdd_clicked()
{
    auto funcName{QInputDialog::getText(this, "Nouveau", "Rentrez le nom de la nouvelle fonction")};

    if(funcName.isEmpty())
        return;

    m_funcList.append(PyDesc::FunctionDesc{funcName,"",{},{}});

    int newIndex{std::size(m_funcList)-1};

    actFunctionListBox(newIndex);

    m_currentFunc = newIndex;
}

void MainWindow::on_pb_funcMinus_clicked()
{
    auto answer{QMessageBox::question(this,"Confirmation","Voulez-vous vraiment supprimer cette fonction ?")};

    if(answer == QMessageBox::No)
        return;

    m_funcList.removeAt(ui->cb_funcSelec->currentIndex());
    actFunctionListBox();

    if(m_funcList.isEmpty())
    {
        ui->tw_args->setRowCount(0);

        ui->tw_return->setRowCount(0);

        ui->le_funcName->setText("");
        ui->te_desc->setText("");

        ui->tb_output->setText("");
    }
}


// ---------------  Actions

void MainWindow::on_act_enableOutput_toggled(bool arg1)
{
    ui->dw_output->setVisible(arg1);
    actArgsListView();
}

void MainWindow::on_action_saveAs_triggered()
{
    QString saveFile{QFileDialog::getSaveFileName(this,"Enregistrer sous","Untitled",QString("PyDocString file (*.")+QString(PYDESCGUI_FILE_EXT)+");; Tous (*)")};

    if(saveFile.isEmpty())
        return;

    m_currentSavePath = saveFile;

    XML::writeObjectListToXMLFile(m_funcList,m_saveFileDocType,m_currentSavePath);
}

void MainWindow::on_action_open_triggered()
{
    QString openedFile{QFileDialog::getOpenFileName(this,"Ouvrir","",QString("PyDocString file (*.")+QString(PYDESCGUI_FILE_EXT)+");; Tous (*)")};

    if(openedFile.isEmpty())
        return;

    m_currentSavePath = openedFile;
    XML::readObjectListFromXMLFile(m_funcList,m_saveFileDocType,openedFile);

    actFunctionListBox();
    actFuncDescAndName();
    actArgsListView();
    actReturnArgsListView();
}

void MainWindow::on_action_save_triggered()
{
    if(m_currentSavePath.isEmpty())
        on_action_saveAs_triggered();

    if(m_currentSavePath.isEmpty())
        return;
}
