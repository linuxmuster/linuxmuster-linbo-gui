#include "linboclientinfo.h"

LinboClientInfo::LinboClientInfo(LinboConfig* config, QWidget *parent) : QWidget(parent)
{

    this->mainWidget = new QWidget(this);

    this->mainLayout = new QGridLayout(this->mainWidget);
    this->mainLayout->setAlignment(Qt::AlignCenter);

    this->networkIconWidget = new QSvgWidget(":/svgIcons/network.svg");
    this->mainLayout->addWidget(this->networkIconWidget, 0, 0, 1, 1);

    //= client_info_hostname
    this->mainLayout->addWidget(new QLabel("<b>" + tr("Hostname:") + "</b>  " + config->getHostname()), 0, 1, 1, 1);

    //= client_info_group
    this->mainLayout->addWidget(new QLabel("<b>" + tr("Group:") + "</b>  " + config->getHostGroup()), 0, 2, 1, 1);

    //= client_info_ip
    this->mainLayout->addWidget(new QLabel("<b>" + tr("Client IP:") + "</b>  " + config->getIpAddress()), 0, 3, 1, 1);

    //= client_info_mac
    this->mainLayout->addWidget(new QLabel("<b>" + tr("Mac:") + "</b>  " + config->getMacAddress()), 0, 4, 1, 1);

    this->desktopIconWidget = new QSvgWidget(":/svgIcons/desktop.svg");
    this->mainLayout->addWidget(this->desktopIconWidget, 1, 0, 1, 1);

    //= client_info_hdd
    this->mainLayout->addWidget(new QLabel("<b>" + tr("HDD:") + "</b>  " + config->getHddSize()), 1, 1, 1, 1);

    //= client_info_cache
    this->mainLayout->addWidget(new QLabel("<b>" + tr("Cache:") + "</b>  " + config->getCacheSize()), 1, 2, 1, 1);

    //= client_info_cpu
    this->mainLayout->addWidget(new QLabel("<b>" + tr("CPU:") + "</b>  " + config->getCpuModel()), 1, 3, 1, 1);

    //= client_info_ram
    this->mainLayout->addWidget(new QLabel("<b>" + tr("RAM:") + "</b>  " + config->getRamSize()), 1, 4, 1, 1);


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
        labelFont.setPixelSize(int(this->height() * 0.2) <= 0 ? 1:this->height() * 0.2);
        label->setFont(labelFont);
    }
}
