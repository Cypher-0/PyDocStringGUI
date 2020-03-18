#include "Setting.hpp"

#define LA_PANOPLIE

struct Panoplie
{
    Screen screen1;
    Screen screen2;
    Mouse mouse;
    Keyboard keyb;
    Coffee_tea utils;
};


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
