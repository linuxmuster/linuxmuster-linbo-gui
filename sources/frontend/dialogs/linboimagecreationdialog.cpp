#include "linboimagecreationdialog.h"

LinboImageCreationDialog::LinboImageCreationDialog(LinboBackend* backend, QWidget* parent) : QModernDialog(parent)
{
    this->backend = backend;

    //: This is the title of the image creation dialog
    //= dialog_create_title
    this->setTitle(tr("Create image"));


    this->mainLayout = new QVBoxLayout(this);

    this->mainLayout->addWidget(new QLabel(tr("The action to perfom:")));
    this->mainLayout->addWidget(new QLabel(tr("Hint: If you replace an image, a backup of the current one will be created automatically.")));

    QRadioButton* replaceImage = new QRadioButton(tr("replace current image"));
    this->mainLayout->addWidget(replaceImage);

    QRadioButton* createNewImage = new QRadioButton(tr("create a new image"));
    this->mainLayout->addWidget(createNewImage);

    this->imageNameLineEdit = new QModernLineEdit();
    this->mainLayout->addWidget(this->imageNameLineEdit);

    this->imageDescriptionTextBrowser = new QModernTextBrowser();
    this->mainLayout->addWidget(this->imageDescriptionTextBrowser);

}
