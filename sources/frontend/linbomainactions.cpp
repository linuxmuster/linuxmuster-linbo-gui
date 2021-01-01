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

#include "linbomainactions.h"

LinboMainActions::LinboMainActions(LinboBackend* backend, QWidget *parent) : QWidget(parent)
{
    this->backend = backend;
    connect(this->backend, SIGNAL(currentOsChanged(LinboOs*)), this, SLOT(handleCurrentOsChanged(LinboOs*)));
    connect(this->backend, SIGNAL(stateChanged(LinboBackend::LinboState)), this, SLOT(handleLinboStateChanged(LinboBackend::LinboState)));
    connect(this->backend, SIGNAL(autostartTimeoutProgressChanged()), this, SLOT(handleAutostartTimeoutProgressChanged()));
    connect(this->backend->getLogger(), SIGNAL(latestLogChanged(const LinboLogger::LinboLog&)), this, SLOT(handleLatestLogChanged(const LinboLogger::LinboLog&)));

    this->stackView = new QModernStackedWidget(this);

    this->inited = false;

    // Action Buttons
    this->buttonWidget = new QWidget();

    this->startOsButton = new QModernPushButton(":/svgIcons/startActionIcons/start.svg", this->buttonWidget);
    connect(this->startOsButton, SIGNAL(clicked()), this->backend, SLOT(startCurrentOs()));

    this->syncOsButton = new QModernPushButton(":/svgIcons/startActionIcons/sync.svg", this->buttonWidget);
    connect(this->syncOsButton, SIGNAL(clicked()), this->backend, SLOT(syncCurrentOs()));

    this->reinstallOsButton = new QModernPushButton(":/svgIcons/startActionIcons/reinstall.svg", this->buttonWidget);
    connect(this->reinstallOsButton, SIGNAL(clicked()), this->backend, SLOT(reinstallCurrentOs()));

    //= main_noBaseImage
    this->noBaseImageLabel = new QLabel(tr("No baseimage defined"), this->buttonWidget);
    this->noBaseImageLabel->setStyleSheet("QLabel { color : red; }");
    this->noBaseImageLabelFont = QFont("Segoe UI");
    this->noBaseImageLabel->setFont(this->noBaseImageLabelFont);
    this->noBaseImageLabel->hide();
    this->noBaseImageLabel->setAlignment(Qt::AlignCenter);

    this->stackView->addWidget(this->buttonWidget);

    // Progress bar
    this->progressBarWidget = new QWidget();
    this->progressBar = new QModernProgressBar(this->progressBarWidget);
    this->progressBar->setRange(0,1000);
    this->progressBar->setIndeterminate(true);

    this->logLabel = new QLabel("", this->progressBarWidget);
    this->logLabel->setAlignment(Qt::AlignCenter);

    this->logFont = QFont("Segoe UI");
    this->logLabel->setFont(this->logFont);

    this->cancelButton = new QModernPushButton(":/svgIcons/cancel.svg", this->progressBarWidget);
    connect(this->cancelButton, SIGNAL(clicked()), this->backend, SLOT(cancelCurrentAction()));

    this->stackView->addWidget(this->progressBarWidget);

    // Message widget
    this->messageWidget = new QWidget();
    this->messageLabel = new QLabel("", this->messageWidget);
    this->messageLabel->setAlignment(Qt::AlignCenter);

    this->messageDetailsLabel = new QLabel("", this->messageWidget);
    this->messageDetailsLabel->setAlignment(Qt::AlignLeft);
    //this->messageDetailsLabel->setWordWrap(true);

    this->resetMessageButton = new QModernPushButton(":/svgIcons/back.svg", this->messageWidget);
    connect(this->resetMessageButton, SIGNAL(clicked()), this->backend, SLOT(resetMessage()));

    this->stackView->addWidget(this->messageWidget);

    // Root widget
    this->rootWidget = new QWidget();
    this->rootLayout = new QVBoxLayout(this->rootWidget);
    this->rootLayout->setAlignment(Qt::AlignCenter);

    QModernPushButton* buttonCache;

    if(this->backend->getConfig()->getUseMinimalLayout()) {
        //= main_root_button_createImage
        buttonCache = new QModernPushButton(":svgIcons/image.svg", tr("Create image"));
        this->rootActionButtons.append(buttonCache);
        connect(buttonCache, &QModernPushButton::clicked, this, &LinboMainActions::imageCreationRequested);

        //= main_root_button_uploadImage
        this->rootActionButtons.append(new QModernPushButton(":svgIcons/upload.svg", tr("Upload image")));
        connect(buttonCache, &QModernPushButton::clicked, this, &LinboMainActions::imageUploadRequested);
    }

    //= main_root_button_openTerminal
    buttonCache = new QModernPushButton(":svgIcons/terminal.svg", tr("Open terminal"));
    connect(buttonCache, &QModernPushButton::clicked, this, &LinboMainActions::terminalRequested);
    this->rootActionButtons.append(buttonCache);

    //= main_root_button_updateCache
    buttonCache = new QModernPushButton(":svgIcons/startActionIcons/sync.svg", tr("Update cache"));
    this->rootActionButtons.append(buttonCache);
    connect(buttonCache, &QModernPushButton::clicked, this, &LinboMainActions::cacheUpdateRequested);

    //= main_root_button_partitionDrive
    buttonCache = new QModernPushButton(":svgIcons/partition.svg", tr("Partition drive"));
    this->rootActionButtons.append(buttonCache);
    connect(buttonCache, &QModernPushButton::clicked, this, &LinboMainActions::drivePartitioningRequested);

    //= main_root_button_register
    buttonCache = new QModernPushButton(":svgIcons/register.svg", tr("Register"));
    this->rootActionButtons.append(buttonCache);
    connect(buttonCache, &QModernPushButton::clicked, this, &LinboMainActions::registrationRequested);

    for(QModernPushButton* button : this->rootActionButtons)
        this->rootLayout->addWidget(button);

    if(this->backend->getConfig()->getUseMinimalLayout()) {
        // insert a line to separate image specific and global actions
        QFrame* separatorLine = new QFrame();
        separatorLine->setStyleSheet("QFrame {color: #cccccc;}");
        separatorLine->setFrameShape(QFrame::HLine);
        this->rootLayout->insertWidget(2, separatorLine);
    }

    this->rootLayout->addStretch();

    this->stackView->addWidget(this->rootWidget);

    // empty widget
    this->emptyWidget = new QWidget();
    this->stackView->addWidget(this->emptyWidget);


    connect(this->stackView, SIGNAL(currentChanged(int)), this, SLOT(resizeAndPositionAllItems()));
    this->handleLinboStateChanged(this->backend->getState());
}

void LinboMainActions::resizeAndPositionAllItems() {

    // stack view
    this->stackView->setFixedSize(this->size());

    // Action buttons
    // bring buttons in correct order:
    LinboOs* selectedOs = this->backend->getCurrentOs();
    LinboOs::LinboOsStartAction defaultAction = LinboOs::UnknownAction;
    if(selectedOs != nullptr)
        defaultAction = selectedOs->getDefaultAction();

    int syncOsPosition = 2;
    int startOsPosition = 0;
    int reinstallOsPosition = 1;

    switch (defaultAction) {
    case LinboOs::StartOs:
        break;
    case LinboOs::SyncOs:
        syncOsPosition = 0;
        startOsPosition = 1;
        reinstallOsPosition = 2;
        break;
    case LinboOs::ReinstallOs:
        syncOsPosition = 1;
        startOsPosition = 2;
        reinstallOsPosition = 0;
        break;
    default:
        break;
    }

    while (this->actionButtons.length() < 3)
        this->actionButtons.append(nullptr);

    this->actionButtons[startOsPosition] = this->startOsButton;
    this->actionButtons[syncOsPosition] = this->syncOsButton;
    this->actionButtons[reinstallOsPosition] = this->reinstallOsButton;

    // check for disabled actions
    QList<bool> positionsEnabled;
    while(positionsEnabled.length() < 3)
        positionsEnabled.append(false);

    if(selectedOs != nullptr) {
        positionsEnabled[startOsPosition] = selectedOs->getStartActionEnabled();
        positionsEnabled[syncOsPosition] = selectedOs->getSyncActionEnabled();
        positionsEnabled[reinstallOsPosition] = selectedOs->getReinstallActionEnabled();
    }

    QList<QRect> geometries;
    while (geometries.length() < 3)
        geometries.append(QRect());

    // move buttons into place
    this->buttonWidget->setGeometry(0,0,this->width(), this->height());

    int buttonSpacing = this->buttonWidget->height() * 0.1;
    int defaultButtonHeight = this->buttonWidget->height() * 0.6;
    geometries[0] = QRect((this->buttonWidget->width() - defaultButtonHeight) / 2, 0,defaultButtonHeight, defaultButtonHeight);


    int secondaryButtonHeight = this->buttonWidget->height() * 0.3;
    if(positionsEnabled[1] && positionsEnabled[2]) {
        // place buttons besides each other
        geometries[1] = QRect(
                    this->buttonWidget->width() / 2 - secondaryButtonHeight - buttonSpacing / 2,
                    defaultButtonHeight + buttonSpacing,
                    secondaryButtonHeight,
                    secondaryButtonHeight
                    );

        geometries[2] = QRect(
                    this->buttonWidget->width() / 2 + buttonSpacing / 2,
                    defaultButtonHeight + buttonSpacing,
                    secondaryButtonHeight,
                    secondaryButtonHeight
                    );
    }
    else {
        // place buttons on top of each other
        geometries[1] = QRect(
                    this->buttonWidget->width() / 2 - secondaryButtonHeight / 2,
                    defaultButtonHeight + buttonSpacing,
                    secondaryButtonHeight,
                    secondaryButtonHeight
                    );

        geometries[2] = geometries[1];
    }

    for(int i = 0; i < this->actionButtons.length(); i++) {
        if(this->inited) {
            this->actionButtons[i]->setVisibleAnimated(positionsEnabled[i]);
            this->actionButtons[i]->setGeometryAnimated(geometries[i]);
        }
        else {
            // don't animate the first time
            this->actionButtons[i]->setVisible(positionsEnabled[i]);
            this->actionButtons[i]->setGeometry(geometries[i]);
        }
    }

    if(selectedOs != nullptr && selectedOs->getBaseImage() == nullptr) {
        int noBaseImageLabelHeight = this->buttonWidget->height() * 0.2;
        this->noBaseImageLabelFont.setPixelSize(noBaseImageLabelHeight <= 0 ? 1:noBaseImageLabelHeight * 0.8);
        this->noBaseImageLabel->setFont(this->noBaseImageLabelFont);
        this->noBaseImageLabel->setGeometry(0, (this->buttonWidget->height() - noBaseImageLabelHeight) / 2, this->buttonWidget->width(), noBaseImageLabelHeight);
        this->noBaseImageLabel->show();
    }
    else {
        this->noBaseImageLabel->hide();
    }

    // Progress bar
    this->progressBarWidget->setGeometry(0,0,this->width(), this->height());
    int progressBarHeight = this->progressBarWidget->height() * 0.1;
    int progressBarWidth = this->progressBarWidget->width() * 0.5;
    int logLabelHeight = progressBarHeight * 2;
    int logLabelWidth = this->progressBarWidget->width() * 0.8;
    int cancelButtonWidth = this->progressBarWidget->height() * 0.4;

    this->logFont.setPixelSize(logLabelHeight <= 0 ? 1:logLabelHeight * 0.8);
    this->logLabel->setFont(this->logFont);
    this->logLabel->setGeometry((this->progressBarWidget->width() - logLabelWidth) / 2, 0, logLabelWidth, logLabelHeight);

    progressBar->setGeometry((this->progressBarWidget->width() - progressBarWidth) / 2, this->logLabel->y() + logLabelHeight + this->progressBarWidget->height() * 0.15, progressBarWidth, progressBarHeight);

    this->cancelButton->setGeometry((this->progressBarWidget->width() - cancelButtonWidth) / 2, this->progressBar->y() + progressBarHeight + this->progressBarWidget->height() * 0.05, cancelButtonWidth, cancelButtonWidth);

    // Message widget
    this->messageWidget->setGeometry(QRect(0,0, this->width(), this->height()));

    if(this->messageDetailsLabel->isVisible()) {
        this->messageLabel->setGeometry(0,0, this->width(), this->height() * 0.2);

        int messageDetailsFontHeight = this->height() * 0.6;
        QFont messageDetailsFont = this->messageDetailsLabel->font();
        messageDetailsFont.setPixelSize(int(messageDetailsFontHeight / 12.5) <= 0 ? 1:messageDetailsFontHeight / 12.5);
        this->messageDetailsLabel->setFont(messageDetailsFont);

        QFont messageFont = this->messageLabel->font();
        messageFont.setBold(true);
        messageFont.setPixelSize(messageDetailsFont.pixelSize() * 1.5);
        this->messageLabel->setFont(messageFont);

        this->messageDetailsLabel->setMaximumWidth(this->width() * 0.8);
        this->messageDetailsLabel->move((this->width() - this->messageDetailsLabel->width()) / 2, this->messageLabel->height());
        this->messageDetailsLabel->setFixedHeight(messageDetailsFontHeight);
    }
    else {
        QFont messageFont = this->messageLabel->font();
        messageFont.setBold(true);
        messageFont.setPixelSize(int(this->height() * 0.1) <= 0 ? 1:this->height() * 0.1);
        this->messageLabel->setFont(messageFont);
        this->messageLabel->setGeometry(0, 0, this->width(), this->height());
    }

    int resetMessageButtonSize = this->height() * 0.15;

    this->resetMessageButton->setGeometry((this->width() - resetMessageButtonSize) / 2, this->height() - resetMessageButtonSize * 1.1, resetMessageButtonSize, resetMessageButtonSize);

    // Root widget
    this->rootWidget->setGeometry(QRect(0,0, this->width(), this->height()));

    int rootActionButtonHeight = this->height() / this->rootActionButtons.length() - this->height() * 0.03;
    this->rootLayout->setSpacing(this->height() * 0.03);

    for(QModernPushButton* button : this->rootActionButtons) {
        button->setFixedHeight(rootActionButtonHeight);
        button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    }

    this->inited = true;
}

void LinboMainActions::resizeEvent(QResizeEvent *event) {
    this->resizeAndPositionAllItems();
    QWidget::resizeEvent(event);
}

void LinboMainActions::handleCurrentOsChanged(LinboOs* newOs) {
    Q_UNUSED(newOs)
    this->resizeAndPositionAllItems();
}

void LinboMainActions::handleLinboStateChanged(LinboBackend::LinboState newState) {

    QWidget* currentWidget = nullptr;

    switch (newState) {
    case LinboBackend::Autostarting:
        this->progressBar->setIndeterminate(false);
        this->progressBar->setReversed(true);
        this->progressBar->setValue(0);
        currentWidget = this->progressBarWidget;
        break;
    case LinboBackend::Idle:
        if(this->backend->getConfig()->getUseMinimalLayout())
            currentWidget = this->buttonWidget;
        else
            currentWidget = this->emptyWidget;
        break;

    case LinboBackend::Starting:
    case LinboBackend::Syncing:
    case LinboBackend::Reinstalling:
    case LinboBackend::Partitioning:
    case LinboBackend::UpdatingCache:
        this->progressBar->setIndeterminate(true);
        this->progressBar->setReversed(false);
        currentWidget = this->progressBarWidget;
        break;

    case LinboBackend::StartActionError:
    case LinboBackend::RootActionError: {
        QList<LinboLogger::LinboLog> chaperLogs = this->backend->getLogger()->getLogsOfCurrentChapter();
        //= main_message_processCrashed
        this->messageLabel->setText(tr("The process %1 crashed:").arg("\"" + chaperLogs[chaperLogs.length()-1].message + "\""));
        QString errorDetails;
        if(chaperLogs.length() == 0)
            //= main_message_noLogs
            errorDetails = "<b>" + tr("No logs before this crash") + "</b>";
        else if(LinboLogger::getFilterLogs(chaperLogs, LinboLogger::StdErr).length() == 0){
            //= main_message_lastLogs
            errorDetails = "<b>" + tr("The last logs before the crash were:") + "</b><br>";
            errorDetails += LinboLogger::logsToStacktrace(chaperLogs, 8).join("<br>");
        }
        else {
            //= main_message_lastErrors
            errorDetails = "<b>" + tr("The last errors before the crash were:") + "</b><br>";
            errorDetails += LinboLogger::logsToStacktrace(LinboLogger::getFilterLogs(chaperLogs, LinboLogger::LinboGuiError | LinboLogger::StdErr), 8).join("<br>");
        }

        //= main_message_askForHelp
        errorDetails += "<br><br><b>" + tr("Please ask your system administrator for help.") + "</b>";

        this->messageDetailsLabel->setText("<html>" + errorDetails + "</html>");
        this->messageLabel->setStyleSheet("QLabel { color : red; }");
        this->messageDetailsLabel->setStyleSheet("QLabel { color : red; }");
        this->messageDetailsLabel->setVisible(true);
        currentWidget = this->messageWidget;
        break;
    }

    case LinboBackend::RootActionSuccess: {
        QList<LinboLogger::LinboLog> chaperLogs = this->backend->getLogger()->getLogsOfCurrentChapter();
        //= main_message_processFinishedSuccessfully
        this->messageLabel->setText(tr("The process %1 finished successfully.").arg("\"" + chaperLogs[chaperLogs.length()-1].message + "\"" ));
        this->messageDetailsLabel->setText("");
        this->messageLabel->setStyleSheet("QLabel { color : green; }");
        currentWidget = this->messageWidget;
        this->messageDetailsLabel->setVisible(false);
        break;
    }

    case LinboBackend::Root:
        currentWidget = this->rootWidget;
        break;

    default:
        break;
    }

    if(this->inited)
        this->stackView->setCurrentWidgetAnimated(currentWidget);
    else
        this->stackView->setCurrentWidget(currentWidget);
}


void LinboMainActions::handleLatestLogChanged(const LinboLogger::LinboLog& latestLog) {
    if(this->backend->getState() == LinboBackend::Idle)
        return;

    QString logColor = "black";
    switch (latestLog.type) {
    case LinboLogger::StdErr:
        logColor = this->backend->getConfig()->getConsoleFontcolorStderr();
        break;
    case LinboLogger::StdOut:
        // TODO?? logColor = this->backend->getConfig()->getConsoleFontcolorStdout();
        break;
    default:
        break;
    }
    this->logLabel->setStyleSheet("QLabel { color : " + logColor + "; }");
    this->logLabel->setText(latestLog.message);
}

void LinboMainActions::handleAutostartTimeoutProgressChanged() {
    if(this->backend->getState() != LinboBackend::Autostarting)
        return;

    this->progressBar->setValue(1000 - this->backend->getAutostartTimeoutProgress() * 1000);

    QString actionString;
    switch (this->backend->getCurrentOs()->getDefaultAction()) {
    case LinboOs::SyncOs:
        actionString = "Syncing and starting";
        break;
    case LinboOs::ReinstallOs:
        actionString = "Reinstalling and starting";
        break;
    default:
        actionString = "Starting";
        break;
    }

    this->logLabel->setText(actionString + " " + this->backend->getCurrentOs()->getName() + " in " + QString::number(this->backend->getAutostartTimeoutRemainingSeconds()) + " seconds.");
}
