#include "linbologindialog.h"

LinboLoginDialog::LinboLoginDialog(LinboBackend* backend, QWidget* parent) : LinboDialog(parent)
{
    this->backend = backend;

    //= dialog_login_title
    this->setTitle(tr("Please enter password:"));

    this->passwordInput = new LinboLineEdit();
    this->passwordInput->setEchoMode(QLineEdit::NoEcho);
    this->passwordInput->setAlignment(Qt::AlignCenter);
    connect(passwordInput,SIGNAL(returnPressed()),this,SLOT(inputFinished()));

    //= cancel
    LinboToolButton* toolButtonCache = new LinboToolButton(tr("cancel"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(close()));

    //= dialog_login_button_login
    toolButtonCache = new LinboToolButton(tr("login"));
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
        //= dialog_login_title_wrong
        this->setTitle(tr("Wrong password!"));
    }

    this->passwordInput->clear();
}

void LinboLoginDialog::setVisibleAnimated(bool visible) {
    if(!visible) {
        //= dialog_login_title
        this->setTitle(tr("Please enter password:"));
        this->passwordInput->clear();
    }
    else {
        this->passwordInput->setFocus();
    }
    LinboDialog::setVisibleAnimated(visible);
}
