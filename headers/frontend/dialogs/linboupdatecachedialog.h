#ifndef LINBOUPDATECACHEDIALOG_H
#define LINBOUPDATECACHEDIALOG_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QButtonGroup>

#include "linbodialog.h"
#include "linbopushbutton.h"
#include "linboradiobutton.h"
#include "linbocheckbox.h"

#include "linbobackend.h"

class LinboUpdateCacheDialog : public LinboDialog
{
    Q_OBJECT
public:
    LinboUpdateCacheDialog(LinboBackend* backend, QWidget* parent);

    void resizeEvent(QResizeEvent *event) override;

private:
    LinboBackend* backend;
    QButtonGroup* updateTypeButtonGroup;
    QVBoxLayout* mainLayout;
    LinboCheckBox* formatCheckBox;

private slots:
    void updateCache();

};

#endif // LINBOUPDATECACHEDIALOG_H
