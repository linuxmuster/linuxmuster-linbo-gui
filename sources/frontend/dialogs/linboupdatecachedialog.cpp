#include "linboupdatecachedialog.h"

LinboUpdateCacheDialog::LinboUpdateCacheDialog(LinboBackend* backend, QWidget* parent) : QModernDialog(parent)
{
    this->backend = backend;
    this->setTitle(tr("Update cache"));

    this->mainLayout = new QVBoxLayout(this);

    QRadioButton* rsyncButton = new QRadioButton(tr("Update using rsync"));
    rsyncButton->setChecked(backend->getConfig()->getDownloadMethod() == LinboConfig::Rsync);
    this->mainLayout->addWidget(rsyncButton);
    QRadioButton* multicastButton = new QRadioButton(tr("Update using multicast"));
    multicastButton->setChecked(backend->getConfig()->getDownloadMethod() == LinboConfig::Multicast);
    this->mainLayout->addWidget(multicastButton);
    QRadioButton* torrentButton = new QRadioButton(tr("Update using torrent"));
    torrentButton->setChecked(backend->getConfig()->getDownloadMethod() == LinboConfig::Torrent);
    this->mainLayout->addWidget(torrentButton);

    this->updateTypeButtonGroup = new QButtonGroup(this);
    this->updateTypeButtonGroup->addButton(rsyncButton, int(LinboConfig::Rsync));
    this->updateTypeButtonGroup->addButton(multicastButton, int(LinboConfig::Multicast));
    this->updateTypeButtonGroup->addButton(torrentButton, int(LinboConfig::Torrent));

    QFrame* separatorLine = new QFrame();
    separatorLine->setFrameShape(QFrame::HLine);
    this->mainLayout->addWidget(separatorLine);

    formatCheckBox = new QCheckBox(tr("Format cache partition"));
    this->mainLayout->addWidget(formatCheckBox);

    this->mainLayout->addStretch();

    buttonLayout = new QHBoxLayout();

    QModernPushButton* registerButton = new QModernPushButton("", tr("update"));
    connect(registerButton, SIGNAL(clicked()), this, SLOT(updateCache()));
    buttonLayout->addWidget(registerButton);

    QModernPushButton* cancelButton = new QModernPushButton("", tr("cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(autoClose()));
    buttonLayout->addWidget(cancelButton);

    this->mainLayout->addLayout(buttonLayout);
}

void LinboUpdateCacheDialog::updateCache() {
    this->backend->updateCache(LinboConfig::DownloadMethod(this->updateTypeButtonGroup->checkedId()), this->formatCheckBox->isChecked());
    this->close();
}

void LinboUpdateCacheDialog::resizeEvent(QResizeEvent *event) {
    QModernDialog::resizeEvent(event);

    int buttonHeight = this->parentWidget()->height() * 0.07;
    int margins = buttonHeight * 0.4;

    this->mainLayout->setContentsMargins(margins, margins, margins, margins);
    for(int i = 0; i < 5; i++) {
        if(i == 3)
            // skip line
            continue;

        QAbstractButton* button = static_cast<QAbstractButton*>(this->mainLayout->itemAt(i)->widget());
        button->setFixedSize(this->width() - margins * 2, buttonHeight);
        QFont buttonFont = button->font();
        buttonFont.setPixelSize(buttonHeight <= 0 ? 1:buttonHeight * 0.45);
        button->setFont(buttonFont);

    }

    this->buttonLayout->setContentsMargins(0,0,0,0);
    for(int i = 0; i < 2; i++) {
        this->buttonLayout->itemAt(i)->widget()->setFixedSize(this->width() * 0.5 - margins * 1.5, buttonHeight);
    }
}
