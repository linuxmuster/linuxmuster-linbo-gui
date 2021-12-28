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
    this->_backend = backend;

    //% "Create image"
    this->setTitle(qtTrId("dialog_createImage_title"));

    this->_mainLayout = new QVBoxLayout(this);
    this->_mainLayout->setSpacing(0);

    //% "The action to perfom:"
    this->_mainLayout->addWidget(new QLabel("<b>" + qtTrId("dialog_createImage_action_title") + "</b>"));

    this->_actionButtonGroup = new QButtonGroup(this);
    this->_actionButtonGroup->setExclusive(true);

    connect(this->_actionButtonGroup, &QButtonGroup::buttonToggled, this, &LinboImageCreationDialog::_refreshPathAndDescription);

    //% "replace current image"
    LinboRadioButton* replaceImage = new LinboRadioButton(qtTrId("dialog_createImage_action_current"));
    this->_mainLayout->addWidget(replaceImage);
    this->_actionButtonGroup->addButton(replaceImage, 0);

    //% "create a new image"
    LinboRadioButton* createNewImage = new LinboRadioButton(qtTrId("dialog_createImage_action_new"));
    this->_mainLayout->addWidget(createNewImage);
    this->_actionButtonGroup->addButton(createNewImage, 1);

    //% "Image name:"
    this->_mainLayout->addWidget(new QLabel("<b>" + qtTrId("dialog_createImage_imageName") + "</b>"));

    this->_imageNameLineEdit = new LinboLineEdit();
    this->_mainLayout->addWidget(this->_imageNameLineEdit);

    //% "Image description:"
    this->_mainLayout->addWidget(new QLabel("<b>" + qtTrId("dialog_createImage_imageDescription") + "</b>"));
    this->_imageDescriptionTextBrowser = new LinboTextBrowser();
    this->_imageDescriptionTextBrowser->setTabChangesFocus(true);
    this->_mainLayout->addWidget(this->_imageDescriptionTextBrowser);
    this->_imageDescriptionTextBrowser->setReadOnly(false);

    // Post process actions
    //% "What to do after the process has finished?"
    _mainLayout->addWidget(new QLabel("<b>" + qtTrId("dialog_createImage_postActionQuestion") + "</b>"));

    this->_postProcessActionButtonGroup = new QButtonGroup(this);
    this->_postProcessActionButtonGroup->setExclusive(true);

    this->_postProcessActionLayout = new QHBoxLayout();
    this->_mainLayout->addLayout(this->_postProcessActionLayout);

    //% "nothing"
    LinboRadioButton* buttonCache = new LinboRadioButton(qtTrId("dialog_createImage_postaction_nothing"));
    buttonCache->setChecked(true);
    this->_postProcessActionLayout->addWidget(buttonCache);
    this->_postProcessActionButtonGroup->addButton(buttonCache, LinboPostProcessActions::NoAction);

    //% "shutdown"
    buttonCache = new LinboRadioButton(qtTrId("dialog_createImage_postaction_shutdown"));
    this->_postProcessActionLayout->addWidget(buttonCache);
    this->_postProcessActionButtonGroup->addButton(buttonCache, LinboPostProcessActions::Shutdown);

    //% "reboot"
    buttonCache = new LinboRadioButton(qtTrId("dialog_createImage_postaction_reboot"));
    this->_postProcessActionLayout->addWidget(buttonCache);
    this->_postProcessActionButtonGroup->addButton(buttonCache, LinboPostProcessActions::Reboot);

    //% "logout"
    buttonCache = new LinboRadioButton(qtTrId("dialog_createImage_postaction_logout"));
    this->_postProcessActionLayout->addWidget(buttonCache);
    this->_postProcessActionButtonGroup->addButton(buttonCache, LinboPostProcessActions::Logout);

    // Bottom buttons

    //% "create"
    LinboToolButton* pushButtonCache = new LinboToolButton(qtTrId("dialog_createImage_button_create"));
    this->addToolButton(pushButtonCache);
    pushButtonCache->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(pushButtonCache, &LinboToolButton::clicked, this, [=]() {
        this->_createImage(LinboPostProcessActions::Flags(this->_postProcessActionButtonGroup->checkedId()));
    });

    //% "create + upload"
    pushButtonCache = new LinboToolButton(qtTrId("dialog_createImage_button_createAndUpload"));
    this->addToolButton(pushButtonCache);
    pushButtonCache->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(pushButtonCache, &LinboPushButton::clicked, this, [=]() {
        LinboPostProcessActions::Flags postProcessActions = LinboPostProcessActions::Flags(this->_postProcessActionButtonGroup->checkedId());
        postProcessActions.setFlag(LinboPostProcessActions::NoAction, false);
        this->_createImage( LinboPostProcessActions::UploadImage | postProcessActions);
    });

    //% cancel
    pushButtonCache = new LinboToolButton(qtTrId("cancel"));
    this->addToolButton(pushButtonCache);
    pushButtonCache->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(pushButtonCache, &LinboToolButton::clicked, this, &LinboImageCreationDialog::autoClose);

    connect(this, &LinboDialog::opened, [=] { this->_refreshPathAndDescription(true); });
}

void LinboImageCreationDialog::open(LinboOs* os) {
    this->_targetOs = os;
    LinboDialog::open();
}

void LinboImageCreationDialog::_createImage(LinboPostProcessActions::Flags postProcessActions) {

    if(this->_actionButtonGroup->checkedId() == 0)
        // replace image
        this->_targetOs->replaceImage(this->_imageDescriptionTextBrowser->toPlainText(), postProcessActions);
    else
        // create new image
        this->_targetOs->createImage(this->_imageNameLineEdit->text(), this->_imageDescriptionTextBrowser->toPlainText(), postProcessActions);

    this->autoClose();
}

void LinboImageCreationDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int margins = gTheme->size(LinboTheme::Margins);

    this->_mainLayout->setContentsMargins(margins, margins, margins, margins);

    for(int i = 0; i < 12; i++) {
        QWidget* item;

        if(i < 8)
            item = this->_mainLayout->itemAt(i)->widget();
        else
            item = this->_postProcessActionLayout->itemAt(i-8)->widget();

        if(i != 6) {
            if(i % 2 == 0)
                item->setFixedSize(this->width() - margins * 2, gTheme->size(LinboTheme::RowLabelHeight));
            else
                item->setFixedSize(this->width() - margins * 2, gTheme->size(LinboTheme::RowHeight));
        }

        QFont buttonFont = item->font();
        buttonFont.setPixelSize(gTheme->size(LinboTheme::RowFontSize));
        item->setFont(buttonFont);
    }
}

void LinboImageCreationDialog::_refreshPathAndDescription(bool isOpening) {
    if(this->_targetOs->baseImage() == nullptr) {
        this->_actionButtonGroup->buttons().at(1)->setChecked(true);
        this->_actionButtonGroup->buttons().at(0)->setChecked(false);
        this->_actionButtonGroup->buttons().at(0)->setEnabled(false);
    }
    else {
        this->_actionButtonGroup->buttons().at(0)->setEnabled(true);
        if(isOpening) {
            this->_actionButtonGroup->buttons().at(0)->setChecked(true);
            this->_actionButtonGroup->buttons().at(1)->setChecked(false);
        }
    }

    if(this->_actionButtonGroup->checkedId() == 0) {
        this->_imageNameLineEdit->setEnabled(false);
        this->_imageNameLineEdit->setText(this->_targetOs->baseImage()->name());
        this->_imageDescriptionTextBrowser->setText(this->_targetOs->baseImage()->getDescription());
    }
    else {
        this->_imageNameLineEdit->setEnabled(true);
        this->_imageNameLineEdit->setText("");
        this->_imageDescriptionTextBrowser->setText("");
    }
}
