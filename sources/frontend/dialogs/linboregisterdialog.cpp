#include "linboregisterdialog.h"

LinboRegisterDialog::LinboRegisterDialog(LinboBackend* backend, QWidget* parent) : QModernDialog(parent)
{
    this->backend = backend;

    //= dialog_register_title
    this->setTitle(tr("Register client"));

    this->mainLayout = new QVBoxLayout(this);

    //= room
    this->mainLayout->addWidget(new QLabel(tr("Room")));
    roomEdit = new QModernLineEdit();
    connect(roomEdit, SIGNAL(textChanged(const QString&)), this, SLOT(handleRoomChanged(const QString&)));
    this->mainLayout->addWidget(roomEdit);

    //= hostname
    this->mainLayout->addWidget(new QLabel(tr("Hostname")));
    hostnameEdit = new QModernLineEdit();
    this->mainLayout->addWidget(hostnameEdit);

    //= ip
    this->mainLayout->addWidget(new QLabel(tr("IP-Address")));
    ipAddressEdit = new QModernLineEdit();
    QString prefilledIp = "";

    QStringList subnetMask = backend->getConfig()->getSubnetMask().split(".");
    QStringList ipAddress = backend->getConfig()->getIpAddress().split(".");

    if(subnetMask.length() == 4 && ipAddress.length() == 4)
    for(int i = 0; i < 4; i++) {
        QString block = subnetMask[i];
        if(block == "255") {
            prefilledIp.append(ipAddress[i] + ".");
        }
    }
    ipAddressEdit->setText(prefilledIp);
    this->mainLayout->addWidget(ipAddressEdit);

    //= group
    this->mainLayout->addWidget(new QLabel(tr("Host group")));
    hostGroupEdit = new QModernLineEdit();
    this->mainLayout->addWidget(hostGroupEdit);

    //= dialog_register_clientRole
    this->mainLayout->addWidget(new QLabel(tr("Client role")));
    this->roleSelectBox = new QComboBox();

    this->roleSelectBox->setStyleSheet(
                "QComboBox {"
                    "border: 0 0 0 0;"
                    "border-bottom: 1px solid #ddd;"
                    "background-color: #f8f8f8;"
                "selection-color: #ffffff;"
                "selection-background-color: #394f5e;"
                "}"
                "QComboBox:focus {"
                    "border-bottom: 1px solid #f59c00;"
                "}"
                );


    //= dialog_register_clientRole_classroomStudent
    this->roleSelectBox->addItem(tr("Classroom student computer"), LinboBackend::ClassroomStudentComputerRole);

    //= dialog_register_clientRole_classroomTeacher
    this->roleSelectBox->addItem(tr("Classroom teacher computer"), LinboBackend::ClassroomTeacherComputerRole);

    //= dialog_register_clientRole_facultyTeacher
    this->roleSelectBox->addItem(tr("Faculty teacher computer"), LinboBackend::FacultyTeacherComputerRole);

    //= dialog_register_clientRole_staffComputer
    this->roleSelectBox->addItem(tr("Staff computer"), LinboBackend::StaffComputerRole);

    this->mainLayout->addWidget(this->roleSelectBox);

    this->mainLayout->addStretch();

    buttonLayout = new QHBoxLayout();

    //= dialog_register_button_resgister
    QModernPushButton* registerButton = new QModernPushButton("", tr("register"));
    registerButton->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(registerButton, SIGNAL(clicked()), this, SLOT(registerClient()));
    buttonLayout->addWidget(registerButton);

    //= cancel
    QModernPushButton* cancelButton = new QModernPushButton("", tr("cancel"));
    cancelButton->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(autoClose()));
    buttonLayout->addWidget(cancelButton);

    this->mainLayout->addLayout(buttonLayout);
}

void LinboRegisterDialog::registerClient() {
    this->backend->registerClient(this->roomEdit->text(), this->hostnameEdit->text(), this->ipAddressEdit->text(), this->hostGroupEdit->text(), LinboBackend::LinboDeviceRole(this->roleSelectBox->currentData().toInt()));
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
    QModernDialog::resizeEvent(event);

    int buttonHeight = this->parentWidget()->height() * 0.06;
    int margins = buttonHeight * 0.4;

    this->mainLayout->setContentsMargins(margins, margins, margins, margins);
    for(int i = 0; i < 10; i++) {
        QWidget* item = this->mainLayout->itemAt(i)->widget();

        // make lables smaller
        if(i % 2 == 0)
            item->setFixedSize(this->width() - margins * 2, buttonHeight * 0.6);
        else
            item->setFixedSize(this->width() - margins * 2, buttonHeight* 0.8);

        QFont font = item->font();
        font.setPixelSize(item->height() * 0.5);

        // make lables bold
        if(i % 2 == 0)
            font.setBold(true);

        item->setFont(font);
    }

    this->buttonLayout->setContentsMargins(0,0,0,0);
    for(int i = 0; i < 2; i++) {
        this->buttonLayout->itemAt(i)->widget()->setFixedSize(this->width() * 0.5 - margins * 1.5, buttonHeight);
    }
}
