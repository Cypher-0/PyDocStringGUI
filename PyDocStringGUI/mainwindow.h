#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "FunctionDescription/functiondesc.h"
#include "ProgSettings/Setting.hpp"
#include "UserProject/UserProject.h"
#include <QList>
#include <QLabel>
#include <QDir>

#define PYDESCGUI_FILE_EXT "pdsgui"

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

    void actArgsListView();
    void actReturnArgsListView();

    void loadFromFile(QString path);

    void argsSwitchRows(int startRow,int endRow);//switch args rows
    void returnArgsSwitchRows(int startRow,int endRow); //switch return args rows

    void actFunctionListBox(int newIndex = -1);
    void actFuncDescAndName();

    void oneElementModified();


    void on_tw_args_cellChanged(int row, int column);
    void on_tw_return_cellChanged(int row, int column);

    void on_pb_argsAdd_clicked();
    void on_pb_argsMinus_clicked();

    void on_pb_returnArgsAdd_clicked();
    void on_pb_returnArgsMinus_clicked();

    void on_te_desc_textChanged();

    void on_le_funcName_textChanged(const QString &arg1);


    void on_dw_output_visibilityChanged(bool visible);


    void on_cb_funcSelec_currentIndexChanged(int index);
    void on_pb_funcAdd_clicked();
    void on_pb_funcMinus_clicked();

    void on_cb_savePyFileBeforeExport_stateChanged(int arg1);
    void on_toolBut_pyFile_clicked();

    //actions

    void on_act_enableOutput_toggled(bool arg1);

    void on_action_saveAs_triggered();

    void on_action_open_triggered();

    void on_action_save_triggered();

    void on_action_initFromPyFiles_triggered();

    void on_action_exportToPyFile_triggered();

    void on_action_loadUknownFuncFromPyFile_triggered();

private:
    Ui::MainWindow *ui;

    int m_currentFunc{0};
    //QList<PyDesc::FunctionDesc> m_funcList{};
    UserProject::UserProject m_userProj{};

    //save

    inline static const QString m_saveFileDocType{"FunctionDescriptionFile"};
    QString m_currentSavePath{""};
    bool m_firstSaveAfterStart{true};

    QLabel *m_lbl_currentPath;//status bar label displaying current file
    inline static const QString m_defaultFile{QDir::currentPath()+"/Default.xml"};

    void saveProjectToFile(QString path);
    void loadProjectFromFile(QString path);

    //settings
    QString m_settingPath{QDir::currentPath()+"/settings.xml"};
    QString m_settingDocType{"ProgSettings"};

    Config::Setting<QString> m_set_lastFile{{"lastFile"},{""}};
    Config::Setting<bool> m_set_savePyFileBeforeExport{{"savePyBeforeExport"},true};


    void readSettings();
    void saveSettings();

    //load from Python
    QString m_lastPyFile{};

    //OTHERS

    void setSavePath(QString newPath);
    void refreshAllViews();
};
#endif // MAINWINDOW_H
