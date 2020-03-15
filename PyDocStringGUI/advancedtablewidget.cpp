#include "advancedtablewidget.h"

#include <QHeaderView>
#include <QDropEvent>

#include <QDebug>
#define cout qDebug()

AdvancedTableWidget::AdvancedTableWidget(QWidget *parent) : QTableWidget(parent)
{
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader()->setStretchLastSection(true);
}

int AdvancedTableWidget::getSelectedRow()
{
    auto selecModel{selectionModel()->selectedRows()};
    return (std::size(selecModel) > 0)?selecModel.at(0).row():-1;
}

void AdvancedTableWidget::mousePressEvent(QMouseEvent *event)
{
    QTableWidget::mousePressEvent(event);
}

void AdvancedTableWidget::dropEvent(QDropEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid())
    {
        return;
    }

    auto selectedRow{getSelectedRow()};

    if(selectedRow == -1)
        return;

    emit switchRows(selectedRow,index.row());
}
