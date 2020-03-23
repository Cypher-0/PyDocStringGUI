#include "DockConsole.h"

#include <QVBoxLayout>

#include <QDebug>
#define cout qDebug()

void DockConsoleMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if(DockConsole::console != nullptr)
    {
        QByteArray localMsg = msg.toLocal8Bit();
        QString txt;
        switch (type)
        {
        case QtDebugMsg:
            txt = QString("%1").arg(msg);
            break;
        case QtInfoMsg:
            txt = QString("Infos : %1").arg(msg);
            break;
        case QtWarningMsg:
            txt = QString("Warning : %1").arg(msg);
            break;
        case QtCriticalMsg:
            txt = QString("Critical : %1").arg(msg);
            break;
        case QtFatalMsg:
            txt = QString("Fatal : %1").arg(msg);
            break;
        }

        DockConsole::console->append(txt);
    }

    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
}


QTextBrowser* DockConsole::console = nullptr;

DockConsole::DockConsole(QMainWindow *parent,int dockArea) : QDockWidget(parent)
{
    this->setFeatures(QDockWidget::AllDockWidgetFeatures);

    auto m_dwContent{new QWidget{}};

    m_lay = new QVBoxLayout{m_dwContent};
    console = new QTextBrowser{m_dwContent};

    m_lay->addWidget(console);

    this->setWidget(m_dwContent);

    parent->addDockWidget(static_cast<Qt::DockWidgetArea>(dockArea), this);

    qInstallMessageHandler(DockConsoleMessageHandler);
}

DockConsole::~DockConsole()
{
    console->deleteLater();
    m_lay->deleteLater();
}
