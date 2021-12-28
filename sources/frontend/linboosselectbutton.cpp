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

#include "linboosselectbutton.h"

LinboOsSelectButton::LinboOsSelectButton(QString icon, LinboOs* os, LinboBackend* backend, QButtonGroup* buttonGroup, QWidget* parent) : QWidget(parent)
{
    this->_inited = false;
    this->_buttonGroup = buttonGroup;
    this->_os = os;
    this->_backend = backend;
    this->_shouldBeVisible = true;
    this->_showDefaultAction = true;
    this->_osNameLabel = nullptr;

    if(!QFile::exists(icon) || !icon.endsWith(".svg")) {
        icon = gTheme->iconPath(LinboTheme::DefaultOsIcon);
    }

    connect(this->_backend, &LinboBackend::stateChanged, this, &LinboOsSelectButton::_handleBackendStateChange);

    QMap<LinboOs::LinboOsStartAction, QString> defaultStartActionOverlayPaths = {
        {LinboOs::StartOs, gTheme->iconPath(LinboTheme::OverlayStartIcon)},
        {LinboOs::SyncOs, gTheme->iconPath(LinboTheme::OverlaySyncIcon)},
        {LinboOs::ReinstallOs, gTheme->iconPath(LinboTheme::OverlayReinstallIcon)},
        {LinboOs::UnknownAction, ""}
    };

    QString defaultActionOverlayPath = defaultStartActionOverlayPaths[this->_os->defaultAction()];
    if(!this->_os->actionEnabled(this->_os->defaultAction()))
        defaultActionOverlayPath = "";

    this->_defaultStartActionOverlay = new LinboPushButtonOverlay (
        LinboPushButtonOverlay::Background,
        new QSvgWidget(defaultActionOverlayPath),
        false
    );

    this->_defaultRootActionOverlay = new LinboPushButtonOverlay (
        LinboPushButtonOverlay::Background,
        new QSvgWidget(gTheme->iconPath(LinboTheme::OverlayImageIcon)),
        false
    );

    this->_button = new LinboPushButton(icon, "", {this->_defaultStartActionOverlay, this->_defaultRootActionOverlay}, this);
    this->setToolTip(this->_os->description());

    connect(this->_button, &LinboPushButton::clicked, this, &LinboOsSelectButton::_handlePrimaryButtonClicked);

    QMap<LinboOs::LinboOsStartAction, QString> startActionButtonIcons = {
        //% "Start %1"
        {LinboOs::StartOs, gTheme->iconPath(LinboTheme::StartLegacyIcon)},
        //% "Sync and start %1"
        {LinboOs::SyncOs, gTheme->iconPath(LinboTheme::SyncLegacyIcon)},
        //% "Reinstall %1"
        {LinboOs::ReinstallOs, gTheme->iconPath(LinboTheme::ReinstallLegacyIcon)}
    };

    for(auto i = startActionButtonIcons.begin(); i != startActionButtonIcons.end(); i++) {
        LinboOs::LinboOsStartAction startAction = i.key();
        QString startActionIconPath = i.value();
        bool disabled = !this->_os->actionEnabled(startAction) || this->_os->defaultAction() == startAction;

        if(disabled)
            continue;

        LinboPushButton* actionButton = new LinboPushButton(startActionIconPath, this);
        actionButton->setToolTip(this->_getTooltipContentForAction(startAction));

        actionButton->setEnabled(!disabled);
        actionButton->setVisible(false);
        this->_startActionButtons.append(actionButton);

        switch (startAction) {
        case LinboOs::StartOs:
            connect(actionButton, &LinboPushButton::clicked, this->_os, &LinboOs::executeStart);
            break;
        case LinboOs::SyncOs:
            connect(actionButton, &LinboPushButton::clicked, this->_os, &LinboOs::executeSync);
            break;
        case LinboOs::ReinstallOs:
            connect(actionButton, &LinboPushButton::clicked, this->_os, &LinboOs::executeReinstall);
            break;
        default:
            break;
        }
    }

    // root action button
    LinboPushButton* actionButton = new LinboPushButton(gTheme->iconPath(LinboTheme::UploadLegacyIcon), this);
    actionButton->setGeometry(0,0,0,0);
    actionButton->setVisible(false);
    //% "Upload image of %1"
    actionButton->setToolTip(qtTrId("uploadImageOfOS").arg(this->_os->name()));
    connect(actionButton, &LinboPushButton::clicked, this, [=] {
        emit this->imageUploadRequested(this->_os);
    });
    this->_rootActionButtons.append(actionButton);

    // OS name label
    this->_osNameLabel = new QLabel(this);
    if(this->_os->baseImage() != nullptr) {
        this->_osNameLabel->setText(this->_os->name());
        this->_osNameLabel->setStyleSheet(gTheme->insertValues("QLabel {color: %TextColor}"));
    }
    else {
        //% "No baseimage defined"
        this->_osNameLabel->setText(qtTrId("main_noBaseImage"));
        this->_osNameLabel->setStyleSheet("QLabel {color: red}");
    }

    this->_osNameLabel->setWordWrap(true);
    this->_osNameLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);


    this->_button->setCheckable(true);
    this->_buttonGroup->addButton(this->_button);
    this->_handleBackendStateChange(this->_backend->state());

    QWidget::setVisible(true);
}

void LinboOsSelectButton::_handlePrimaryButtonClicked() {
    if(this->_backend->state() == LinboBackend::Idle)
        switch (this->_os->defaultAction()) {
        case LinboOs::StartOs:
            this->_os->executeStart();
            break;
        case LinboOs::SyncOs:
            this->_os->executeSync();
            break;
        case LinboOs::ReinstallOs:
            this->_os->executeReinstall();
            break;
        default:
            break;
        }
    else if (this->_backend->state() == LinboBackend::Root) {
        emit this->imageCreationRequested(this->_os);
    }
}

LinboOs* LinboOsSelectButton::_getOs() {
    return this->_os;
}

void LinboOsSelectButton::_setVisibleAnimated(bool visible) {
    this->_shouldBeVisible = visible;
    this->_button->setVisibleAnimated(visible);
    if(this->_osNameLabel != nullptr)
        this->_osNameLabel->setVisible(visible);

    this->_updateActionButtonVisibility();
}

void LinboOsSelectButton::setVisible(bool visible) {
    this->_shouldBeVisible = visible;
    this->_button->setVisible(visible);
    if(this->_osNameLabel != nullptr)
        this->_osNameLabel->setVisible(visible);

    this->_updateActionButtonVisibility(true);
}

void LinboOsSelectButton::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    this->_button->setGeometry(0, 0, event->size().height(), event->size().height());

    int x = this->height();
    int spacing = this->width() * 0.04;
    int actionButtonSize = (this->width() - this->height()) / 4 - spacing;

    if(this->height() <= 0 || this->width() / this->height() < 1.1) {
        // only the big button is visible
        actionButtonSize = 0;
        x = 0;
        spacing = 0;
    }

    for(LinboPushButton* actionButton : this->_startActionButtons) {
        actionButton->setGeometry(x + spacing, this->height() - actionButtonSize, actionButtonSize, actionButtonSize);
        x += actionButtonSize + spacing;
    }

    x = this->height();

    for(LinboPushButton* actionButton : this->_rootActionButtons) {
        actionButton->setGeometry(x + spacing, this->height() - actionButtonSize, actionButtonSize, actionButtonSize);
        x += actionButtonSize + spacing;
    }

    this->_osNameLabel->setGeometry(this->height() + spacing, 0, this->width() - this->height() - spacing * 2, actionButtonSize);
    QFont fontCache = this->_osNameLabel->font();
    fontCache.setPixelSize(gTheme->toFontSize(actionButtonSize * 0.5));
    this->_osNameLabel->setFont(fontCache);

    this->_updateActionButtonVisibility();
}

void LinboOsSelectButton::_handleBackendStateChange(LinboBackend::LinboState state) {
    this->_showDefaultAction = false;

    bool checkedOverlayMuted = true;

    switch (state) {
    case LinboBackend::Idle:
        this->_button->setToolTip(this->_getTooltipContentForAction(this->_os->defaultAction()));

        this->_showDefaultAction = true;

        checkedOverlayMuted = false;
        break;
    case LinboBackend::Root:
        //% "Create image of %1"
        this->_button->setToolTip(qtTrId("createImageOfOS").arg(this->_os->name()));

        this->_showDefaultAction = true;

        checkedOverlayMuted = false;
        break;
    case LinboBackend::Autostarting:
        this->_showDefaultAction = true;
        break;
    default:
        break;
    }

    this->_button->setOverlayTypeMuted(LinboPushButtonOverlay::OnChecked, checkedOverlayMuted);
    this->_updateActionButtonVisibility();
}

QString LinboOsSelectButton::_getTooltipContentForAction(LinboOs::LinboOsStartAction action) {
    QMap<LinboOs::LinboOsStartAction, QString> startActionButtonIcons = {
        //% "Start %1"
        {LinboOs::StartOs, qtTrId("startOS")},
        //% "Sync and start %1"
        {LinboOs::SyncOs, qtTrId("syncOS")},
        //% "Reinstall %1"
        {LinboOs::ReinstallOs, qtTrId("reinstallOS")}
    };

    return startActionButtonIcons[action].arg(this->_os->name());
}

void LinboOsSelectButton::_updateActionButtonVisibility(bool doNotAnimate) {

    bool startActionVisible = this->_shouldBeVisible && this->_backend->state() < LinboBackend::Root;
    bool rootActionVisible = this->_shouldBeVisible && this->_backend->state() >= LinboBackend::Root;

    if(this->_inited && !doNotAnimate) {
        this->_defaultStartActionOverlay->setVisibleAnimated(startActionVisible && this->_showDefaultAction);
        this->_defaultRootActionOverlay->setVisibleAnimated(rootActionVisible && this->_showDefaultAction);
    }
    else {
        this->_defaultStartActionOverlay->setVisible(startActionVisible && this->_showDefaultAction);
        this->_defaultRootActionOverlay->setVisible(rootActionVisible && this->_showDefaultAction);
    }

    for(LinboPushButton* actionButton : this->_startActionButtons)
        if(this->_inited && !doNotAnimate)
            actionButton->setVisibleAnimated(startActionVisible);
        else
            actionButton->setVisible(startActionVisible);

    for(LinboPushButton* actionButton : this->_rootActionButtons)
        if(this->_inited && !doNotAnimate)
            actionButton->setVisibleAnimated(rootActionVisible);
        else
            actionButton->setVisible(rootActionVisible);

    this->_inited = true;
}

