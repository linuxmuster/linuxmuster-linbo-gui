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

    connect(this->_actionButtonGroup, &QButtonGroup::buttonToggled, this, [=] { this->_refreshPathAndDescription(false); });

    //% "create new base image"
    LinboRadioButton* newBaseImage = new LinboRadioButton(qtTrId("dialog_createImage_action_base"));
    this->_mainLayout->addWidget(newBaseImage);
    this->_actionButtonGroup->addButton(newBaseImage, BASE_IMAGE);

    //% "create new differential image"
    LinboRadioButton* newDiffImage = new LinboRadioButton(qtTrId("dialog_createImage_action_diff"));
    this->_mainLayout->addWidget(newDiffImage);
    this->_actionButtonGroup->addButton(newDiffImage, DIFF_IMAGE);

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
    connect(pushButtonCache, &LinboToolButton::clicked, this, [=]() {
        this->_createImage(LinboPostProcessActions::Flags(this->_postProcessActionButtonGroup->checkedId()));
    });

    //% "create + upload"
    pushButtonCache = new LinboToolButton(qtTrId("dialog_createImage_button_createAndUpload"));
    this->addToolButton(pushButtonCache);
    connect(pushButtonCache, &LinboPushButton::clicked, this, [=]() {
        LinboPostProcessActions::Flags postProcessActions = LinboPostProcessActions::Flags(this->_postProcessActionButtonGroup->checkedId());
        postProcessActions.setFlag(LinboPostProcessActions::NoAction, false);
        this->_createImage( LinboPostProcessActions::UploadImage | postProcessActions);
    });

    //% cancel
    pushButtonCache = new LinboToolButton(qtTrId("cancel"));
    this->addToolButton(pushButtonCache);
    connect(pushButtonCache, &LinboToolButton::clicked, this, &LinboImageCreationDialog::autoClose);

    connect(this, &LinboDialog::opened, [=] { this->_refreshPathAndDescription(true); });
}

void LinboImageCreationDialog::open(LinboOs* os) {
    this->_targetOs = os;
    LinboDialog::open();
}

void LinboImageCreationDialog::_createImage(LinboPostProcessActions::Flags postProcessActions) {

    switch (this->_actionButtonGroup->checkedId()) {
    case BASE_IMAGE:
        this->_targetOs->createBaseImage(this->_imageDescriptionTextBrowser->toPlainText(), postProcessActions);
        break;
    case DIFF_IMAGE:
        this->_targetOs->createDiffImage(this->_imageDescriptionTextBrowser->toPlainText(), postProcessActions);
        break;
    }

    this->autoClose();
}

void LinboImageCreationDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int margins = gTheme->size(LinboTheme::Margins);

    this->_mainLayout->setContentsMargins(margins, margins, margins, margins);

    for(int i = 0, mainItemCount = 0;; i++) {
        QLayoutItem* item;
        QWidget* widget;

        if(mainItemCount == 0)
            item = this->_mainLayout->itemAt(i);
        else
            item = this->_postProcessActionLayout->itemAt(i-mainItemCount);

        if ((item == nullptr || item->widget() == nullptr) && mainItemCount == 0) {
            mainItemCount = i+1;
            continue;
        }

        if (item == nullptr || item->widget() == nullptr) break;

        widget = item->widget();

        QFont buttonFont = widget->font();
        buttonFont.setPixelSize(gTheme->size(LinboTheme::RowFontSize));
        widget->setFont(buttonFont);

        if(widget == this->_imageDescriptionTextBrowser)
            continue;

        if(i % 2 == 0)
            widget->setFixedSize(this->width() - margins * 2, gTheme->size(LinboTheme::RowLabelHeight));
        else
            widget->setFixedSize(this->width() - margins * 2, gTheme->size(LinboTheme::RowHeight));
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

    this->_imageDescriptionTextBrowser->setText(this->_targetOs->baseImage()->getDescription());
}
