/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020-2021  Dorian Zedler <dorian@itsblue.de>
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Affero General Public License as published
 ** by the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Affero General Public License for more details.
 **
 ** You should have received a copy of the GNU Affero General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "linboimagecreationdialog.h"

LinboImageCreationDialog::LinboImageCreationDialog(LinboBackend* backend, QWidget* parent) : LinboDialog(parent)
{
    this->backend = backend;

    //% "Create image"
    this->setTitle(qtTrId("dialog_createImage_title"));

    this->mainLayout = new QVBoxLayout(this);
    this->mainLayout->setSpacing(0);

    //% "The action to perfom:"
    this->mainLayout->addWidget(new QLabel("<b>" + qtTrId("dialog_createImage_action_title") + "</b>"));

    this->actionButtonGroup = new QButtonGroup(this);
    this->actionButtonGroup->setExclusive(true);

    connect(this->actionButtonGroup, SIGNAL(buttonToggled(QAbstractButton*, bool)), this, SLOT(refreshPathAndDescription()));

    //% "replace current image"
    LinboRadioButton* replaceImage = new LinboRadioButton(qtTrId("dialog_createImage_action_current"));
    this->mainLayout->addWidget(replaceImage);
    this->actionButtonGroup->addButton(replaceImage, 0);

    //% "create a new image"
    LinboRadioButton* createNewImage = new LinboRadioButton(qtTrId("dialog_createImage_action_new"));
    this->mainLayout->addWidget(createNewImage);
    this->actionButtonGroup->addButton(createNewImage, 1);

    //% "Image name:"
    this->mainLayout->addWidget(new QLabel("<b>" + qtTrId("dialog_createImage_imageName") + "</b>"));

    this->imageNameLineEdit = new LinboLineEdit();
    this->mainLayout->addWidget(this->imageNameLineEdit);

    //% "Image description:"
    this->mainLayout->addWidget(new QLabel("<b>" + qtTrId("dialog_createImage_imageDescription") + "</b>"));
    this->imageDescriptionTextBrowser = new LinboTextBrowser();
    this->mainLayout->addWidget(this->imageDescriptionTextBrowser);
    this->imageDescriptionTextBrowser->setReadOnly(false);

    // Post process actions
    //% "What to do after the process has finished?"
    mainLayout->addWidget(new QLabel("<b>" + qtTrId("dialog_createImage_postActionQuestion") + "</b>"));

    this->postProcessActionButtonGroup = new QButtonGroup(this);
    this->postProcessActionButtonGroup->setExclusive(true);

    this->postProcessActionLayout = new QHBoxLayout();
    this->mainLayout->addLayout(this->postProcessActionLayout);

    //% "nothing"
    LinboRadioButton* buttonCache = new LinboRadioButton(qtTrId("dialog_createImage_postaction_nothing"));
    buttonCache->setChecked(true);
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache, LinboBackend::NoAction);

    //% "shutdown"
    buttonCache = new LinboRadioButton(qtTrId("dialog_createImage_postaction_shutdown"));
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache, LinboBackend::Shutdown);

    //% "reboot"
    buttonCache = new LinboRadioButton(qtTrId("dialog_createImage_postaction_reboot"));
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache, LinboBackend::Reboot);

    //% "logout"
    buttonCache = new LinboRadioButton(qtTrId("dialog_createImage_postaction_logout"));
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache, LinboBackend::Logout);

    // Bottom buttons

    //% "create"
    LinboToolButton* pushButtonCache = new LinboToolButton(qtTrId("dialog_createImage_button_create"));
    this->addToolButton(pushButtonCache);
    pushButtonCache->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(pushButtonCache, &LinboToolButton::clicked, [=](){
        this->createImage(LinboBackend::LinboPostProcessAction(this->postProcessActionButtonGroup->checkedId()));
    });

    //% "create + upload"
    pushButtonCache = new LinboToolButton(qtTrId("dialog_createImage_button_createAndUpload"));
    this->addToolButton(pushButtonCache);
    pushButtonCache->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(pushButtonCache, &LinboPushButton::clicked, [=](){
        LinboBackend::LinboPostProcessActions postProcessActions = LinboBackend::LinboPostProcessAction(this->postProcessActionButtonGroup->checkedId());
        postProcessActions.setFlag(LinboBackend::NoAction, false);
        this->createImage( LinboBackend::UploadImage | postProcessActions);
    });

    //% cancel
    pushButtonCache = new LinboToolButton(qtTrId("cancel"));
    this->addToolButton(pushButtonCache);
    pushButtonCache->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(pushButtonCache, SIGNAL(clicked()), this, SLOT(autoClose()));

    connect(this, &LinboDialog::opened, [=]{ this->refreshPathAndDescription(true); });
}

void LinboImageCreationDialog::createImage(LinboBackend::LinboPostProcessActions postProcessActions) {

    if(this->actionButtonGroup->checkedId() == 0)
        // replace image
        this->backend->replaceImageOfCurrentOs(this->imageDescriptionTextBrowser->toPlainText(), postProcessActions);
    else
        // create new image
        this->backend->createImageOfCurrentOS(this->imageNameLineEdit->text(), this->imageDescriptionTextBrowser->toPlainText(), postProcessActions);

    this->autoClose();
}

void LinboImageCreationDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int margins = gTheme->getSize(LinboGuiTheme::Margins);

    this->mainLayout->setContentsMargins(margins, margins, margins, margins);

    for(int i = 0; i < 12; i++) {
        QWidget* item;

        if(i < 8)
            item = this->mainLayout->itemAt(i)->widget();
        else
            item = this->postProcessActionLayout->itemAt(i-8)->widget();

        if(i != 6) {
            if(i % 2 == 0)
                item->setFixedSize(this->width() - margins * 2, gTheme->getSize(LinboGuiTheme::RowLabelHeight));
            else
                item->setFixedSize(this->width() - margins * 2, gTheme->getSize(LinboGuiTheme::RowHeight));
        }

        QFont buttonFont = item->font();
        buttonFont.setPixelSize(gTheme->getSize(LinboGuiTheme::RowFontSize));
        item->setFont(buttonFont);
    }
}

void LinboImageCreationDialog::refreshPathAndDescription(bool isOpening) {
    if(this->backend->getCurrentOs()->getBaseImage() == nullptr) {
        this->actionButtonGroup->buttons()[1]->setChecked(true);
        this->actionButtonGroup->buttons()[0]->setChecked(false);
        this->actionButtonGroup->buttons()[0]->setEnabled(false);
    }
    else {
        this->actionButtonGroup->buttons()[0]->setEnabled(true);
        if(isOpening) {
            this->actionButtonGroup->buttons()[0]->setChecked(true);
            this->actionButtonGroup->buttons()[1]->setChecked(false);
        }
    }

    if(this->actionButtonGroup->checkedId() == 0) {
        this->imageNameLineEdit->setText(this->backend->getCurrentOs()->getBaseImage()->getName());
        this->imageDescriptionTextBrowser->setText(this->backend->getCurrentOs()->getBaseImage()->getDescription());
    }
    else {
        this->imageNameLineEdit->setText("");
        this->imageDescriptionTextBrowser->setText("");
    }
}
