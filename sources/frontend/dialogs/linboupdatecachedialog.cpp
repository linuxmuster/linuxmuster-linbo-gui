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

    //% "Update cache"
    this->setTitle(qtTrId("dialog_updateCache_title"));

    this->mainLayout = new QVBoxLayout(this);

    this->updateTypeButtonGroup = new QButtonGroup(this);

    //% "Update using rsync"
    LinboRadioButton* rsyncButton = new LinboRadioButton(qtTrId("dialog_updateCache_updateType_rsync"));
    rsyncButton->setChecked(backend->getConfig()->downloadMethod() == LinboConfig::Rsync);
    this->mainLayout->addWidget(rsyncButton);
    this->updateTypeButtonGroup->addButton(rsyncButton, int(LinboConfig::Rsync));

    //% "Update using multicast"
    LinboRadioButton* multicastButton = new LinboRadioButton(qtTrId("dialog_updateCache_updateType_multicast"));
    multicastButton->setChecked(backend->getConfig()->downloadMethod() == LinboConfig::Multicast);
    this->mainLayout->addWidget(multicastButton);
    this->updateTypeButtonGroup->addButton(multicastButton, int(LinboConfig::Multicast));

    //% "Update using torrent"
    LinboRadioButton* torrentButton = new LinboRadioButton(qtTrId("dialog_updateCache_updateType_torrent"));
    torrentButton->setChecked(backend->getConfig()->downloadMethod() == LinboConfig::Torrent);
    this->mainLayout->addWidget(torrentButton);
    this->updateTypeButtonGroup->addButton(torrentButton, int(LinboConfig::Torrent));

    QFrame* separatorLine = new QFrame();
    separatorLine->setStyleSheet("QFrame {color: " + gTheme->getColor(LinboTheme::LineColor).name() + ";}");
    separatorLine->setFrameShape(QFrame::HLine);
    this->mainLayout->addWidget(separatorLine);

    //% "Format cache partition"
    formatCheckBox = new LinboCheckBox(qtTrId("dialog_updateCache_formatPartition"));
    this->mainLayout->addWidget(formatCheckBox);

    this->mainLayout->addStretch();

    //% "update"
    LinboToolButton* toolButtonCache = new LinboToolButton(qtTrId("dialog_updateCache_button_update"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, &LinboToolButton::clicked, this, &LinboUpdateCacheDialog::updateCache);

    //% "cancel"
    toolButtonCache = new LinboToolButton(qtTrId("cancel"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, &LinboToolButton::clicked, this, &LinboUpdateCacheDialog::autoClose);
}

void LinboUpdateCacheDialog::updateCache() {
    this->backend->updateCache(LinboConfig::DownloadMethod(this->updateTypeButtonGroup->checkedId()), this->formatCheckBox->isChecked());
    this->close();
}

void LinboUpdateCacheDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int margins = gTheme->getSize(LinboTheme::Margins);

    this->mainLayout->setContentsMargins(margins, margins, margins, margins);
    for(int i = 0; i < 5; i++) {
        if(i == 3)
            // skip line
            continue;

        QAbstractButton* button = static_cast<QAbstractButton*>(this->mainLayout->itemAt(i)->widget());
        button->setFixedSize(this->width() - margins * 2, gTheme->getSize(LinboTheme::RowHeight));
        QFont buttonFont = button->font();
        buttonFont.setPixelSize(gTheme->getSize(LinboTheme::RowFontSize));
        button->setFont(buttonFont);
    }
}
