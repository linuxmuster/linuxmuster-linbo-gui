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

protected:
    void keyPressEvent(QKeyEvent * event) override;

private:
    int _fixedPosition;
    int _currentHistoryIndex;
    bool _doNotExitOnProcessExit;
    QString _commandBeforeHistorySwitch;
    QStringList _commandHistory;
    QProcess* _process;

public slots:
    void clearAndRestart();

private slots:
    QString _getCurrentCommand();
    void _setCurrentCommand(QString command);
    void _handleCursorPositionChanged();
    void _readOutput();
    void _handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void _execute(QString command);
    void _restartProcess();

signals:
    void processExited();
};

#endif // LINBOTERMINAL_H
