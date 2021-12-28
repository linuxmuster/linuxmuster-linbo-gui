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

#include "linboregisterdialog.h"

LinboRegisterDialog::LinboRegisterDialog(LinboBackend* backend, QWidget* parent) : LinboDialog(parent)
{
    this->_backend = backend;

    //% "Register client"
    this->setTitle(qtTrId("dialog_register_title"));

    this->_mainLayout = new QVBoxLayout(this);

    //% "Room"
    this->_mainLayout->addWidget(new QLabel(qtTrId("room")));
    _roomEdit = new LinboLineEdit();
    connect(_roomEdit, &LinboLineEdit::textChanged, this, &LinboRegisterDialog::_handleRoomChanged);
    this->_mainLayout->addWidget(_roomEdit);

    //% "Hostname"
    this->_mainLayout->addWidget(new QLabel(qtTrId("hostname")));
    _hostnameEdit = new LinboLineEdit();
    this->_mainLayout->addWidget(_hostnameEdit);

    //% "IP-Address"
    this->_mainLayout->addWidget(new QLabel(qtTrId("ip")));
    _ipAddressEdit = new LinboLineEdit();
    QString prefilledIp = "";

    QStringList subnetMask = backend->config()->subnetMask().split(".");
    QStringList ipAddress = backend->config()->ipAddress().split(".");

    if(subnetMask.length() == 4 && ipAddress.length() == 4)
        for(int i = 0; i < 4; i++) {
            QString block = subnetMask[i];
            if(block == "255") {
                prefilledIp.append(ipAddress[i] + ".");
            }
        }
    _ipAddressEdit->setText(prefilledIp);
    this->_mainLayout->addWidget(_ipAddressEdit);

    //% "Host group"
    this->_mainLayout->addWidget(new QLabel(qtTrId("group")));
    _hostGroupEdit = new LinboLineEdit();
    this->_mainLayout->addWidget(_hostGroupEdit);

    //% "Client role"
    this->_mainLayout->addWidget(new QLabel(qtTrId("dialog_register_clientRole")));
    this->_roleSelectBox = new LinboComboBox();

    //% "Classroom student computer"
    this->_roleSelectBox->addItem(qtTrId("dialog_register_clientRole_classroomStudent"), LinboConfig::ClassroomStudentComputerRole);

    //% "Classroom teacher computer"
    this->_roleSelectBox->addItem(qtTrId("dialog_register_clientRole_classroomTeacher"), LinboConfig::ClassroomTeacherComputerRole);

    //% "Faculty teacher computer"
    this->_roleSelectBox->addItem(qtTrId("dialog_register_clientRole_facultyTeacher"), LinboConfig::FacultyTeacherComputerRole);

    //% "Staff computer"
    this->_roleSelectBox->addItem(qtTrId("dialog_register_clientRole_staffComputer"), LinboConfig::StaffComputerRole);

    this->_mainLayout->addWidget(this->_roleSelectBox);

    this->_mainLayout->addStretch();

    //% "register"
    LinboToolButton* toolButtonCache = new LinboToolButton(qtTrId("dialog_register_button_resgister"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, &LinboToolButton::clicked, this, &LinboRegisterDialog::_registerClient);

    //% "cancel"
    toolButtonCache = new LinboToolButton(qtTrId("cancel"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, &LinboToolButton::clicked, this, &LinboRegisterDialog::autoClose);
}

void LinboRegisterDialog::_registerClient() {
    this->_backend->registerClient(this->_roomEdit->text(), this->_hostnameEdit->text(), this->_ipAddressEdit->text(), this->_hostGroupEdit->text(), LinboConfig::LinboDeviceRole(this->_roleSelectBox->currentData().toInt()));
    this->setVisibleAnimated(false);
}

void LinboRegisterDialog::_handleRoomChanged(const QString& newText) {
    QString currentHostnameText = this->_hostnameEdit->text();
    currentHostnameText.replace("-", "");
    if(this->_hostnameEdit->text().isEmpty() || newText.startsWith(currentHostnameText) || (currentHostnameText.startsWith(newText) && this->_hostnameEdit->text().endsWith("-")))
        this->_hostnameEdit->setText(newText + "-");

    if(newText.isEmpty() && this->_hostnameEdit->text() == "-")
        this->_hostnameEdit->setText("");
}

void LinboRegisterDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int margins = gTheme->size(LinboTheme::Margins);

    this->_mainLayout->setContentsMargins(margins, margins, margins, margins);

    for(int i = 0; i < 10; i++) {
        QWidget* item = this->_mainLayout->itemAt(i)->widget();

        // make lables smaller
        if(i % 2 == 0)
            item->setFixedSize(this->width() - margins * 2, gTheme->size(LinboTheme::RowLabelHeight));
        else
            item->setFixedSize(this->width() - margins * 2, gTheme->size(LinboTheme::RowHeight));

        QFont font = item->font();
        font.setPixelSize(gTheme->size(LinboTheme::RowFontSize));

        // make lables bold
        if(i % 2 == 0)
            font.setBold(true);

        item->setFont(font);
    }
}
