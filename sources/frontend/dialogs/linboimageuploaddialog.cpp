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
    this->backend = backend;

    //= dialog_uploadImage_title
    this->setTitle(tr("Upload image"));

    this->mainLayout = new QVBoxLayout(this);
    this->mainLayout->addStretch();

    //= dialog_uploadImage_selection_title
    this->mainLayout->addWidget(new QLabel("<b>" + tr("The image to upload:") + "</b>"));

    this->imageSelectBox = new LinboComboBox();

    this->mainLayout->addWidget(this->imageSelectBox);

    // Post process actions
    //= dialog_createImage_postActionQuestion
    mainLayout->addWidget(new QLabel("<b>" + tr("What to do after the process has finished?") + "</b>"));

    this->postProcessActionButtonGroup = new QButtonGroup(this);
    this->postProcessActionButtonGroup->setExclusive(true);

    this->postProcessActionLayout = new QHBoxLayout();
    this->mainLayout->addLayout(this->postProcessActionLayout);

    //= dialog_createImage_postaction_nothing
    LinboRadioButton* buttonCache = new LinboRadioButton(tr("nothing"));
    buttonCache->setChecked(true);
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache, LinboBackend::NoAction);

    //= dialog_createImage_postaction_shutdown
    buttonCache = new LinboRadioButton(tr("shutdown"));
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache, LinboBackend::Shutdown);

    //= dialog_createImage_postaction_reboot
    buttonCache = new LinboRadioButton(tr("reboot"));
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache, LinboBackend::Reboot);

    //= dialog_createImage_postaction_logout
    buttonCache = new LinboRadioButton(tr("logout"));
    this->postProcessActionLayout->addWidget(buttonCache);
    this->postProcessActionButtonGroup->addButton(buttonCache, LinboBackend::Logout);

    this->mainLayout->addStretch();

    // Toolbuttons

    //= dialog_uploadImage_button_upload
    LinboToolButton* toolButtonCache = new LinboToolButton(tr("upload"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, &LinboPushButton::clicked, [=](){
        LinboBackend::LinboPostProcessActions postProcessActions = LinboBackend::LinboPostProcessAction(this->postProcessActionButtonGroup->checkedId());
        this->backend->uploadImage(this->backend->getImageByName(this->imageSelectBox->currentText()), postProcessActions);
        this->autoClose();
    });

    //= cancel
    toolButtonCache = new LinboToolButton(tr("cancel"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(autoClose()));

    connect(this, &LinboImageUploadDialog::opened, this, &LinboImageUploadDialog::refreshImageList);
}

void LinboImageUploadDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int margins = gTheme->getSize(LinboGuiTheme::Margins);

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
            item->setFixedSize(this->width() - margins * 2, gTheme->getSize(LinboGuiTheme::RowLabelHeight));
        else
            item->setFixedSize(this->width() - margins * 2, gTheme->getSize(LinboGuiTheme::RowHeight));

        QFont itemFont = item->font();
        itemFont.setPixelSize(gTheme->getSize(LinboGuiTheme::RowFontSize));
        item->setFont(itemFont);
    }
}

void LinboImageUploadDialog::refreshImageList() {
    this->imageSelectBox->clear();

    for(LinboImage* image : this->backend->getImagesOfOs(this->backend->getCurrentOs(), true))
        this->imageSelectBox->addItem(image->getName());
}
