#include "linboimagecreationdialog.h"

LinboImageCreationDialog::LinboImageCreationDialog(LinboBackend* backend, QWidget* parent) : QModernDialog(parent)
{
    this->backend = backend;

    //= dialog_createImage_title
    this->setTitle(tr("Create image"));

    this->mainLayout = new QVBoxLayout(this);

    //= dialog_createImage_action_title
    this->mainLayout->addWidget(new QLabel("<b>" + tr("The action to perfom:") + "</b>"));

    this->actionButtonGroup = new QButtonGroup(this);
    this->actionButtonGroup->setExclusive(true);

    connect(this->actionButtonGroup, SIGNAL(buttonToggled(QAbstractButton*, bool)), this, SLOT(refreshPathAndDescription()));

    //= dialog_createImage_action_current
    QModernRadioButton* replaceImage = new QModernRadioButton(tr("replace current image"));
    this->mainLayout->addWidget(replaceImage);
    replaceImage->setChecked(true);
    this->actionButtonGroup->addButton(replaceImage, 0);

    //= dialog_createImage_action_new
    QModernRadioButton* createNewImage = new QModernRadioButton(tr("create a new image"));
    this->mainLayout->addWidget(createNewImage);
    this->actionButtonGroup->addButton(createNewImage, 1);

    //= dialog_createImage_imageName
    this->mainLayout->addWidget(new QLabel("<b>" + tr("Image name:") + "</b>"));

    this->imageNameLineEdit = new QModernLineEdit();
    this->mainLayout->addWidget(this->imageNameLineEdit);

    //= dialog_createImage_imageDescription
    this->mainLayout->addWidget(new QLabel("<b>" + tr("Image description:") + "</b>"));
    this->imageDescriptionTextBrowser = new QModernTextBrowser();
    this->mainLayout->addWidget(this->imageDescriptionTextBrowser);
    this->imageDescriptionTextBrowser->setReadOnly(false);

    // Post process actions
    mainLayout->addWidget(new QLabel("<b>" + tr("What to do after the processes has finished?") + "</b>"));

    this->postProcessActionButtonGroup = new QButtonGroup(this);
    this->postProcessActionButtonGroup->setExclusive(true);

    this->postProcessActionLayout = new QHBoxLayout();
    this->mainLayout->addLayout(this->postProcessActionLayout);

    //= dialog_createImage_postaction_nothing
    QModernRadioButton* buttonCache = new QModernRadioButton(tr("nothing"));
    buttonCache->setChecked(true);
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache);

    //= dialog_createImage_postaction_shutdown
    buttonCache = new QModernRadioButton(tr("shutdown"));
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache);

    //= dialog_createImage_postaction_reboot
    buttonCache = new QModernRadioButton(tr("reboot"));
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache);

    //= dialog_createImage_postaction_logout
    buttonCache = new QModernRadioButton(tr("logout"));
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache);

    // Bottom buttons
    this->buttonLayout = new QHBoxLayout();
    mainLayout->addLayout(this->buttonLayout);

    //= dialog_createImage_button_create
    QModernPushButton* pushButtonCache = new QModernPushButton("", tr("create"));
    pushButtonCache->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    this->buttonLayout->addWidget(pushButtonCache);

    //= dialog_createImage_button_createAndUpload
    pushButtonCache = new QModernPushButton("", tr("create + upload"));
    pushButtonCache->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    this->buttonLayout->addWidget(pushButtonCache);

    //= cancel
    pushButtonCache = new QModernPushButton("", tr("cancel"));
    pushButtonCache->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(pushButtonCache, SIGNAL(clicked()), this, SLOT(autoClose()));
    this->buttonLayout->addWidget(pushButtonCache);

    connect(this, SIGNAL(opened()), this, SLOT(refreshPathAndDescription()));
}


void LinboImageCreationDialog::resizeEvent(QResizeEvent *event) {
    QModernDialog::resizeEvent(event);

    int buttonHeight = this->parentWidget()->height() * 0.06;
    int itemHeight = buttonHeight * 0.6;
    int margins = buttonHeight * 0.4;

    this->mainLayout->setContentsMargins(margins, margins, margins, margins);
    this->mainLayout->setSpacing(margins * 0.5);

    for(int i = 0; i < 12; i++) {
        QWidget* item;

        if(i < 8)
            item = this->mainLayout->itemAt(i)->widget();
        else
            item = this->postProcessActionLayout->itemAt(i-8)->widget();

        if(i != 6)
            item->setFixedSize(this->width() - margins * 2, itemHeight);

        QFont buttonFont = item->font();
        buttonFont.setPixelSize(itemHeight <= 0 ? 1:itemHeight * 0.6);
        item->setFont(buttonFont);
    }

    this->buttonLayout->setContentsMargins(0,0,0,0);
    this->buttonLayout->setSpacing(margins / 2);

    for(int i = 0; i < 3; i++) {
        if(i == 1)
            this->buttonLayout->itemAt(i)->widget()->setFixedSize((this->width() - 3 * margins) / 3 * 1.5, buttonHeight);
        else
            this->buttonLayout->itemAt(i)->widget()->setFixedSize((this->width() - 3 * margins) / 3 * 0.75, buttonHeight);
    }
}

void LinboImageCreationDialog::refreshPathAndDescription() {
    if(this->actionButtonGroup->checkedId() == 0)
        this->imageNameLineEdit->setText(this->backend->getCurrentOs()->getBaseImage()->getName());
    else
        this->imageNameLineEdit->setText("");
}
