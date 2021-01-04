#include "linboimageuploaddialog.h"

LinboImageUploadDialog::LinboImageUploadDialog(LinboBackend* backend, QWidget* parent) : QModernDialog(parent)
{
    this->backend = backend;

    //= dialog_uploadImage_title
    this->setTitle(tr("Upload image"));

    this->mainLayout = new QVBoxLayout(this);
    this->mainLayout->addStretch();

    //= dialog_uploadImage_selection_title
    this->mainLayout->addWidget(new QLabel("<b>" + tr("The image to upload:") + "</b>"));

    this->imageSelectBox = new QComboBox();
    this->imageSelectBox->setStyleSheet(
                "QComboBox {"
                    "border: 0 0 0 0;"
                    "border-bottom: 1px solid #ddd;"
                    "background-color: #f8f8f8;"
                "selection-color: #ffffff;"
                "selection-background-color: #394f5e;"
                "}"
                "QComboBox:focus {"
                    "border-bottom: 1px solid #f59c00;"
                "}"
                );

    this->imageSelectBox->addItem("ubuntu.cloop");
    this->imageSelectBox->addItem("Test2");
    this->mainLayout->addWidget(this->imageSelectBox);

    // Post process actions
    //= dialog_createImage_postActionQuestion
    mainLayout->addWidget(new QLabel("<b>" + tr("What to do after the process has finished?") + "</b>"));

    this->postProcessActionButtonGroup = new QButtonGroup(this);
    this->postProcessActionButtonGroup->setExclusive(true);

    this->postProcessActionLayout = new QHBoxLayout();
    this->mainLayout->addLayout(this->postProcessActionLayout);

    //= dialog_createImage_postaction_nothing
    QModernRadioButton* buttonCache = new QModernRadioButton(tr("nothing"));
    buttonCache->setChecked(true);
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache, LinboBackend::NoAction);

    //= dialog_createImage_postaction_shutdown
    buttonCache = new QModernRadioButton(tr("shutdown"));
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache, LinboBackend::Shutdown);

    //= dialog_createImage_postaction_reboot
    buttonCache = new QModernRadioButton(tr("reboot"));
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache, LinboBackend::Reboot);

    //= dialog_createImage_postaction_logout
    buttonCache = new QModernRadioButton(tr("logout"));
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache, LinboBackend::Logout);

    this->mainLayout->addStretch();

    // Bottom buttons
    this->buttonLayout = new QHBoxLayout();
    mainLayout->addLayout(this->buttonLayout);

    //= dialog_uploadImage_button_upload
    QModernPushButton* pushButtonCache = new QModernPushButton("", tr("upload"));
    pushButtonCache->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(pushButtonCache, &QModernPushButton::clicked, [=](){
    });
    this->buttonLayout->addWidget(pushButtonCache);

    //= cancel
    pushButtonCache = new QModernPushButton("", tr("cancel"));
    pushButtonCache->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(pushButtonCache, SIGNAL(clicked()), this, SLOT(autoClose()));
    this->buttonLayout->addWidget(pushButtonCache);
}

void LinboImageUploadDialog::resizeEvent(QResizeEvent *event) {
    QModernDialog::resizeEvent(event);

    int buttonHeight = this->parentWidget()->height() * 0.06;
    int itemHeight = buttonHeight * 0.6;
    int margins = buttonHeight * 0.4;

    this->mainLayout->setContentsMargins(margins, margins, margins, margins);
    this->mainLayout->setSpacing(margins * 0.5);

    for(int i = 1; i < 8; i++) {
        QWidget* item;

        if(i < 4)
            item = this->mainLayout->itemAt(i)->widget();
        else
            item = this->postProcessActionLayout->itemAt(i-4)->widget();

        // make lables smaller
        if(i != 2)
            item->setFixedSize(this->width() - margins * 2, buttonHeight * 0.6);
        else
            item->setFixedSize(this->width() - margins * 2, buttonHeight* 0.8);

        QFont itemFont = item->font();
        itemFont.setPixelSize(itemHeight <= 0 ? 1:itemHeight * 0.6);
        item->setFont(itemFont);
    }

    this->buttonLayout->setContentsMargins(0,0,0,0);
    this->buttonLayout->setSpacing(margins / 2);

    for(int i = 0; i < 2; i++) {
        this->buttonLayout->itemAt(i)->widget()->setFixedSize(this->width() * 0.5 - margins * 1.5, buttonHeight);
    }
}
