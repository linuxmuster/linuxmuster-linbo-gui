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
    this->backend = backend;

    //% "Register client"
    this->setTitle(qtTrId("dialog_register_title"));

    this->mainLayout = new QVBoxLayout(this);

    //% "Room"
    this->mainLayout->addWidget(new QLabel(qtTrId("room")));
    roomEdit = new LinboLineEdit();
    connect(roomEdit, SIGNAL(textChanged(const QString&)), this, SLOT(handleRoomChanged(const QString&)));
    this->mainLayout->addWidget(roomEdit);

    //% "Hostname"
    this->mainLayout->addWidget(new QLabel(qtTrId("hostname")));
    hostnameEdit = new LinboLineEdit();
    this->mainLayout->addWidget(hostnameEdit);

    //% "IP-Address"
    this->mainLayout->addWidget(new QLabel(qtTrId("ip")));
    ipAddressEdit = new LinboLineEdit();
    QString prefilledIp = "";

    QStringList subnetMask = backend->getConfig()->subnetMask().split(".");
    QStringList ipAddress = backend->getConfig()->ipAddress().split(".");

    if(subnetMask.length() == 4 && ipAddress.length() == 4)
        for(int i = 0; i < 4; i++) {
            QString block = subnetMask[i];
            if(block == "255") {
                prefilledIp.append(ipAddress[i] + ".");
            }
        }
    ipAddressEdit->setText(prefilledIp);
    this->mainLayout->addWidget(ipAddressEdit);

    //% "Host group"
    this->mainLayout->addWidget(new QLabel(qtTrId("group")));
    hostGroupEdit = new LinboLineEdit();
    this->mainLayout->addWidget(hostGroupEdit);

    //% "Client role"
    this->mainLayout->addWidget(new QLabel(qtTrId("dialog_register_clientRole")));
    this->roleSelectBox = new LinboComboBox();

    //% "Classroom student computer"
    this->roleSelectBox->addItem(qtTrId("dialog_register_clientRole_classroomStudent"), LinboConfig::ClassroomStudentComputerRole);

    //% "Classroom teacher computer"
    this->roleSelectBox->addItem(qtTrId("dialog_register_clientRole_classroomTeacher"), LinboConfig::ClassroomTeacherComputerRole);

    //% "Faculty teacher computer"
    this->roleSelectBox->addItem(qtTrId("dialog_register_clientRole_facultyTeacher"), LinboConfig::FacultyTeacherComputerRole);

    //% "Staff computer"
    this->roleSelectBox->addItem(qtTrId("dialog_register_clientRole_staffComputer"), LinboConfig::StaffComputerRole);

    this->mainLayout->addWidget(this->roleSelectBox);

    this->mainLayout->addStretch();

    //% "register"
    LinboToolButton* toolButtonCache = new LinboToolButton(qtTrId("dialog_register_button_resgister"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(registerClient()));

    //% "cancel"
    toolButtonCache = new LinboToolButton(qtTrId("cancel"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(autoClose()));
}

void LinboRegisterDialog::registerClient() {
    this->backend->registerClient(this->roomEdit->text(), this->hostnameEdit->text(), this->ipAddressEdit->text(), this->hostGroupEdit->text(), LinboConfig::LinboDeviceRole(this->roleSelectBox->currentData().toInt()));
    this->setVisibleAnimated(false);
}

void LinboRegisterDialog::handleRoomChanged(const QString& newText) {
    QString currentHostnameText = this->hostnameEdit->text();
    currentHostnameText.replace("-", "");
    if(this->hostnameEdit->text().isEmpty() || newText.startsWith(currentHostnameText) || (currentHostnameText.startsWith(newText) && this->hostnameEdit->text().endsWith("-")))
        this->hostnameEdit->setText(newText + "-");

    if(newText.isEmpty() && this->hostnameEdit->text() == "-")
        this->hostnameEdit->setText("");
}

void LinboRegisterDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int margins = gTheme->getSize(LinboGuiTheme::Margins);

    this->mainLayout->setContentsMargins(margins, margins, margins, margins);

    for(int i = 0; i < 10; i++) {
        QWidget* item = this->mainLayout->itemAt(i)->widget();

        // make lables smaller
        if(i % 2 == 0)
            item->setFixedSize(this->width() - margins * 2, gTheme->getSize(LinboGuiTheme::RowLabelHeight));
        else
            item->setFixedSize(this->width() - margins * 2, gTheme->getSize(LinboGuiTheme::RowHeight));

        QFont font = item->font();
        font.setPixelSize(gTheme->getSize(LinboGuiTheme::RowFontSize));

        // make lables bold
        if(i % 2 == 0)
            font.setBold(true);

        item->setFont(font);
    }
}
