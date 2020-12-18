#ifndef LINBOLOGINDIALOG_H
#define LINBOLOGINDIALOG_H

#include <QDialog>
#include <QObject>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolTip>
#include <QtDebug>

#include "qmoderndialog.h"
#include "qmodernpushbutton.h"
#include "qmodernlineedit.h"
#include "linbobackend.h"

class LinboLoginDialog : public QModernDialog
{
    Q_OBJECT
public:
    LinboLoginDialog(LinboBackend* backend, QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    virtual void setVisibleAnimated(bool visible) override;

private:
    LinboBackend* backend;
    QLabel* headerLabel;
    QLineEdit* passwordInput;
    QWidget* buttonLayoutWidget;

    QModernPushButton* cancelButton;
    QModernPushButton* loginButton;
    QHBoxLayout* buttonLayout;

    QVBoxLayout* mainLayout;

private slots:
    void inputFinished();
};

#endif // LINBOLOGINDIALOG_H
