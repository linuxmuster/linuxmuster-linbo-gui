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

#include "linboimageuploaddialog.h"

LinboImageUploadDialog::LinboImageUploadDialog(LinboBackend* backend, QWidget* parent) : LinboDialog(parent)
{
    this->_backend = backend;

    //% "Upload image"
    this->setTitle(qtTrId("dialog_uploadImage_title"));

    this->_mainLayout = new QVBoxLayout(this);
    this->_mainLayout->addStretch();

    //% "The image to upload:"
    this->_mainLayout->addWidget(new QLabel("<b>" + qtTrId("dialog_uploadImage_selection_title") + "</b>"));

    this->_imageSelectBox = new LinboComboBox();

    this->_mainLayout->addWidget(this->_imageSelectBox);

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

    this->_mainLayout->addStretch();

    // Toolbuttons

    //% "upload"
    this->_uploadButton = new LinboToolButton(qtTrId("dialog_uploadImage_button_upload"));
    this->addToolButton(this->_uploadButton);
    connect(this->_uploadButton, &LinboPushButton::clicked, this, [=]() {
        LinboPostProcessActions::Flags postProcessActions = LinboPostProcessActions::Flags(this->_postProcessActionButtonGroup->checkedId());
        LinboImage* image = this->_backend->config()->getImageByName(this->_imageSelectBox->currentText());
        if(image != nullptr) {
            image->upload(postProcessActions);
            this->autoClose();
        }
    });

    //% "cancel"
    LinboToolButton* toolButtonCache = new LinboToolButton(qtTrId("cancel"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, &LinboToolButton::clicked, this, &LinboImageUploadDialog::autoClose);

    connect(this, &LinboImageUploadDialog::opened, this, &LinboImageUploadDialog::refreshImageList);
}

void LinboImageUploadDialog::open(LinboOs* os) {
    this->_targetOs = os;
    LinboDialog::open();
}

void LinboImageUploadDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int margins = gTheme->size(LinboTheme::Margins);

    this->_mainLayout->setContentsMargins(margins, margins, margins, margins);
    this->_mainLayout->setSpacing(margins * 0.5);

    for(int i = 1; i < 8; i++) {
        QWidget* item;

        if(i < 4)
            item = this->_mainLayout->itemAt(i)->widget();
        else
            item = this->_postProcessActionLayout->itemAt(i-4)->widget();

        // make lables smaller
        if(i != 2)
            item->setFixedSize(this->width() - margins * 2, gTheme->size(LinboTheme::RowLabelHeight));
        else
            item->setFixedSize(this->width() - margins * 2, gTheme->size(LinboTheme::RowHeight));

        QFont itemFont = item->font();
        itemFont.setPixelSize(gTheme->size(LinboTheme::RowFontSize));
        item->setFont(itemFont);
    }
}

void LinboImageUploadDialog::refreshImageList() {
    this->_imageSelectBox->clear();

    bool imagesWereFound = false;
    for(LinboImage* image : this->_backend->config()->getImagesOfOs(this->_targetOs, true, false)) {
        this->_imageSelectBox->addItem(image->name());
        imagesWereFound = true;
    }

    if(!imagesWereFound) {
        //% "No image found"
        this->_imageSelectBox->addItem(qtTrId("dialog_uploadImage_noImages"));
        this->_imageSelectBox->setEnabled(false);
        this->_uploadButton->setEnabled(false);
    }
    else {
        this->_uploadButton->setEnabled(true);
    }
}
