#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QList>
#include <QInputDialog>

#include <QDebug>

#include "xmlProps.h"
#include "functiondesc.h"
#include "xmlManagement.h"
#include "advancedtablewidget.h"

#define cout qDebug()

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->tw_args,SIGNAL(switchRows(int,int)),this,SLOT(argsSwitchRows(int,int)));
    connect(ui->tw_return,SIGNAL(switchRows(int,int)),this,SLOT(returnArgsSwitchRows(int,int)));


    QList<PyDesc::Argument> list_args{{"client","ModbusPLC","Automate sur lequel lire le bit"},{"bit","int","Adresse sur laquelle lire le bit"},
                                      {"Value","int","Adresse sur laquelle lire le bit"},{"Value","int","Adresse sur laquelle lire le bit"}};
    QList<PyDesc::Argument> list_returnArgs{{"Index 0","int","Valeur lue sur l'automate"}};
    QList<PyDesc::Argument> list_returnArgs2{{"Index 0","int","Valeur lue sur l'automate"},{"Index 1","str","nom test"}};

    m_funcList.append({"writeBit","Permet d'écrire un bit sur l'automate",list_args,list_returnArgs});
    m_funcList.append({"writeWord","Permet d'écrire un mot sur l'automate",list_args,list_returnArgs2});
    ui->le_funcName->setText("writeBit");
    ui->te_desc->setText("Permet d'écrire un bit sur l'automate");

    XML::writeObjectListToXMLFile(m_funcList,"FunctionDescriptionFile","default.xml");

    actArgsListView();
    actReturnArgsListView();
    actFunctionListBox();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_act_enableOutput_toggled(bool arg1)
{
    ui->dw_output->setVisible(arg1);
    actArgsListView();
}

void MainWindow::actArgsListView()
{
    ui->tw_args->setColumnCount(3);

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
    auto argsList{&(m_funcList[m_currentFunc].list_args)};

    argsList->swapItemsAt(startRow,endRow);

    actArgsListView();

    oneElementModified();
}

void MainWindow::returnArgsSwitchRows(int startRow,int endRow)
{
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
    ui->le_funcName->setText(m_funcList[m_currentFunc].name);
    ui->te_desc->setPlainText(m_funcList[m_currentFunc].desc);
}

void MainWindow::oneElementModified()
{
    ui->tb_output->setPlainText(PyDesc::getFormattedDesc(m_funcList[m_currentFunc]));
}

    //add / del rows

        //function arguments
void MainWindow::on_pb_argsAdd_clicked()
{
    m_funcList[m_currentFunc].list_args.append(PyDesc::Argument{"","",""});
    actArgsListView();
}

void MainWindow::on_pb_argsMinus_clicked()
{
    auto selectedRow{ui->tw_args->getSelectedRow()};
    if(selectedRow == -1)
        return;

    m_funcList[m_currentFunc].list_args.removeAt(selectedRow);
    actArgsListView();
}

        //function return values
void MainWindow::on_pb_returnArgsAdd_clicked()
{
    auto returnList{&(m_funcList[m_currentFunc].list_returnArgs)};

    returnList->append(PyDesc::Argument{"Index "+QString::number(std::size(*returnList)),"",""});
    actReturnArgsListView();
}

void MainWindow::on_pb_returnArgsMinus_clicked()
{
    auto selectedRow{ui->tw_return->getSelectedRow()};
    if(selectedRow == -1)
        return;

    m_funcList[m_currentFunc].list_returnArgs.removeAt(selectedRow);

    for(auto i{selectedRow};i < std::size(m_funcList[m_currentFunc].list_returnArgs);i++)
    {
        m_funcList[m_currentFunc].list_returnArgs[i].name = "Index "+QString::number(i);
    }

    actReturnArgsListView();
}

    //change function text

void MainWindow::on_te_desc_textChanged()
{
    m_funcList[m_currentFunc].desc = ui->te_desc->toPlainText();

    oneElementModified();
}

void MainWindow::on_le_funcName_textChanged(const QString &arg1)
{
    m_funcList[m_currentFunc].name = arg1;

    oneElementModified();
}

        //misc
void MainWindow::on_dw_output_visibilityChanged(bool visible)
{
    ui->act_enableOutput->setChecked(visible);
}

void MainWindow::on_actionEnregistrer_sous_triggered()
{
    XML::writeObjectListToXMLFile(m_funcList,"FunctionDescriptionFile","default.xml");
}



void MainWindow::on_cb_funcSelec_currentIndexChanged(int index)
{
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
