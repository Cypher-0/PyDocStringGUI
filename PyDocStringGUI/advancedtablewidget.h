#ifndef ADVANCEDTABLEWIDGET_H
#define ADVANCEDTABLEWIDGET_H

#include <QObject>
#include <QTableWidget>
#include <QWidget>
#include <QMouseEvent>

class AdvancedTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    AdvancedTableWidget(QWidget *parent=nullptr);

    int getSelectedRow();

signals:
    void switchRows(int,int);//signal to ask to switch rows from index int (1st) and int (2nd)

protected :
    void mousePressEvent(QMouseEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

private :
};

#endif // ADVANCEDTABLEWIDGET_H
