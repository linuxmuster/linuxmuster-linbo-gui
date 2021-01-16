#ifndef LINBOIMAGEUPLOADDIALOG_H
#define LINBOIMAGEUPLOADDIALOG_H

#include <QObject>
#include <QButtonGroup>
#include <QComboBox>

#include "qmoderndialog.h"
#include "linbobackend.h"
#include "qmodernradiobutton.h"

class LinboImageUploadDialog : public QModernDialog
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

    QHBoxLayout* buttonLayout;
};

#endif // LINBOIMAGEUPLOADDIALOG_H
