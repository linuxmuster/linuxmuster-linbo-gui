#ifndef LINBOIMAGECREATIONDIALOG_H
#define LINBOIMAGECREATIONDIALOG_H

#include <QObject>
#include <QWidget>
#include <QButtonGroup>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>

#include "linbodialog.h"
#include "linbotoolbutton.h"
#include "linbolineedit.h"
#include "linbotextbrowser.h"
#include "linboradiobutton.h"

#include "linbobackend.h"

class LinboImageCreationDialog : public LinboDialog
{
    Q_OBJECT
public:
    LinboImageCreationDialog(LinboBackend* backend, QWidget* parent);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    LinboBackend* backend;
    QVBoxLayout* mainLayout;
    QButtonGroup* actionButtonGroup;
    LinboLineEdit* imageNameLineEdit;
    LinboTextBrowser* imageDescriptionTextBrowser;

    QButtonGroup* postProcessActionButtonGroup;
    QHBoxLayout* postProcessActionLayout;

private slots:
    void refreshPathAndDescription();
    void createImage(LinboBackend::LinboPostProcessActions postProcessActions);
};

#endif // LINBOIMAGECREATIONDIALOG_H
