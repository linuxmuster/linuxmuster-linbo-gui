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

    //= dialog_createImage_title
    this->setTitle(tr("Create image"));

    this->mainLayout = new QVBoxLayout(this);
    this->mainLayout->setSpacing(0);

    //= dialog_createImage_action_title
    this->mainLayout->addWidget(new QLabel("<b>" + tr("The action to perfom:") + "</b>"));

    this->actionButtonGroup = new QButtonGroup(this);
    this->actionButtonGroup->setExclusive(true);

    connect(this->actionButtonGroup, SIGNAL(buttonToggled(QAbstractButton*, bool)), this, SLOT(refreshPathAndDescription()));

    //= dialog_createImage_action_current
    LinboRadioButton* replaceImage = new LinboRadioButton(tr("replace current image"));
    this->mainLayout->addWidget(replaceImage);
    this->actionButtonGroup->addButton(replaceImage, 0);

    //= dialog_createImage_action_new
    LinboRadioButton* createNewImage = new LinboRadioButton(tr("create a new image"));
    this->mainLayout->addWidget(createNewImage);
    this->actionButtonGroup->addButton(createNewImage, 1);

    //= dialog_createImage_imageName
    this->mainLayout->addWidget(new QLabel("<b>" + tr("Image name:") + "</b>"));

    this->imageNameLineEdit = new LinboLineEdit();
    this->mainLayout->addWidget(this->imageNameLineEdit);

    //= dialog_createImage_imageDescription
    this->mainLayout->addWidget(new QLabel("<b>" + tr("Image description:") + "</b>"));
    this->imageDescriptionTextBrowser = new LinboTextBrowser();
    this->mainLayout->addWidget(this->imageDescriptionTextBrowser);
    this->imageDescriptionTextBrowser->setReadOnly(false);

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

    // Bottom buttons

    //= dialog_createImage_button_create
    LinboToolButton* pushButtonCache = new LinboToolButton(tr("create"));
    this->addToolButton(pushButtonCache);
    pushButtonCache->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(pushButtonCache, &LinboToolButton::clicked, [=](){
        this->createImage(LinboBackend::LinboPostProcessAction(this->postProcessActionButtonGroup->checkedId()));
    });

    //= dialog_createImage_button_createAndUpload
    pushButtonCache = new LinboToolButton(tr("create + upload"));
    this->addToolButton(pushButtonCache);
    pushButtonCache->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(pushButtonCache, &LinboPushButton::clicked, [=](){
        LinboBackend::LinboPostProcessActions postProcessActions = LinboBackend::LinboPostProcessAction(this->postProcessActionButtonGroup->checkedId());
        postProcessActions.setFlag(LinboBackend::NoAction, false);
        this->createImage( LinboBackend::UploadImage | postProcessActions);
    });

    //= cancel
    pushButtonCache = new LinboToolButton(tr("cancel"));
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
