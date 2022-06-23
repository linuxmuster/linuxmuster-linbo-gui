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
    this->_backend = backend;

    //% "Update cache"
    this->setTitle(qtTrId("dialog_updateCache_title"));

    this->_mainLayout = new QVBoxLayout(this);

    this->_updateTypeButtonGroup = new QButtonGroup(this);

    //% "Update using rsync"
    LinboRadioButton* rsyncButton = new LinboRadioButton(qtTrId("dialog_updateCache_updateType_rsync"));
    rsyncButton->setChecked(backend->config()->downloadMethod() == LinboConfig::Rsync);
    this->_mainLayout->addWidget(rsyncButton);
    this->_updateTypeButtonGroup->addButton(rsyncButton, int(LinboConfig::Rsync));

    //% "Update using multicast"
    LinboRadioButton* multicastButton = new LinboRadioButton(qtTrId("dialog_updateCache_updateType_multicast"));
    multicastButton->setChecked(backend->config()->downloadMethod() == LinboConfig::Multicast);
    this->_mainLayout->addWidget(multicastButton);
    this->_updateTypeButtonGroup->addButton(multicastButton, int(LinboConfig::Multicast));

    //% "Update using torrent"
    LinboRadioButton* torrentButton = new LinboRadioButton(qtTrId("dialog_updateCache_updateType_torrent"));
    torrentButton->setChecked(backend->config()->downloadMethod() == LinboConfig::Torrent);
    this->_mainLayout->addWidget(torrentButton);
    this->_updateTypeButtonGroup->addButton(torrentButton, int(LinboConfig::Torrent));

    QFrame* separatorLine = new QFrame();
    separatorLine->setStyleSheet(gTheme->insertValues("QFrame {color: %LineColor;}"));
    separatorLine->setFrameShape(QFrame::HLine);
    this->_mainLayout->addWidget(separatorLine);

    //% "Format cache partition"
    _formatCheckBox = new LinboCheckBox(qtTrId("dialog_updateCache_formatPartition"));
    this->_mainLayout->addWidget(_formatCheckBox);

    this->_mainLayout->addStretch();

    //% "update"
    LinboToolButton* toolButtonCache = new LinboToolButton(qtTrId("dialog_updateCache_button_update"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, &LinboToolButton::clicked, this, &LinboUpdateCacheDialog::_updateCache);

    //% "cancel"
    toolButtonCache = new LinboToolButton(qtTrId("cancel"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, &LinboToolButton::clicked, this, &LinboUpdateCacheDialog::autoClose);
}

void LinboUpdateCacheDialog::_updateCache() {
    this->_backend->updateCache(LinboConfig::DownloadMethod(this->_updateTypeButtonGroup->checkedId()), this->_formatCheckBox->isChecked());
    this->close();
}

void LinboUpdateCacheDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int margins = gTheme->size(LinboTheme::Margins);

    this->_mainLayout->setContentsMargins(margins, margins, margins, margins);
    for(int i = 0; i < 5; i++) {
        if(i == 3)
            // skip line
            continue;

        QAbstractButton* button = static_cast<QAbstractButton*>(this->_mainLayout->itemAt(i)->widget());
        button->setFixedSize(this->width() - margins * 2, gTheme->size(LinboTheme::RowHeight));
        QFont buttonFont = button->font();
        buttonFont.setPixelSize(gTheme->size(LinboTheme::RowFontSize));
        button->setFont(buttonFont);
    }
}
