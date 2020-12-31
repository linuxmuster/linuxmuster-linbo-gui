#include "linboupdatecachedialog.h"

LinboUpdateCacheDialog::LinboUpdateCacheDialog(LinboBackend* backend, QWidget* parent) : QModernDialog(parent)
{
    this->backend = backend;

    //= dialog_updateCache_title
    this->setTitle(tr("Update cache"));

    this->mainLayout = new QVBoxLayout(this);

    this->updateTypeButtonGroup = new QButtonGroup(this);

    //= dialog_updateCache_updateType_rsync
    QModernRadioButton* rsyncButton = new QModernRadioButton(tr("Update using rsync"));
    rsyncButton->setChecked(backend->getConfig()->getDownloadMethod() == LinboConfig::Rsync);
    this->mainLayout->addWidget(rsyncButton);
    this->updateTypeButtonGroup->addButton(rsyncButton, int(LinboConfig::Rsync));

    //= dialog_updateCache_updateType_multicast
    QModernRadioButton* multicastButton = new QModernRadioButton(tr("Update using multicast"));
    multicastButton->setChecked(backend->getConfig()->getDownloadMethod() == LinboConfig::Multicast);
    this->mainLayout->addWidget(multicastButton);
    this->updateTypeButtonGroup->addButton(multicastButton, int(LinboConfig::Multicast));

    //= dialog_updateCache_updateType_torrent
    QModernRadioButton* torrentButton = new QModernRadioButton(tr("Update using torrent"));
    torrentButton->setChecked(backend->getConfig()->getDownloadMethod() == LinboConfig::Torrent);
    this->mainLayout->addWidget(torrentButton);
    this->updateTypeButtonGroup->addButton(torrentButton, int(LinboConfig::Torrent));

    QFrame* separatorLine = new QFrame();
    separatorLine->setStyleSheet("QFrame {color: #cccccc;}");
    separatorLine->setFrameShape(QFrame::HLine);
    this->mainLayout->addWidget(separatorLine);

    //= dialog_updateCache_formatPartition
    formatCheckBox = new QModernCheckBox(tr("Format cache partition"));
    this->mainLayout->addWidget(formatCheckBox);

    this->mainLayout->addStretch();

    buttonLayout = new QHBoxLayout();

    //= dialog_updateCache_button_update
    QModernPushButton* updateButton = new QModernPushButton("", tr("update"));
    updateButton->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    connect(updateButton, SIGNAL(clicked()), this, SLOT(updateCache()));
    buttonLayout->addWidget(updateButton);

    //= cancel
    QModernPushButton* cancelButton = new QModernPushButton("", tr("cancel"));
    cancelButton->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
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

    int buttonHeight = this->parentWidget()->height() * 0.06;
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
