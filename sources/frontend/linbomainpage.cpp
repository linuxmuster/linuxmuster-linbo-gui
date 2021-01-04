/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020  Dorian Zedler <dorian@itsblue.de>
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

#include "linbomainpage.h"

LinboMainPage::LinboMainPage(LinboBackend* backend, QWidget *parent) : QWidget(parent)
{
    this->inited = false;

    this->backend = backend;

#ifdef TEST_ENV
    this->backend->login("Muster!");
#endif

    connect(this->backend, SIGNAL(stateChanged(LinboBackend::LinboState)), this, SLOT(handleLinboStateChanged(LinboBackend::LinboState)));

    this->setGeometry(QRect(0,0,parent->width(), parent->height()));

    // create the main layout

    // main layout
    QWidget* mainLayoutWidget = new QWidget(this);
    mainLayoutWidget->setGeometry(this->geometry());
    QVBoxLayout* mainLayout = new QVBoxLayout(mainLayoutWidget);
    mainLayout->setSpacing(this->height()*0.025);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addStretch();

    // OS Buttons
    osSelectionRow = new LinboOsSelectionRow(this->backend);
    mainLayout->addWidget(osSelectionRow);

    // action buttons
    this->mainActions = new LinboMainActions(this->backend, this);
    this->mainActions->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mainLayout->addWidget(this->mainActions);

    this->startActionWidgetAnimation = new QPropertyAnimation(this->mainActions, "minimumSize");
    this->startActionWidgetAnimation->setDuration(400);
    this->startActionWidgetAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    mainLayout->addStretch();

    QLabel* versionAndNetworkLabel = new QLabel(backend->getConfig()->getLinboVersion() + " - GUI " + GUI_VERSION + " - " + this->backend->getConfig()->getIpAddress());
    mainLayout->addWidget(versionAndNetworkLabel);
    mainLayout->setAlignment(versionAndNetworkLabel, Qt::AlignCenter);

    // client info
    clientInfo = new LinboClientInfo(this->backend->getConfig(), this);
    clientInfo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    clientInfo->setFixedWidth(this->width() * 0.9);
    mainLayout->addWidget(clientInfo);
    mainLayout->setAlignment(this->clientInfo, Qt::AlignCenter);

    this->clientInfoAnimation = new QPropertyAnimation(clientInfo, "minimumSize");
    this->clientInfoAnimation->setDuration(400);
    this->clientInfoAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    this->showClientInfo = false;

    // power and settings Buttons
    QWidget* powerActionsLayoutWidget = new QWidget(this);
    int height = this->height() * 0.3;
    int width = height / 3;
    int margins = width * 0.1;
    int buttonWidth = width * 0.6;
    powerActionsLayoutWidget->setGeometry(QRect(this->width() - (width + margins), this->height() - (height + margins), width * 1.1, height));

    rootActionButton = new QModernPushButton(":/svgIcons/settingsAction.svg");
    this->powerActionButtons.append(rootActionButton);
    rootActionButton->setFixedHeight(buttonWidth);
    rootActionButton->setFixedWidth(buttonWidth);

    logoutActionButton = new QModernPushButton(":/svgIcons/logout.svg");
    connect(logoutActionButton, SIGNAL(clicked()), this->backend, SLOT(logout()));
    this->powerActionButtons.append(logoutActionButton);
    logoutActionButton->setFixedHeight(buttonWidth);
    logoutActionButton->setFixedWidth(buttonWidth);
    logoutActionButton->setVisible(false);

    QModernPushButton* rebootActionButton = new QModernPushButton(":/svgIcons/rebootAction.svg");
    connect(rebootActionButton, SIGNAL(clicked()), this->backend, SLOT(reboot()));
    this->powerActionButtons.append(rebootActionButton);
    rebootActionButton->setFixedHeight(buttonWidth);
    rebootActionButton->setFixedWidth(buttonWidth);

    QModernPushButton* shutdownActionButton = new QModernPushButton(":/svgIcons/shutdownAction.svg");
    connect(shutdownActionButton, SIGNAL(clicked()), this->backend, SLOT(shutdown()));
    this->powerActionButtons.append(shutdownActionButton);
    shutdownActionButton->setFixedHeight(buttonWidth);
    shutdownActionButton->setFixedWidth(buttonWidth);

    QVBoxLayout* powerActionsLayout = new QVBoxLayout(powerActionsLayoutWidget);
    powerActionsLayout->setSpacing(0);
    powerActionsLayout->addWidget(rootActionButton);
    powerActionsLayout->addWidget(logoutActionButton);
    powerActionsLayout->addWidget(rebootActionButton);
    powerActionsLayout->addWidget(shutdownActionButton);

    // Dialogs (for imaging stuff)
    this->loginDialog = new LinboLoginDialog(this->backend, this);
    int dialogHeight = this->height() * 0.3;
    int dialogWidth = this->height() * 0.5;
    this->loginDialog->setGeometry( (this->width() - dialogWidth) / 2, (this->height() - dialogHeight) / 2,dialogWidth, dialogHeight);
    connect(this->powerActionButtons[0], SIGNAL(clicked()), this->loginDialog, SLOT(open()));

    dialogHeight = this->parentWidget()->height() * 0.8;
    dialogWidth = dialogHeight;

    this->imageCreationDialog = new LinboImageCreationDialog(backend, parent);
    this->imageCreationDialog->setGeometry(0, 0, dialogWidth, dialogHeight);
    this->imageCreationDialog->centerInParent();
    connect(this->osSelectionRow, &LinboOsSelectionRow::imageCreationRequested,
            this->imageCreationDialog, &LinboImageCreationDialog::open);
    connect(this->mainActions, &LinboMainActions::imageCreationRequested,
            this->imageCreationDialog, &LinboImageCreationDialog::open);

    this->imageUploadDialog = new LinboImageUploadDialog(backend, parent);
    this->imageUploadDialog->setGeometry(0, 0, dialogWidth, dialogHeight * 0.4);
    this->imageUploadDialog->centerInParent();
    connect(this->osSelectionRow, &LinboOsSelectionRow::imageUploadRequested,
            this->imageUploadDialog, &LinboImageUploadDialog::open);
    connect(this->mainActions, &LinboMainActions::imageUploadRequested,
            this->imageUploadDialog, &LinboImageUploadDialog::open);

    this->terminalDialog = new LinboTerminalDialog(parent);
    this->terminalDialog->setGeometry(0, 0, dialogWidth, dialogHeight);
    this->terminalDialog->centerInParent();
    connect(this->mainActions, &LinboMainActions::terminalRequested,
            this->terminalDialog, &LinboImageCreationDialog::open);

    this->confirmationDialog = new LinboConfirmationDialog(
                //= dialog_partition_title
                tr("Partition drive"),
                //= dialog_partition_question
                tr("Are you sure? This will delete all data on your drive!"),
                parent);
    this->confirmationDialog->setGeometry(0, 0, dialogWidth, dialogHeight * 0.3);
    this->confirmationDialog->centerInParent();
    connect(this->confirmationDialog, SIGNAL(accepted()), this->backend, SLOT(partitionDrive()));
    connect(this->mainActions, &LinboMainActions::drivePartitioningRequested,
            this->confirmationDialog, &LinboImageCreationDialog::open);

    this->registerDialog = new LinboRegisterDialog(backend, parent);
    this->registerDialog->setGeometry(0, 0, dialogWidth, dialogHeight * 0.8);
    this->registerDialog->centerInParent();
    connect(this->mainActions, &LinboMainActions::registrationRequested,
            this->registerDialog, &LinboImageCreationDialog::open);

    this->updateCacheDialog = new LinboUpdateCacheDialog(backend, parent);
    this->updateCacheDialog->setGeometry(0, 0, dialogWidth * 0.6, dialogHeight * 0.5);
    this->updateCacheDialog->centerInParent();
    connect(this->mainActions, &LinboMainActions::cacheUpdateRequested,
            this->updateCacheDialog, &LinboImageCreationDialog::open);

    // Linbo logo
    QSvgRenderer* renderer = new QSvgRenderer(QString(":/images/linbo_logo_small.svg"));
    renderer->setAspectRatioMode(Qt::KeepAspectRatio);

    QImage* image = new QImage(500, 100, QImage::Format_ARGB32);
    image->fill("#ffffff");

    QPainter painter(image);
    renderer->render(&painter);

    QLabel* linboLogo = new QLabel(this);
    linboLogo->setPixmap(QPixmap::fromImage(*image));

    int linboLogoHeight = this->height() * 0.15;
    linboLogo->move((this->width() - linboLogoHeight * 3) / 2, linboLogoHeight * 0.1);
    linboLogo->setFixedHeight(linboLogoHeight);
    linboLogo->setFixedWidth(linboLogoHeight * 3);
    linboLogo->hide();

    this->handleLinboStateChanged(this->backend->getState());
}

void LinboMainPage::handleLinboStateChanged(LinboBackend::LinboState newState) {
    bool powerActionButtonsVisible = false;
    int startActionsWidgetHeight;
    int osSelectionRowHeight;
    int clientInfoHeight = 0;
    bool useMinimalLayout = this->backend->getConfig()->getUseMinimalLayout();

    switch (newState) {
    case LinboBackend::StartActionError:
    case LinboBackend::RootActionError:
        osSelectionRowHeight = this->height() * 0.3;
        startActionsWidgetHeight = this->height() * 0.5;
        break;

    case LinboBackend::Idle:
        if(useMinimalLayout){
            osSelectionRowHeight = this->height() * 0.3;
            startActionsWidgetHeight = this->height() * 0.2;
        }
        else {
            osSelectionRowHeight = this->height() * 0.4;
            startActionsWidgetHeight = this->height() * 0;
        }

        if(this->showClientInfo)
            clientInfoHeight = this->height() * 0.1;

        powerActionButtonsVisible = true;
        break;

    case LinboBackend::Root:
    case LinboBackend::Registering:
    case LinboBackend::RootActionSuccess:
        if(useMinimalLayout){
            osSelectionRowHeight = this->height() * 0.2;
            startActionsWidgetHeight = this->height() * 0.45;
        }
        else {
            osSelectionRowHeight = this->height() * 0.4;
            startActionsWidgetHeight = this->height() * 0.3;
        }

        clientInfoHeight = this->height() * 0.1;

        powerActionButtonsVisible = true;
        break;

    case LinboBackend::Partitioning:
    case LinboBackend::UpdatingCache:
        osSelectionRowHeight = this->height() * 0;
        startActionsWidgetHeight = this->height() * 0.2;
        break;

    default:
        osSelectionRowHeight = this->height() * 0.3;
        startActionsWidgetHeight = this->height() * 0.2;
        break;
    }

    for(QModernPushButton* powerActionButton : this->powerActionButtons)
        if(powerActionButton == logoutActionButton && newState < LinboBackend::Root)
            powerActionButton->setVisible(false);
        else if(powerActionButton == logoutActionButton && newState >= LinboBackend::Root)
            powerActionButton->setVisible(powerActionButtonsVisible);
        else if(powerActionButton == rootActionButton && newState >= LinboBackend::Root)
            powerActionButton->setVisible(false);
        else
            powerActionButton->setVisible(powerActionButtonsVisible);

    if(this->inited) {
        this->startActionWidgetAnimation->setStartValue(QSize(this->width(), this->mainActions->height()));
        this->startActionWidgetAnimation->setEndValue(QSize(this->width(), startActionsWidgetHeight));
        this->startActionWidgetAnimation->start();

        this->osSelectionRow->setMinimumSizeAnimated(QSize(this->width(), osSelectionRowHeight));

        this->clientInfoAnimation->setStartValue(QSize(this->width() * 0.9, this->clientInfo->height()));
        this->clientInfoAnimation->setEndValue(QSize(this->width() * 0.9, clientInfoHeight));
        this->clientInfoAnimation->start();
    }
    else {
        this->mainActions->setMinimumSize(this->width(), startActionsWidgetHeight);
        this->osSelectionRow->setMinimumSize(this->width(), osSelectionRowHeight);
        this->clientInfo->setMinimumSize(this->width() * 0.9, clientInfoHeight);
    }

    this->inited = true;
}

void LinboMainPage::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_F1 && this->backend->getState() == LinboBackend::Idle) {
        this->showClientInfo = !this->showClientInfo;

        if(this->showClientInfo) {
            this->clientInfoAnimation->setStartValue(QSize(this->width() * 0.9, this->clientInfo->height()));
            this->clientInfoAnimation->setEndValue(QSize(this->width() * 0.9, this->height() * 0.1));
            this->clientInfoAnimation->start();
        }
        else {
            this->clientInfoAnimation->setStartValue(QSize(this->width() * 0.9, this->clientInfo->height()));
            this->clientInfoAnimation->setEndValue(QSize(this->width() * 0.9, 0));
            this->clientInfoAnimation->start();
        }
    }
}
