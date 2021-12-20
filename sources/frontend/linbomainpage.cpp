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

#include "linbomainpage.h"

LinboMainPage::LinboMainPage(LinboBackend* backend, QWidget *parent) : QWidget(parent)
{
    this->inited = false;
    this->showClientInfo = backend->config()->clientDetailsVisibleByDefault();
    this->f1Pressed = false;

    this->backend = backend;

#ifdef TEST_ENV
    //this->backend->login("Muster!");
#endif

    connect(this->backend, &LinboBackend::stateChanged, this, &LinboMainPage::handleLinboStateChanged);

    this->setGeometry(QRect(0,0,parent->width(), parent->height()));

    // create the main layout
    // main layout
    QWidget* mainLayoutWidget = new QWidget(this);
    mainLayoutWidget->setGeometry(this->geometry());
    QVBoxLayout* mainLayout = new QVBoxLayout(mainLayoutWidget);
    mainLayout->setSpacing(this->height()*0.03);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addSpacerItem(new QSpacerItem(this->width(), mainLayout->spacing()));

    // Linbo logo
    int linboLogoHeight = gTheme->getSize(LinboTheme::TopLogoHeight);

    LinboSvgWidget* linboLogo = new LinboSvgWidget(linboLogoHeight, gTheme->getIconPath(LinboTheme::BrandTopIcon));
    mainLayout->addWidget(linboLogo);
    mainLayout->setAlignment(linboLogo, Qt::AlignCenter);

    mainLayout->addStretch();

    // OS Buttons
    osSelectionRow = new LinboOsSelectionRow(this->backend);
    mainLayout->addWidget(osSelectionRow);

    mainLayout->addStretch();

    // action buttons
    this->mainActions = new LinboMainActions(this->backend, this);
    this->mainActions->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mainLayout->addWidget(this->mainActions);

    this->startActionWidgetAnimation = new QPropertyAnimation(this->mainActions, "minimumSize");
    this->startActionWidgetAnimation->setDuration(400);
    this->startActionWidgetAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    mainLayout->addStretch();

    // version / network label
    QLabel* versionAndNetworkLabel = new QLabel(backend->config()->linboVersion() + "- GUI " + GUI_VERSION + " - " + this->backend->config()->ipAddress() + " - F1");
    QFont versionAndNetworkLabelFont;
    versionAndNetworkLabelFont.setPixelSize(gTheme->getSize(LinboTheme::RowFontSize));
    versionAndNetworkLabel->setFont(versionAndNetworkLabelFont);
    versionAndNetworkLabel->setStyleSheet( "QLabel { color: " + gTheme->getColor(LinboTheme::TextColor).name() + "; }");
    mainLayout->addWidget(versionAndNetworkLabel);
    mainLayout->setAlignment(versionAndNetworkLabel, Qt::AlignCenter);


    // Linuxmuster logo
    double linuxmusterLogoHeight = gTheme->getSize(LinboTheme::BottomLogoHeight);

    LinboSvgWidget* linuxmusterLogo = new LinboSvgWidget(linuxmusterLogoHeight, gTheme->getIconPath(LinboTheme::BrandBottomIcon));
    mainLayout->addWidget(linuxmusterLogo);
    mainLayout->setAlignment(linuxmusterLogo, Qt::AlignCenter);

    // client info
    clientInfo = new LinboClientInfo(this->backend->config(), this);
    clientInfo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    clientInfo->setFixedWidth(this->width() * 0.9);
    mainLayout->addWidget(clientInfo);
    mainLayout->setAlignment(this->clientInfo, Qt::AlignCenter);

    this->clientInfoAnimation = new QPropertyAnimation(clientInfo, "minimumSize");
    this->clientInfoAnimation->setDuration(400);
    this->clientInfoAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // power and settings Buttons

    QWidget* powerActionsLayoutWidget = new QWidget(this);
    int height = this->height() * 0.3;
    int width = height / 3;
    int margins = width * 0.1;
    int buttonWidth = width * 0.6;
    int x = this->width() - (width + margins);
    int y = this->height() - (height + margins);
    powerActionsLayoutWidget->setGeometry(QRect(x, y, width, height));

    rootActionButton = new LinboToolButton(LinboTheme::SettingsIcon);
    this->powerActionButtons.append(rootActionButton);
    rootActionButton->setFixedHeight(buttonWidth);
    rootActionButton->setFixedWidth(buttonWidth);
    //% "Settings"
    rootActionButton->setToolTip(qtTrId("settings"));

    logoutActionButton = new LinboToolButton(LinboTheme::LogoutIcon);
    connect(logoutActionButton, &LinboToolButton::clicked, this->backend, &LinboBackend::logout);
    this->powerActionButtons.append(logoutActionButton);
    logoutActionButton->setFixedHeight(buttonWidth);
    logoutActionButton->setFixedWidth(buttonWidth);
    logoutActionButton->setVisible(false);
    //% "Log out"
    logoutActionButton->setToolTip(qtTrId("logout"));

    LinboPushButton* rebootActionButton = new LinboToolButton(LinboTheme::RebootIcon);
    connect(rebootActionButton, &LinboToolButton::clicked, this->backend, &LinboBackend::reboot);
    this->powerActionButtons.append(rebootActionButton);
    rebootActionButton->setFixedHeight(buttonWidth);
    rebootActionButton->setFixedWidth(buttonWidth);
    //% "Reboot"
    rebootActionButton->setToolTip(qtTrId("reboot"));

    LinboPushButton* shutdownActionButton = new LinboToolButton(LinboTheme::ShutdownIcon);
    connect(shutdownActionButton, &LinboToolButton::clicked, this->backend, &LinboBackend::shutdown);
    this->powerActionButtons.append(shutdownActionButton);
    shutdownActionButton->setFixedHeight(buttonWidth);
    shutdownActionButton->setFixedWidth(buttonWidth);
    //% "Shutdown"
    shutdownActionButton->setToolTip(qtTrId("shutdown"));

    QVBoxLayout* powerActionsLayout = new QVBoxLayout(powerActionsLayoutWidget);
    powerActionsLayout->setSpacing(0);
    powerActionsLayout->setContentsMargins(0,0,0,0);
    powerActionsLayout->addWidget(rootActionButton);
    powerActionsLayout->addWidget(logoutActionButton);
    powerActionsLayout->addWidget(rebootActionButton);
    powerActionsLayout->addWidget(shutdownActionButton);

    // Dialogs (for imaging stuff)
    this->loginDialog = new LinboLoginDialog(this->backend, this);
    int dialogHeight = gTheme->getSize(LinboTheme::DialogHeight);
    int dialogWidth = gTheme->getSize(LinboTheme::DialogWidth);
    this->loginDialog->setGeometry( 0, 0, dialogWidth * 0.8, dialogHeight * 0.2);
    this->loginDialog->centerInParent();
    connect(this->powerActionButtons[0], &LinboToolButton::clicked, this->loginDialog, &LinboLoginDialog::open);

    this->imageCreationDialog = new LinboImageCreationDialog(backend, parent);
    this->allDialogs.append(this->imageCreationDialog);
    this->imageCreationDialog->setGeometry(0, 0, dialogWidth, dialogHeight);
    this->imageCreationDialog->centerInParent();
    connect(this->osSelectionRow, &LinboOsSelectionRow::imageCreationRequested,
            this->imageCreationDialog, &LinboImageCreationDialog::open);
    connect(this->mainActions, &LinboMainActions::imageCreationRequested,
            this->imageCreationDialog, &LinboImageCreationDialog::open);

    this->imageUploadDialog = new LinboImageUploadDialog(backend, parent);
    this->allDialogs.append(this->imageUploadDialog);
    this->imageUploadDialog->setGeometry(0, 0, dialogWidth, dialogHeight * 0.3);
    this->imageUploadDialog->centerInParent();
    connect(this->osSelectionRow, &LinboOsSelectionRow::imageUploadRequested,
            this->imageUploadDialog, &LinboImageUploadDialog::open);
    connect(this->mainActions, &LinboMainActions::imageUploadRequested,
            this->imageUploadDialog, &LinboImageUploadDialog::open);

    this->terminalDialog = new LinboTerminalDialog(parent);
    this->allDialogs.append(this->terminalDialog);
    this->terminalDialog->setGeometry(0, 0, std::min(dialogWidth * 2, int(this->width() * 0.9)), dialogHeight);
    this->terminalDialog->centerInParent();
    connect(this->mainActions, &LinboMainActions::terminalRequested,
            this->terminalDialog, &LinboImageCreationDialog::open);

    this->confirmationDialog = new LinboConfirmationDialog(
        //% "Partition drive"
        qtTrId("dialog_partition_title"),
        //% "Are you sure? This will delete all data on your drive!"
        qtTrId("dialog_partition_question"),
        parent);
    this->allDialogs.append(this->confirmationDialog);

    this->confirmationDialog->setGeometry(0, 0, dialogWidth, dialogHeight * 0.2);
    this->confirmationDialog->centerInParent();
    connect(this->confirmationDialog, &LinboConfirmationDialog::accepted, this->backend, &LinboBackend::partitionDrive);
    connect(this->mainActions, &LinboMainActions::drivePartitioningRequested,
            this->confirmationDialog, &LinboDialog::open);

    this->registerDialog = new LinboRegisterDialog(backend, parent);
    this->allDialogs.append(this->registerDialog);
    this->registerDialog->setGeometry(0, 0, dialogWidth, dialogHeight * 0.7);
    this->registerDialog->centerInParent();
    connect(this->mainActions, &LinboMainActions::registrationRequested,
            this->registerDialog, &LinboImageCreationDialog::open);

    this->updateCacheDialog = new LinboUpdateCacheDialog(backend, parent);
    this->allDialogs.append(this->updateCacheDialog);
    this->updateCacheDialog->setGeometry(0, 0, dialogWidth * 0.5, dialogHeight * 0.3);
    this->updateCacheDialog->centerInParent();
    connect(this->mainActions, &LinboMainActions::cacheUpdateRequested,
            this->updateCacheDialog, &LinboImageCreationDialog::open);

    // attach eventFilter
    qApp->installEventFilter(this);
    this->handleLinboStateChanged(this->backend->state());
}

void LinboMainPage::handleLinboStateChanged(LinboBackend::LinboState newState) {
    bool powerActionButtonsVisible = false;
    int startActionsWidgetHeight;
    int osSelectionRowHeight;
    int clientInfoHeight = 0;
    bool useMinimalLayout = this->backend->config()->useMinimalLayout();

    switch (newState) {
    case LinboBackend::StartActionError:
    case LinboBackend::RootActionError:
        if(useMinimalLayout) {
            osSelectionRowHeight = this->height() * 0.2;
            startActionsWidgetHeight = this->height() * 0.4;
        }
        else {
            osSelectionRowHeight = this->height() * 0.15;
            startActionsWidgetHeight = this->height() * 0.45;
        }
        break;

    case LinboBackend::Idle:
        if(useMinimalLayout) {
            osSelectionRowHeight = this->height() * 0.2;
            startActionsWidgetHeight = this->height() * 0.2;
        }
        else {
            osSelectionRowHeight = this->height() * 0.4;
            startActionsWidgetHeight = this->height() * 0;
        }

        powerActionButtonsVisible = true;
        break;

    case LinboBackend::Root:
        if(useMinimalLayout) {
            osSelectionRowHeight = this->height() * 0.2;
            startActionsWidgetHeight = this->height() * 0.25;
        }
        else {
            osSelectionRowHeight = this->height() * 0.3;
            startActionsWidgetHeight = this->height() * 0.15;
        }

        powerActionButtonsVisible = true;
        break;

    case LinboBackend::RootTimeout:
        for(LinboDialog* dialog : this->allDialogs)
            dialog->autoClose();
    // fall through
    case LinboBackend::Partitioning:
    case LinboBackend::UpdatingCache:
    case LinboBackend::Registering:
    case LinboBackend::Disabled:
        osSelectionRowHeight = this->height() * 0;
        startActionsWidgetHeight = this->height() * 0.2;
        break;

    case LinboBackend::RootActionSuccess:
        osSelectionRowHeight = this->height() * 0;
        startActionsWidgetHeight = this->height() * 0.3;
        break;

    default:
        osSelectionRowHeight = this->height() * 0.2;
        startActionsWidgetHeight = this->height() * 0.2;
        break;
    }

    for(LinboPushButton* powerActionButton : this->powerActionButtons)
        if(powerActionButton == logoutActionButton && newState < LinboBackend::Root)
            powerActionButton->setVisible(false);
        else if(powerActionButton == logoutActionButton && newState >= LinboBackend::Root)
            powerActionButton->setVisible(powerActionButtonsVisible);
        else if(powerActionButton == rootActionButton && newState >= LinboBackend::Root)
            powerActionButton->setVisible(false);
        else
            powerActionButton->setVisible(powerActionButtonsVisible);

    if(this->showClientInfo)
        clientInfoHeight = this->height() * 0.1;

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

bool LinboMainPage::eventFilter(QObject *obj, QEvent *event) {
    Q_UNUSED(obj)

    if (event->type() == QEvent::MouseMove)
    {
        this->backend->restartRootTimeout();
    }
    else if(event->type() == QEvent::KeyPress) {
        this->backend->restartRootTimeout();

        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_F1) {
            this->f1Pressed = true;
        }
        else if(keyEvent->key() == Qt::Key_Escape) {
            switch (this->backend->state()) {
            case LinboBackend::Autostarting:
            case LinboBackend::RootTimeout:
                this->backend->cancelCurrentAction();
                break;

            case LinboBackend::StartActionError:
            case LinboBackend::RootActionError:
            case LinboBackend::RootActionSuccess:
                this->backend->resetMessage();
                break;

            case LinboBackend::Root: {
                bool someDialogOpen = false;
                for(LinboDialog* dialog : this->allDialogs) {
                    if(dialog->isVisible()) {
                        someDialogOpen = true;
                        break;
                    }
                }

                if(!someDialogOpen)
                    this->backend->logout();
                break;
            }

            default:
                break;
            }
        }
    }
    else if(event->type() == QEvent::KeyRelease) {
        this->backend->restartRootTimeout();

        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_F1 && this->clientInfoAnimation->state() == QPropertyAnimation::Stopped) {
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

            this->f1Pressed = false;
        }
    }
    return false;
}

