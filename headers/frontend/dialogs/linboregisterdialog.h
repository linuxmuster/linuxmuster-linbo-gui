#ifndef LINBOREGISTERDIALOG_H
#define LINBOREGISTERDIALOG_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

#include "qmoderndialog.h"
#include "qmodernlineedit.h"
#include "qmodernpushbutton.h"

#include "linbobackend.h"

class LinboRegisterDialog : public QModernDialog
{
    Q_OBJECT
public:
    LinboRegisterDialog(LinboBackend* backend, QWidget* parent);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    LinboBackend* backend;
    QVBoxLayout* mainLayout;
    QModernLineEdit* roomEdit;
    QModernLineEdit* hostnameEdit;
    QModernLineEdit* ipAddressEdit;
    QModernLineEdit* hostGroupEdit;
    QComboBox* roleSelectBox;
    QHBoxLayout* buttonLayout;

private slots:
    void handleRoomChanged(const QString& newText);
    void registerClient();
};

#endif // LINBOREGISTERDIALOG_H
