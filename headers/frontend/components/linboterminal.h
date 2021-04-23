#ifndef LINBOTERMINAL_H
#define LINBOTERMINAL_H

#include <QTextEdit>
#include <QObject>
#include <QWidget>
#include <QKeyEvent>
#include <QtDebug>
#include <QProcess>
#include <QScrollBar>

#include "linboguitheme.h"

class LinboTerminal : public QTextEdit
{
    Q_OBJECT
public:
    LinboTerminal(QWidget* parent = nullptr);

public slots:
    void clearAndRestart();

private:
    int fixedPosition;
    QProcess* process;
    QString commandBeforeHistorySwitch;
    QStringList commandHistory;
    int currentHistoryIndex;
    void keyPressEvent (QKeyEvent * event) override;
    bool doNotExitOnProcessExit;

private slots:
    QString getCurrentCommand();
    void setCurrentCommand(QString command);
    void handleCursorPositionChanged();
    void readOutput();
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void execute(QString command);
    void restartProcess();

signals:
    void processExited();
};

#endif // LINBOTERMINAL_H
