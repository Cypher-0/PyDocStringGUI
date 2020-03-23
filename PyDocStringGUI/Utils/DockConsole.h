#ifndef UTILS_DOCK_CONSOLE_H
#define UTILS_DOCK_CONSOLE_H

#include <QTextBrowser>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QMainWindow>

static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(0);

void DockConsoleMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

class DockConsole : public QDockWidget
{
public:
    explicit DockConsole(QMainWindow *parent = nullptr, int dockArea = 1);
    ~DockConsole();

    static QTextBrowser *console;

private:
    QVBoxLayout *m_lay;
};

#endif
