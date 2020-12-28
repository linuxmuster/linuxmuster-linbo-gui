#include "linboimagecreationdialog.h"

LinboImageCreationDialog::LinboImageCreationDialog(LinboBackend* backend, QWidget* parent) : QModernDialog(parent)
{
    this->backend = backend;

    //= dialog_create_title
    this->setTitle(tr("Create image"));

    this->mainLayout = new QVBoxLayout(this);

    //= dialog_create_action_title
    this->mainLayout->addWidget(new QLabel(tr("The action to perfom:")));

    //= dialog_create_action_hint
    this->mainLayout->addWidget(new QLabel(tr("Hint: If you replace an image, a backup of the current one will be created automatically.")));

    this->actionButtonGroup = new QButtonGroup(this);
    this->actionButtonGroup->setExclusive(true);

    //= dialog_create_action_current
    QRadioButton* replaceImage = new QRadioButton(tr("replace current image"));
    this->mainLayout->addWidget(replaceImage);
    replaceImage->setChecked(true);
    this->actionButtonGroup->addButton(replaceImage, 0);

    //= dialog_create_action_new
    QRadioButton* createNewImage = new QRadioButton(tr("create a new image"));
    this->mainLayout->addWidget(createNewImage);
    this->actionButtonGroup->addButton(createNewImage, 1);

    this->imageNameLineEdit = new QModernLineEdit();
    this->mainLayout->addWidget(this->imageNameLineEdit);

    this->imageDescriptionTextBrowser = new QModernTextBrowser();
    this->mainLayout->addWidget(this->imageDescriptionTextBrowser);
    this->imageDescriptionTextBrowser->setReadOnly(false);

    connect(this, SIGNAL(opened()), this, SLOT(refreshPathAndDescription()));
    connect(this->actionButtonGroup, SIGNAL(buttonToggled(QAbstractButton*, bool)), this, SLOT(refreshPathAndDescription()));
}

void LinboImageCreationDialog::refreshPathAndDescription() {
    if(this->actionButtonGroup->checkedId() == 0)
        this->imageNameLineEdit->setText(this->backend->getCurrentOs()->getBaseImage()->getName());
    else
        this->imageNameLineEdit->setText("");
}
