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

#include "linbologindialog.h"

LinboLoginDialog::LinboLoginDialog(LinboBackend* backend, QWidget* parent) : LinboDialog(parent)
{
    this->backend = backend;

    //% "Please enter password:"
    this->setTitle(qtTrId("dialog_login_title"));

    this->passwordInput = new LinboLineEdit();
    this->passwordInput->setEchoMode(QLineEdit::NoEcho);
    this->passwordInput->setAlignment(Qt::AlignCenter);
    connect(passwordInput,SIGNAL(returnPressed()),this,SLOT(inputFinished()));

    //% "cancel"
    LinboToolButton* toolButtonCache = new LinboToolButton(qtTrId("cancel"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(close()));

    //% "login"
    toolButtonCache = new LinboToolButton(qtTrId("dialog_login_button_login"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(inputFinished()));

    this->mainLayout = new QVBoxLayout(this);
    this->mainLayout->setContentsMargins(0,0,0,0);
    this->mainLayout->addStretch();
    this->mainLayout->addWidget(this->passwordInput);
    this->mainLayout->setAlignment(this->passwordInput, Qt::AlignCenter);
    this->mainLayout->addStretch();
    this->mainLayout->setAlignment(Qt::AlignCenter);
}

void LinboLoginDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    this->passwordInput->setFixedHeight(gTheme->getSize(LinboGuiTheme::RowHeight));
    this->passwordInput->setFixedWidth(this->width() - gTheme->getSize(LinboGuiTheme::Margins) * 2);
}


void LinboLoginDialog::inputFinished() {
    if(this->backend->login(this->passwordInput->text())) {
        this->close();
    }
    else {
        //% "Wrong password!"
        this->setTitle(qtTrId("dialog_login_title_wrong"));
    }

    this->passwordInput->clear();
}

void LinboLoginDialog::setVisibleAnimated(bool visible) {
    if(!visible) {
        //% "Please enter password:"
        this->setTitle(qtTrId("dialog_login_title"));
        this->passwordInput->clear();
    }
    else {
        this->passwordInput->setFocus();
    }
    LinboDialog::setVisibleAnimated(visible);
}
