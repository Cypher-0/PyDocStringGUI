#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "functiondesc.h"
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void argsSwitchRows(int startRow,int endRow);
    void returnArgsSwitchRows(int startRow,int endRow);

    void actArgsListView();
    void actReturnArgsListView();

    void actFunctionListBox(int newIndex = -1);
    void actFuncDescAndName();

    void oneElementModified();



    void on_act_enableOutput_toggled(bool arg1);

    void on_tw_args_cellChanged(int row, int column);

    void on_tw_return_cellChanged(int row, int column);

    void on_pb_argsAdd_clicked();

    void on_pb_argsMinus_clicked();

    void on_pb_returnArgsAdd_clicked();

    void on_pb_returnArgsMinus_clicked();

    void on_te_desc_textChanged();

    void on_le_funcName_textChanged(const QString &arg1);


    void on_dw_output_visibilityChanged(bool visible);

    void on_actionEnregistrer_sous_triggered();


    void on_cb_funcSelec_currentIndexChanged(int index);

    void on_pb_funcAdd_clicked();

private:
    Ui::MainWindow *ui;

    int m_currentFunc{0};
    QList<PyDesc::FunctionDesc> m_funcList{};
};
#endif // MAINWINDOW_H
