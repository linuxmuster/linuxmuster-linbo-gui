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
    this->_backend = backend;

    //% "Please enter password:"
    this->setTitle(qtTrId("dialog_login_title"));

    this->_passwordInput = new LinboLineEdit();
    this->_passwordInput->setEchoMode(QLineEdit::NoEcho);
    this->_passwordInput->setAlignment(Qt::AlignCenter);
    connect(_passwordInput, &LinboLineEdit::returnPressed, this, &LinboLoginDialog::inputFinished);

    //% "Please enter password:"
    connect(_passwordInput, &QLineEdit::textChanged, this, [=] {this->setTitle(qtTrId("dialog_login_title"));});

    //% "cancel"
    LinboToolButton* toolButtonCache = new LinboToolButton(qtTrId("cancel"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, &LinboToolButton::clicked, this, &LinboLoginDialog::close);

    //% "login"
    toolButtonCache = new LinboToolButton(qtTrId("dialog_login_button_login"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, &LinboToolButton::clicked, this, &LinboLoginDialog::inputFinished);

    this->_mainLayout = new QVBoxLayout(this);
    this->_mainLayout->setContentsMargins(0,0,0,0);
    this->_mainLayout->addStretch();
    this->_mainLayout->addWidget(this->_passwordInput);
    this->_mainLayout->setAlignment(this->_passwordInput, Qt::AlignCenter);
    this->_mainLayout->addStretch();
    this->_mainLayout->setAlignment(Qt::AlignCenter);
}

void LinboLoginDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    this->_passwordInput->setFixedHeight(gTheme->size(LinboTheme::RowHeight));
    this->_passwordInput->setFixedWidth(this->width() - gTheme->size(LinboTheme::Margins) * 2);
}


void LinboLoginDialog::inputFinished() {

    if(this->_backend->login(this->_passwordInput->text())) {
        this->_passwordInput->clear();
        this->close();
    }
    else {
        this->_passwordInput->clear();
        //% "Wrong password!"
        this->setTitle(qtTrId("dialog_login_title_wrong"));
    }

}

void LinboLoginDialog::setVisibleAnimated(bool visible) {
    if(!visible) {
        this->_passwordInput->clear();
    }
    else {
        this->_passwordInput->setFocus();
    }
    LinboDialog::setVisibleAnimated(visible);
}
