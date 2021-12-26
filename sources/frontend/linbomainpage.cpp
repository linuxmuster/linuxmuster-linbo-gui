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
    this->_inited = false;
    this->_showClientInfo = backend->config()->clientDetailsVisibleByDefault();
    this->_f1Pressed = false;

    this->_backend = backend;

#ifdef TEST_ENV
    //this->backend->login("Muster!");
#endif

    connect(this->_backend, &LinboBackend::stateChanged, this, &LinboMainPage::_handleLinboStateChanged);

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
    _osSelectionRow = new LinboOsSelectionRow(this->_backend);
    mainLayout->addWidget(_osSelectionRow);

    mainLayout->addStretch();

    // action buttons
    this->_mainActions = new LinboMainActions(this->_backend, this);
    this->_mainActions->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mainLayout->addWidget(this->_mainActions);

    this->_startActionWidgetAnimation = new QPropertyAnimation(this->_mainActions, "minimumSize");
    this->_startActionWidgetAnimation->setDuration(400);
    this->_startActionWidgetAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    mainLayout->addStretch();

    // version / network label
    QLabel* versionAndNetworkLabel = new QLabel(backend->config()->linboVersion() + "- GUI " + GUI_VERSION + " - " + this->_backend->config()->ipAddress() + " - F1");
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
    _clientInfo = new LinboClientInfo(this->_backend->config(), this);
    _clientInfo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _clientInfo->setFixedWidth(this->width() * 0.9);
    mainLayout->addWidget(_clientInfo);
    mainLayout->setAlignment(this->_clientInfo, Qt::AlignCenter);

    this->_clientInfoAnimation = new QPropertyAnimation(_clientInfo, "minimumSize");
    this->_clientInfoAnimation->setDuration(400);
    this->_clientInfoAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // power and settings Buttons

    QWidget* powerActionsLayoutWidget = new QWidget(this);
    int height = this->height() * 0.3;
    int width = height / 3;
    int margins = width * 0.1;
    int buttonWidth = width * 0.6;
    int x = this->width() - (width + margins);
    int y = this->height() - (height + margins);
    powerActionsLayoutWidget->setGeometry(QRect(x, y, width, height));

    _rootActionButton = new LinboToolButton(LinboTheme::SettingsIcon);
    this->_powerActionButtons.append(_rootActionButton);
    _rootActionButton->setFixedHeight(buttonWidth);
    _rootActionButton->setFixedWidth(buttonWidth);
    //% "Settings"
    _rootActionButton->setToolTip(qtTrId("settings"));

    _logoutActionButton = new LinboToolButton(LinboTheme::LogoutIcon);
    connect(_logoutActionButton, &LinboToolButton::clicked, this->_backend, &LinboBackend::logout);
    this->_powerActionButtons.append(_logoutActionButton);
    _logoutActionButton->setFixedHeight(buttonWidth);
    _logoutActionButton->setFixedWidth(buttonWidth);
    _logoutActionButton->setVisible(false);
    //% "Log out"
    _logoutActionButton->setToolTip(qtTrId("logout"));

    LinboPushButton* rebootActionButton = new LinboToolButton(LinboTheme::RebootIcon);
    connect(rebootActionButton, &LinboToolButton::clicked, this->_backend, &LinboBackend::reboot);
    this->_powerActionButtons.append(rebootActionButton);
    rebootActionButton->setFixedHeight(buttonWidth);
    rebootActionButton->setFixedWidth(buttonWidth);
    //% "Reboot"
    rebootActionButton->setToolTip(qtTrId("reboot"));

    LinboPushButton* shutdownActionButton = new LinboToolButton(LinboTheme::ShutdownIcon);
    connect(shutdownActionButton, &LinboToolButton::clicked, this->_backend, &LinboBackend::shutdown);
    this->_powerActionButtons.append(shutdownActionButton);
    shutdownActionButton->setFixedHeight(buttonWidth);
    shutdownActionButton->setFixedWidth(buttonWidth);
    //% "Shutdown"
    shutdownActionButton->setToolTip(qtTrId("shutdown"));

    QVBoxLayout* powerActionsLayout = new QVBoxLayout(powerActionsLayoutWidget);
    powerActionsLayout->setSpacing(0);
    powerActionsLayout->setContentsMargins(0,0,0,0);
    powerActionsLayout->addWidget(_rootActionButton);
    powerActionsLayout->addWidget(_logoutActionButton);
    powerActionsLayout->addWidget(rebootActionButton);
    powerActionsLayout->addWidget(shutdownActionButton);

    // Dialogs (for imaging stuff)
    this->_loginDialog = new LinboLoginDialog(this->_backend, this);
    int dialogHeight = gTheme->getSize(LinboTheme::DialogHeight);
    int dialogWidth = gTheme->getSize(LinboTheme::DialogWidth);
    this->_loginDialog->setGeometry( 0, 0, dialogWidth * 0.8, dialogHeight * 0.2);
    this->_loginDialog->centerInParent();
    connect(this->_powerActionButtons[0], &LinboToolButton::clicked, this->_loginDialog, &LinboLoginDialog::open);

    this->_imageCreationDialog = new LinboImageCreationDialog(backend, parent);
    this->_allDialogs.append(this->_imageCreationDialog);
    this->_imageCreationDialog->setGeometry(0, 0, dialogWidth, dialogHeight);
    this->_imageCreationDialog->centerInParent();
    connect(this->_osSelectionRow, &LinboOsSelectionRow::imageCreationRequested,
            this->_imageCreationDialog, &LinboImageCreationDialog::open);
    connect(this->_mainActions, &LinboMainActions::imageCreationRequested,
            this->_imageCreationDialog, &LinboImageCreationDialog::open);

    this->_imageUploadDialog = new LinboImageUploadDialog(backend, parent);
    this->_allDialogs.append(this->_imageUploadDialog);
    this->_imageUploadDialog->setGeometry(0, 0, dialogWidth, dialogHeight * 0.3);
    this->_imageUploadDialog->centerInParent();
    connect(this->_osSelectionRow, &LinboOsSelectionRow::imageUploadRequested,
            this->_imageUploadDialog, &LinboImageUploadDialog::open);
    connect(this->_mainActions, &LinboMainActions::imageUploadRequested,
            this->_imageUploadDialog, &LinboImageUploadDialog::open);

    this->_terminalDialog = new LinboTerminalDialog(parent);
    this->_allDialogs.append(this->_terminalDialog);
    this->_terminalDialog->setGeometry(0, 0, std::min(dialogWidth * 2, int(this->width() * 0.9)), dialogHeight);
    this->_terminalDialog->centerInParent();
    connect(this->_mainActions, &LinboMainActions::terminalRequested,
            this->_terminalDialog, &LinboTerminalDialog::open);

    this->_confirmationDialog = new LinboConfirmationDialog(
        //% "Partition drive"
        qtTrId("dialog_partition_title"),
        //% "Are you sure? This will delete all data on your drive!"
        qtTrId("dialog_partition_question"),
        parent);
    this->_allDialogs.append(this->_confirmationDialog);

    this->_confirmationDialog->setGeometry(0, 0, dialogWidth, dialogHeight * 0.2);
    this->_confirmationDialog->centerInParent();
    connect(this->_confirmationDialog, &LinboConfirmationDialog::accepted, this->_backend, &LinboBackend::partitionDrive);
    connect(this->_mainActions, &LinboMainActions::drivePartitioningRequested,
            this->_confirmationDialog, &LinboDialog::open);

    this->_registerDialog = new LinboRegisterDialog(backend, parent);
    this->_allDialogs.append(this->_registerDialog);
    this->_registerDialog->setGeometry(0, 0, dialogWidth, dialogHeight * 0.7);
    this->_registerDialog->centerInParent();
    connect(this->_mainActions, &LinboMainActions::registrationRequested,
            this->_registerDialog, &LinboRegisterDialog::open);

    this->_updateCacheDialog = new LinboUpdateCacheDialog(backend, parent);
    this->_allDialogs.append(this->_updateCacheDialog);
    this->_updateCacheDialog->setGeometry(0, 0, dialogWidth * 0.5, dialogHeight * 0.3);
    this->_updateCacheDialog->centerInParent();
    connect(this->_mainActions, &LinboMainActions::cacheUpdateRequested,
            this->_updateCacheDialog, &LinboUpdateCacheDialog::open);

    // attach eventFilter
    qApp->installEventFilter(this);
    this->_handleLinboStateChanged(this->_backend->state());
}

void LinboMainPage::_handleLinboStateChanged(LinboBackend::LinboState newState) {
    bool powerActionButtonsVisible = false;
    int startActionsWidgetHeight;
    int osSelectionRowHeight;
    int clientInfoHeight = 0;
    bool useMinimalLayout = this->_backend->config()->useMinimalLayout();

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
        for(LinboDialog* dialog : this->_allDialogs)
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

    for(LinboPushButton* powerActionButton : this->_powerActionButtons)
        if(powerActionButton == _logoutActionButton && newState < LinboBackend::Root)
            powerActionButton->setVisible(false);
        else if(powerActionButton == _logoutActionButton && newState >= LinboBackend::Root)
            powerActionButton->setVisible(powerActionButtonsVisible);
        else if(powerActionButton == _rootActionButton && newState >= LinboBackend::Root)
            powerActionButton->setVisible(false);
        else
            powerActionButton->setVisible(powerActionButtonsVisible);

    if(this->_showClientInfo)
        clientInfoHeight = this->height() * 0.1;

    if(this->_inited) {
        this->_startActionWidgetAnimation->setStartValue(QSize(this->width(), this->_mainActions->height()));
        this->_startActionWidgetAnimation->setEndValue(QSize(this->width(), startActionsWidgetHeight));
        this->_startActionWidgetAnimation->start();

        this->_osSelectionRow->setMinimumSizeAnimated(QSize(this->width(), osSelectionRowHeight));

        this->_clientInfoAnimation->setStartValue(QSize(this->width() * 0.9, this->_clientInfo->height()));
        this->_clientInfoAnimation->setEndValue(QSize(this->width() * 0.9, clientInfoHeight));
        this->_clientInfoAnimation->start();
    }
    else {
        this->_mainActions->setMinimumSize(this->width(), startActionsWidgetHeight);
        this->_osSelectionRow->setMinimumSize(this->width(), osSelectionRowHeight);
        this->_clientInfo->setMinimumSize(this->width() * 0.9, clientInfoHeight);
    }

    this->_inited = true;
}

bool LinboMainPage::eventFilter(QObject *obj, QEvent *event) {
    Q_UNUSED(obj)

    if (event->type() == QEvent::MouseMove)
    {
        this->_backend->restartRootTimeout();
    }
    else if(event->type() == QEvent::KeyPress) {
        this->_backend->restartRootTimeout();

        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_F1) {
            this->_f1Pressed = true;
        }
        else if(keyEvent->key() == Qt::Key_Escape) {
            switch (this->_backend->state()) {
            case LinboBackend::Autostarting:
            case LinboBackend::RootTimeout:
                this->_backend->cancelCurrentAction();
                break;

            case LinboBackend::StartActionError:
            case LinboBackend::RootActionError:
            case LinboBackend::RootActionSuccess:
                this->_backend->resetMessage();
                break;

            case LinboBackend::Root: {
                bool someDialogOpen = false;
                for(LinboDialog* dialog : this->_allDialogs) {
                    if(dialog->isVisible()) {
                        someDialogOpen = true;
                        break;
                    }
                }

                if(!someDialogOpen)
                    this->_backend->logout();
                break;
            }

            default:
                break;
            }
        }
    }
    else if(event->type() == QEvent::KeyRelease) {
        this->_backend->restartRootTimeout();

        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_F1 && this->_clientInfoAnimation->state() == QPropertyAnimation::Stopped) {
            this->_showClientInfo = !this->_showClientInfo;

            if(this->_showClientInfo) {
                this->_clientInfoAnimation->setStartValue(QSize(this->width() * 0.9, this->_clientInfo->height()));
                this->_clientInfoAnimation->setEndValue(QSize(this->width() * 0.9, this->height() * 0.1));
                this->_clientInfoAnimation->start();
            }
            else {
                this->_clientInfoAnimation->setStartValue(QSize(this->width() * 0.9, this->_clientInfo->height()));
                this->_clientInfoAnimation->setEndValue(QSize(this->width() * 0.9, 0));
                this->_clientInfoAnimation->start();
            }

            this->_f1Pressed = false;
        }
    }
    return false;
}

