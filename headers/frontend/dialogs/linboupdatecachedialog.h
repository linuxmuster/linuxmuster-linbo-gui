#ifndef LINBOUPDATECACHEDIALOG_H
#define LINBOUPDATECACHEDIALOG_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QButtonGroup>

#include "qmoderndialog.h"
#include "qmodernpushbutton.h"
#include "qmodernradiobutton.h"
#include "qmoderncheckbox.h"

#include "linbobackend.h"

class LinboUpdateCacheDialog : public QModernDialog
{
    Q_OBJECT
public:
    LinboUpdateCacheDialog(LinboBackend* backend, QWidget* parent);

    void resizeEvent(QResizeEvent *event) override;

private:
    LinboBackend* backend;
    QButtonGroup* updateTypeButtonGroup;
    QVBoxLayout* mainLayout;
    QModernCheckBox* formatCheckBox;
    QHBoxLayout* buttonLayout;

private slots:
    void updateCache();

};

#endif // LINBOUPDATECACHEDIALOG_H
