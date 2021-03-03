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

#include "linboclientinfo.h"

LinboClientInfo::LinboClientInfo(LinboConfig* config, QWidget *parent) : QWidget(parent)
{

    this->mainWidget = new QWidget(this);
    this->mainWidget->setStyleSheet( "QLabel { color: " + QString(config->isBackgroundColorDark() ? "white":"black") + "; }");

    this->mainLayout = new QGridLayout(this->mainWidget);
    this->mainLayout->setAlignment(Qt::AlignCenter);

    this->networkIconWidget = new QSvgWidget(gTheme->getIconPath(LinboGuiTheme::NetworkIcon));
    this->mainLayout->addWidget(this->networkIconWidget, 0, 0, 1, 1);

    //% "Hostname"
    this->mainLayout->addWidget(new QLabel("<b>" + qtTrId("hostname") + ":</b>  " + config->getHostname()), 0, 1, 1, 1);

    //% "Host group"
    this->mainLayout->addWidget(new QLabel("<b>" + qtTrId("group") + ":</b>  " + config->getHostGroup()), 0, 2, 1, 1);

    //% "IP-Address"
    this->mainLayout->addWidget(new QLabel("<b>" + qtTrId("ip") + ":</b>  " + config->getIpAddress()), 0, 3, 1, 1);

    //% "Mac"
    this->mainLayout->addWidget(new QLabel("<b>" + qtTrId("client_info_mac") + ":</b>  " + config->getMacAddress()), 0, 4, 1, 1);

    this->desktopIconWidget = new QSvgWidget(gTheme->getIconPath(LinboGuiTheme::DesktopIcon));
    this->mainLayout->addWidget(this->desktopIconWidget, 1, 0, 1, 1);

    //% "HDD"
    this->mainLayout->addWidget(new QLabel("<b>" + qtTrId("client_info_hdd") + ":</b>  " + config->getHddSize()), 1, 1, 1, 1);

    //% "Cache"
    this->mainLayout->addWidget(new QLabel("<b>" + qtTrId("client_info_cache") + ":</b>  " + config->getCacheSize()), 1, 2, 1, 1);

    //% "CPU"
    this->mainLayout->addWidget(new QLabel("<b>" + qtTrId("client_info_cpu") + ":</b>  " + config->getCpuModel()), 1, 3, 1, 1);

    //% "RAM"
    this->mainLayout->addWidget(new QLabel("<b>" + qtTrId("client_info_ram") + ":</b>  " + config->getRamSize()), 1, 4, 1, 1);


    this->setStyleSheet("QGridLayout {background: red;}");
}


void LinboClientInfo::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    int iconHeight = this->height() * 0.3;

    this->mainWidget->setGeometry(0,0,this->width(), this->height());
    this->mainLayout->setSpacing(this->height() * 0.2);

    this->networkIconWidget->setFixedSize(iconHeight, iconHeight);
    this->desktopIconWidget->setFixedSize(iconHeight, iconHeight);

    // set font size
    for(int i = 0; i < 10; i++) {
        // skip svg icons
        if(i == 0 || i == 5)
            continue;

        QLayoutItem* labelItem = this->mainLayout->itemAt(i);

        QLabel* label = static_cast<QLabel*>(labelItem->widget());
        label->setTextFormat(Qt::RichText);

        label->setMaximumWidth(this->width() * 0.2);

        QFont labelFont = label->font();
        labelFont.setPixelSize(gTheme->toFontSize(this->height() * 0.2));
        label->setFont(labelFont);
    }
}
