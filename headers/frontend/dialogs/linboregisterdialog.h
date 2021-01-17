#ifndef LINBOREGISTERDIALOG_H
#define LINBOREGISTERDIALOG_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

#include "linbodialog.h"
#include "linbolineedit.h"
#include "linbopushbutton.h"
#include "linbocombobox.h"

#include "linbobackend.h"

class LinboRegisterDialog : public LinboDialog
{
    Q_OBJECT
public:
    LinboRegisterDialog(LinboBackend* backend, QWidget* parent);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    LinboBackend* backend;
    QVBoxLayout* mainLayout;
    LinboLineEdit* roomEdit;
    LinboLineEdit* hostnameEdit;
    LinboLineEdit* ipAddressEdit;
    LinboLineEdit* hostGroupEdit;
    QComboBox* roleSelectBox;

private slots:
    void handleRoomChanged(const QString& newText);
    void registerClient();
};

#endif // LINBOREGISTERDIALOG_H
