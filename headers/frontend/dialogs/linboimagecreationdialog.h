#ifndef LINBOIMAGECREATIONDIALOG_H
#define LINBOIMAGECREATIONDIALOG_H

#include <QObject>
#include <QWidget>
#include <QButtonGroup>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>

#include "qmoderndialog.h"
#include "qmodernpushbutton.h"
#include "qmodernlineedit.h"
#include "qmoderntextbrowser.h"

#include "linbobackend.h"

class LinboImageCreationDialog : public QModernDialog
{
    Q_OBJECT
public:
    LinboImageCreationDialog(LinboBackend* backend, QWidget* parent);

protected:
    //void resizeEvent(QResizeEvent *event) override;

private:
    LinboBackend* backend;
    QVBoxLayout* mainLayout;
    QButtonGroup* actionButtonGroup;
    QModernLineEdit* imageNameLineEdit;
    QModernTextBrowser* imageDescriptionTextBrowser;
    QCheckBox* rebootCheckBox;
    QCheckBox* shutDownCheckBox;
    QHBoxLayout* buttonLayout;

private slots:
    //void createImage();
    //void createImageAndUpload();
};

#endif // LINBOIMAGECREATIONDIALOG_H
