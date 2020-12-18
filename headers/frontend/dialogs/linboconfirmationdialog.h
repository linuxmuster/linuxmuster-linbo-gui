#ifndef LINBOCONFIRMATIONDIALOG_H
#define LINBOCONFIRMATIONDIALOG_H

#include <QObject>
#include <QWidget>

#include "qmoderndialog.h"
#include "qmodernpushbutton.h"

class LinboConfirmationDialog : public QModernDialog
{
    Q_OBJECT
public:
    LinboConfirmationDialog(QString title, QString question, QWidget* parent);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QModernPushButton* yesButton;
    QModernPushButton* noButton;
    QLabel* questionLabel;

signals:
    void accepted();
    void rejected();
};

#endif // LINBOCONFIRMATIONDIALOG_H
