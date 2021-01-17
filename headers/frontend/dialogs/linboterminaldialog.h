#ifndef LINBOTERMINALDIALOG_H
#define LINBOTERMINALDIALOG_H

#include <QObject>
#include <QWidget>
#include <QTextBrowser>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QProcess>
#include <QScrollBar>
#include <QKeyEvent>

#include "linbodialog.h"
#include "linbolineedit.h"
#include "linbotextbrowser.h"

class LinboTerminalDialog : public LinboDialog
{
    Q_OBJECT
public:
    LinboTerminalDialog(QWidget* parent);

protected:
    virtual void setVisibleAnimated(bool visible) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    LinboTextBrowser* textBrowser;
    QLineEdit* lineEdit;
    QVBoxLayout* mainLayout;
    QProcess* process;
    QString commandBeforeHistorySwitch;
    QStringList commandHistory;
    int currentHistoryIndex;

protected slots:
    void readOutput();
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void execute();
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // LINBOTERMINALDIALOG_H
