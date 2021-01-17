#ifndef LINBOIMAGEUPLOADDIALOG_H
#define LINBOIMAGEUPLOADDIALOG_H

#include <QObject>
#include <QButtonGroup>
#include <QComboBox>

#include "linbodialog.h"
#include "linbobackend.h"
#include "linboradiobutton.h"
#include "linbotoolbutton.h"
#include "linbocombobox.h"

class LinboImageUploadDialog : public LinboDialog
{
    Q_OBJECT
public:
    LinboImageUploadDialog(LinboBackend* backend, QWidget* parent);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void refreshImageList();

private:
    LinboBackend* backend;

    QVBoxLayout* mainLayout;

    QComboBox* imageSelectBox;

    QButtonGroup* postProcessActionButtonGroup;
    QHBoxLayout* postProcessActionLayout;
};

#endif // LINBOIMAGEUPLOADDIALOG_H
