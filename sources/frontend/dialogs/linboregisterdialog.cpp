#include "linboregisterdialog.h"

LinboRegisterDialog::LinboRegisterDialog(LinboBackend* backend, QWidget* parent) : QModernDialog(parent)
{
    this->backend = backend;
    //: This is the title of the register dialog
    //= dialog_register_title
    this->setTitle(tr("Register client"));

    this->mainLayout = new QVBoxLayout(this);

    this->mainLayout->addWidget(new QLabel(tr("Room")));
    roomEdit = new QModernLineEdit();
    connect(roomEdit, SIGNAL(textChanged(const QString&)), this, SLOT(handleRoomChanged(const QString&)));
    this->mainLayout->addWidget(roomEdit);

    this->mainLayout->addWidget(new QLabel(tr("Hostname")));
    hostnameEdit = new QModernLineEdit();
    this->mainLayout->addWidget(hostnameEdit);

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

    this->mainLayout->addWidget(new QLabel(tr("Host group")));
    hostGroupEdit = new QModernLineEdit();
    this->mainLayout->addWidget(hostGroupEdit);

    this->mainLayout->addWidget(new QLabel(tr("Client role")));
    this->roleSelectBox = new QComboBox();

    this->roleSelectBox->addItem(tr("Classroom student computer"), LinboBackend::ClassroomStudentComputerRole);
    this->roleSelectBox->addItem(tr("Classroom teacher computer"), LinboBackend::ClassroomTeacherComputerRole);
    this->roleSelectBox->addItem(tr("Faculty teacher computer"), LinboBackend::FacultyTeacherComputerRole);
    this->roleSelectBox->addItem(tr("Staff computer"), LinboBackend::StaffComputerRole);

    this->mainLayout->addWidget(this->roleSelectBox);

    this->mainLayout->addStretch();

    buttonLayout = new QHBoxLayout();

    QModernPushButton* registerButton = new QModernPushButton("", tr("register"));
    connect(registerButton, SIGNAL(clicked()), this, SLOT(registerClient()));
    buttonLayout->addWidget(registerButton);

    QModernPushButton* cancelButton = new QModernPushButton("", tr("cancel"));
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

    int buttonHeight = this->parentWidget()->height() * 0.07;
    int margins = buttonHeight * 0.4;

    this->mainLayout->setContentsMargins(margins, margins, margins, margins);
    for(int i = 0; i < 10; i++) {
        this->mainLayout->itemAt(i)->widget()->setFixedSize(this->width() - margins * 2, buttonHeight* 0.8);
    }

    this->buttonLayout->setContentsMargins(0,0,0,0);
    for(int i = 0; i < 2; i++) {
        this->buttonLayout->itemAt(i)->widget()->setFixedSize(this->width() * 0.5 - margins * 1.5, buttonHeight);
    }
}
