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

#include "linboupdatecachedialog.h"

LinboUpdateCacheDialog::LinboUpdateCacheDialog(LinboBackend* backend, QWidget* parent) : LinboDialog(parent)
{
    this->backend = backend;

    //= dialog_updateCache_title
    this->setTitle(tr("Update cache"));

    this->mainLayout = new QVBoxLayout(this);

    this->updateTypeButtonGroup = new QButtonGroup(this);

    //= dialog_updateCache_updateType_rsync
    LinboRadioButton* rsyncButton = new LinboRadioButton(tr("Update using rsync"));
    rsyncButton->setChecked(backend->getConfig()->getDownloadMethod() == LinboConfig::Rsync);
    this->mainLayout->addWidget(rsyncButton);
    this->updateTypeButtonGroup->addButton(rsyncButton, int(LinboConfig::Rsync));

    //= dialog_updateCache_updateType_multicast
    LinboRadioButton* multicastButton = new LinboRadioButton(tr("Update using multicast"));
    multicastButton->setChecked(backend->getConfig()->getDownloadMethod() == LinboConfig::Multicast);
    this->mainLayout->addWidget(multicastButton);
    this->updateTypeButtonGroup->addButton(multicastButton, int(LinboConfig::Multicast));

    //= dialog_updateCache_updateType_torrent
    LinboRadioButton* torrentButton = new LinboRadioButton(tr("Update using torrent"));
    torrentButton->setChecked(backend->getConfig()->getDownloadMethod() == LinboConfig::Torrent);
    this->mainLayout->addWidget(torrentButton);
    this->updateTypeButtonGroup->addButton(torrentButton, int(LinboConfig::Torrent));

    QFrame* separatorLine = new QFrame();
    separatorLine->setStyleSheet("QFrame {color: " + gTheme->getColor(LinboGuiTheme::LineColor).name() + ";}");
    separatorLine->setFrameShape(QFrame::HLine);
    this->mainLayout->addWidget(separatorLine);

    //= dialog_updateCache_formatPartition
    formatCheckBox = new LinboCheckBox(tr("Format cache partition"));
    this->mainLayout->addWidget(formatCheckBox);

    this->mainLayout->addStretch();

    //= dialog_updateCache_button_update
    LinboToolButton* toolButtonCache = new LinboToolButton(tr("update"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(updateCache()));

    //= cancel
    toolButtonCache = new LinboToolButton(tr("cancel"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(autoClose()));
}

void LinboUpdateCacheDialog::updateCache() {
    this->backend->updateCache(LinboConfig::DownloadMethod(this->updateTypeButtonGroup->checkedId()), this->formatCheckBox->isChecked());
    this->close();
}

void LinboUpdateCacheDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int margins = gTheme->getSize(LinboGuiTheme::Margins);

    this->mainLayout->setContentsMargins(margins, margins, margins, margins);
    for(int i = 0; i < 5; i++) {
        if(i == 3)
            // skip line
            continue;

        QAbstractButton* button = static_cast<QAbstractButton*>(this->mainLayout->itemAt(i)->widget());
        button->setFixedSize(this->width() - margins * 2, gTheme->getSize(LinboGuiTheme::RowHeight));
        QFont buttonFont = button->font();
        buttonFont.setPixelSize(gTheme->getSize(LinboGuiTheme::RowFontSize));
        button->setFont(buttonFont);
    }
}
