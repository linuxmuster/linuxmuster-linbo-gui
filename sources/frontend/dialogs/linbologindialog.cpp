#include "linbologindialog.h"

LinboLoginDialog::LinboLoginDialog(LinboBackend* backend, QWidget* parent) : QModernDialog(parent)
{
    this->backend = backend;

    this->setWindowFlags(Qt::FramelessWindowHint);

    //= dialog_login_title
    this->headerLabel = new QLabel(tr("Please enter password:"));
    this->headerLabel->setAlignment(Qt::AlignCenter);

    this->passwordInput = new QModernLineEdit();
    this->passwordInput->setEchoMode(QLineEdit::NoEcho);
    this->passwordInput->setAlignment(Qt::AlignCenter);
    connect(passwordInput,SIGNAL(returnPressed()),this,SLOT(inputFinished()));

    //= cancel
    cancelButton = new QModernPushButton("", tr("cancel"));
    cancelButton->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    //= dialog_login_button_login
    loginButton = new QModernPushButton("", "login");
    loginButton->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(loginButton, SIGNAL(clicked()), this, SLOT(inputFinished()));

    this->buttonLayout = new QHBoxLayout();
    this->buttonLayout->setContentsMargins(0,0,0,0);
    this->buttonLayout->addWidget(cancelButton);
    this->buttonLayout->addWidget(loginButton);
    this->buttonLayout->setAlignment(Qt::AlignCenter);

    this->mainLayout = new QVBoxLayout(this);
    this->mainLayout->setContentsMargins(0,0,0,0);
    this->mainLayout->addStretch();
    this->mainLayout->addWidget(this->headerLabel);
    this->mainLayout->addStretch();
    this->mainLayout->addWidget(this->passwordInput);
    this->mainLayout->setAlignment(this->passwordInput, Qt::AlignCenter);
    this->mainLayout->addStretch();
    this->mainLayout->addLayout(this->buttonLayout);
    this->mainLayout->addStretch();
    this->mainLayout->setAlignment(Qt::AlignCenter);
}

void LinboLoginDialog::resizeEvent(QResizeEvent *event) {
    QModernDialog::resizeEvent(event);

    int buttonHeight = this->parentWidget()->height() * 0.06;

    QFont font = this->headerLabel->font();
    this->headerLabel->setFixedWidth(this->width());
    this->headerLabel->setFixedHeight(this->height() * 0.2);
    font.setPixelSize(this->height() <= 0 ? 1:double(this->height()) * 0.1);
    this->headerLabel->setFont(font);

    this->passwordInput->setFixedHeight(this->height() * 0.2);
    this->passwordInput->setFixedWidth(this->width() * 0.85);

    this->cancelButton->setFixedSize(this->width() * 0.4, buttonHeight);
    this->loginButton->setFixedSize(this->width() * 0.4, buttonHeight);

    this->buttonLayout->setSpacing(this->width() * 0.05);
    this->mainLayout->setSpacing(this->height() * 0.05);
}


void LinboLoginDialog::inputFinished() {
    if(this->backend->login(this->passwordInput->text())) {
        this->close();
    }
    else {
        this->headerLabel->setStyleSheet("QLabel { color : red; }");
        //= dialog_login_title_wrong
        this->headerLabel->setText(tr("Wrong password!"));
    }

    this->passwordInput->clear();
}

void LinboLoginDialog::setVisibleAnimated(bool visible) {
    if(!visible) {
        this->headerLabel->setStyleSheet("QLabel { color : black; }");
        //= dialog_login_title
        this->headerLabel->setText(tr("Please enter password:"));
        this->passwordInput->clear();
    }
    else {
        this->passwordInput->setFocus();
    }
    QModernDialog::setVisibleAnimated(visible);
}
