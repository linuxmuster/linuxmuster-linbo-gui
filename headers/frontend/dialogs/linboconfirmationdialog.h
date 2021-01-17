#ifndef LINBOCONFIRMATIONDIALOG_H
#define LINBOCONFIRMATIONDIALOG_H

#include <QObject>
#include <QWidget>

#include "linbodialog.h"
#include "linbotoolbutton.h"

class LinboConfirmationDialog : public LinboDialog
{
    Q_OBJECT
public:
    LinboConfirmationDialog(QString title, QString question, QWidget* parent);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel* questionLabel;

signals:
    void accepted();
    void rejected();
};

#endif // LINBOCONFIRMATIONDIALOG_H
