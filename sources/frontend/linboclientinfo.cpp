#include "linboclientinfo.h"

LinboClientInfo::LinboClientInfo(LinboConfig* config, QWidget *parent) : QWidget(parent)
{

    this->mainLayout = new QVBoxLayout(this);
    this->mainLayout->setAlignment(Qt::AlignCenter);

    this->mainLayout->addStretch();

    this->networkInfoLayout = new QHBoxLayout();
    this->networkInfoLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    this->mainLayout->addLayout(this->networkInfoLayout);

    this->networkIconWidget = new QSvgWidget(":/svgIcons/network.svg");
    this->networkInfoLayout->addWidget(this->networkIconWidget);

    this->networkInfoLayout->addWidget(new QLabel("Hostname: " + config->getHostname()));
    this->networkInfoLayout->addWidget(new QLabel("Group:" + config->getHostGroup()));
    this->networkInfoLayout->addWidget(new QLabel("Client IP:" + config->getIpAddress()));
    this->networkInfoLayout->addWidget(new QLabel("Mac:" + config->getMacAddress()));

    this->mainLayout->addStretch();

    this->clientInfoLayout = new QHBoxLayout();
    this->clientInfoLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    this->mainLayout->addLayout(this->clientInfoLayout);

    this->desktopIconWidget = new QSvgWidget(":/svgIcons/desktop.svg");
    this->clientInfoLayout->addWidget(this->desktopIconWidget);

    this->clientInfoLayout->addWidget(new QLabel("HD: " + config->getHddSize()));
    this->clientInfoLayout->addWidget(new QLabel("Cache: " + config->getCacheSize()));
    this->clientInfoLayout->addWidget(new QLabel("CPU: " + config->getCpuModel()));
    this->clientInfoLayout->addWidget(new QLabel("RAM: " + config->getRamSize()));

    this->mainLayout->addStretch();
}


void LinboClientInfo::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    int iconHeight = this->height() * 0.3;

    this->networkIconWidget->setFixedSize(iconHeight, iconHeight);
    this->desktopIconWidget->setFixedSize(iconHeight, iconHeight);

    // set font size
    for(int i = 0; i < 8; i++) {
        QLayoutItem* labelItem;

        if(i < 4)
            labelItem = this->networkInfoLayout->itemAt(i + 1);
        else
            labelItem = this->clientInfoLayout->itemAt(i-4 + 1);

        QLabel* label = static_cast<QLabel*>(labelItem->widget());
        QFont labelFont = label->font();
        labelFont.setPixelSize(this->height() * 0.2);
        label->setFont(labelFont);
    }
}
