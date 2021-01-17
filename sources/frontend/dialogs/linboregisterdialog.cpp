#include "linboregisterdialog.h"

LinboRegisterDialog::LinboRegisterDialog(LinboBackend* backend, QWidget* parent) : LinboDialog(parent)
{
    this->backend = backend;

    //= dialog_register_title
    this->setTitle(tr("Register client"));

    this->mainLayout = new QVBoxLayout(this);

    //= room
    this->mainLayout->addWidget(new QLabel(tr("Room")));
    roomEdit = new LinboLineEdit();
    connect(roomEdit, SIGNAL(textChanged(const QString&)), this, SLOT(handleRoomChanged(const QString&)));
    this->mainLayout->addWidget(roomEdit);

    //= hostname
    this->mainLayout->addWidget(new QLabel(tr("Hostname")));
    hostnameEdit = new LinboLineEdit();
    this->mainLayout->addWidget(hostnameEdit);

    //= ip
    this->mainLayout->addWidget(new QLabel(tr("IP-Address")));
    ipAddressEdit = new LinboLineEdit();
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
    hostGroupEdit = new LinboLineEdit();
    this->mainLayout->addWidget(hostGroupEdit);

    //= dialog_register_clientRole
    this->mainLayout->addWidget(new QLabel(tr("Client role")));
    this->roleSelectBox = new LinboComboBox();

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

    //= dialog_register_button_resgister
    LinboToolButton* toolButtonCache = new LinboToolButton(tr("register"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(registerClient()));

    //= cancel
    toolButtonCache = new LinboToolButton(tr("cancel"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(autoClose()));
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
