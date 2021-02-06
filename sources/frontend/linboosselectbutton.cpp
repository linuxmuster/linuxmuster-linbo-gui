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

LinboOsSelectButton::LinboOsSelectButton(QString icon, LinboOs* os, QButtonGroup* buttonGroup, QWidget* parent) : QWidget(parent)
{
    this->showActionButtons = false;
    this->inited = false;
    this->buttonGroup = buttonGroup;
    this->os = os;
    this->shouldBeVisible = true;
    this->showDefaultAction = true;
    this->osNameLabel = nullptr;

    if(!QFile::exists(icon) || !icon.endsWith(".svg")) {
        icon = gTheme->getIconPath(LinboGuiTheme::DefaultOsIcon);
    }

    connect(os->getBackend(), &LinboBackend::stateChanged, this, &LinboOsSelectButton::handleBackendStateChange);

    QMap<LinboOs::LinboOsStartAction, QString> defaultStartActionOverlayPaths = {
        {LinboOs::StartOs, gTheme->getIconPath(LinboGuiTheme::OverlayStartIcon)},
        {LinboOs::SyncOs, gTheme->getIconPath(LinboGuiTheme::OverlaySyncIcon)},
        {LinboOs::ReinstallOs, gTheme->getIconPath(LinboGuiTheme::OverlayReinstallIcon)},
        {LinboOs::UnknownAction, ""}
    };

    LinboPushButtonOverlay* checkedOverlay= new LinboPushButtonOverlay (
        LinboPushButtonOverlay::OnChecked,
        new QSvgWidget(gTheme->getIconPath(LinboGuiTheme::OverlayCheckedIcon)),
        false
    );

    QString defaultActionOverlayPath = defaultStartActionOverlayPaths[this->os->getDefaultAction()];
    if(!this->os->getActionEnabled(this->os->getDefaultAction()))
        defaultActionOverlayPath = "";

    this->defaultStartActionOverlay = new LinboPushButtonOverlay (
        LinboPushButtonOverlay::Background,
        new QSvgWidget(defaultActionOverlayPath),
        false
    );

    this->defaultRootActionOverlay = new LinboPushButtonOverlay (
        LinboPushButtonOverlay::Background,
        new QSvgWidget(gTheme->getIconPath(LinboGuiTheme::OverlayImageIcon)),
        false
    );

    this->button = new LinboPushButton(icon, "", {checkedOverlay, this->defaultStartActionOverlay, this->defaultRootActionOverlay}, this);
    this->setToolTip(this->os->getDescription());

    if(!os->getBackend()->getConfig()->getUseMinimalLayout()) {
        connect(this->button, &LinboPushButton::clicked, this, &LinboOsSelectButton::handlePrimaryButtonClicked);

        QMap<LinboOs::LinboOsStartAction, QString> startActionButtonIcons = {
            //% "Start %1"
            {LinboOs::StartOs, gTheme->getIconPath(LinboGuiTheme::StartLegacyIcon)},
            //% "Sync and start %1"
            {LinboOs::SyncOs, gTheme->getIconPath(LinboGuiTheme::SyncLegacyIcon)},
            //% "Reinstall %1"
            {LinboOs::ReinstallOs, gTheme->getIconPath(LinboGuiTheme::ReinstallLegacyIcon)}
        };

        for(LinboOs::LinboOsStartAction startAction : startActionButtonIcons.keys()) {
            QString startActionIconPath = startActionButtonIcons[startAction];
            bool disabled = !this->os->getActionEnabled(startAction) || this->os->getDefaultAction() == startAction;

            if(disabled)
                continue;

            LinboPushButton* actionButton = new LinboPushButton(startActionIconPath, this);
            actionButton->setToolTip(this->getTooltipContentForAction(startAction));

            actionButton->setEnabled(!disabled);
            actionButton->setVisible(false);
            this->startActionButtons.append(actionButton);

            switch (startAction) {
            case LinboOs::StartOs:
                connect(actionButton, &LinboPushButton::clicked, this->os, &LinboOs::start);
                break;
            case LinboOs::SyncOs:
                connect(actionButton, &LinboPushButton::clicked, this->os, &LinboOs::sync);
                break;
            case LinboOs::ReinstallOs:
                connect(actionButton, &LinboPushButton::clicked, this->os, &LinboOs::reinstall);
                break;
            default:
                break;
            }
        }

        // root action button
        LinboPushButton* actionButton = new LinboPushButton(gTheme->getIconPath(LinboGuiTheme::UploadLegacyIcon), this);
        actionButton->setGeometry(0,0,0,0);
        actionButton->setVisible(false);
        //% "Upload image of %1"
        actionButton->setToolTip(qtTrId("uploadImageOfOS").arg(this->os->getName()));
        connect(actionButton, &LinboPushButton::clicked, [=] {
            this->os->getBackend()->setCurrentOs(this->os);
            emit this->imageUploadRequested();
        });
        this->rootActionButtons.append(actionButton);

        // OS name label
        this->osNameLabel = new QLabel(this);
        if(this->os->getBaseImage() != nullptr) {
            this->osNameLabel->setText(this->os->getName());
            this->osNameLabel->setStyleSheet("QLabel {color: " + gTheme->getColor(LinboGuiTheme::TextColor).name() + "}");
        }
        else {
            //% "No baseimage defined"
            this->osNameLabel->setText(qtTrId("main_noBaseImage"));
            this->osNameLabel->setStyleSheet("QLabel {color: red}");
        }

        this->osNameLabel->setWordWrap(true);
        this->osNameLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    }
    else {
        connect(this->button, &LinboPushButton::hovered, [=] {this->button->setChecked(true);});
        connect(this->button, &LinboPushButton::doubleClicked, this, &LinboOsSelectButton::handlePrimaryButtonClicked);
    }

    this->button->setCheckable(true);
    this->buttonGroup->addButton(this->button);
    this->handleBackendStateChange(this->os->getBackend()->getState());

    QWidget::setVisible(true);
}

void LinboOsSelectButton::handlePrimaryButtonClicked() {
    if(this->os->getBackend()->getState() == LinboBackend::Idle)
        switch (this->os->getDefaultAction()) {
        case LinboOs::StartOs:
            this->os->start();
            break;
        case LinboOs::SyncOs:
            this->os->sync();
            break;
        case LinboOs::ReinstallOs:
            this->os->reinstall();
            break;
        default:
            break;
        }
    else if (this->os->getBackend()->getState() == LinboBackend::Root) {
        this->os->getBackend()->setCurrentOs(this->os);
        emit this->imageCreationRequested();
    }
}

LinboOs* LinboOsSelectButton::getOs() {
    return this->os;
}

void LinboOsSelectButton::setVisibleAnimated(bool visible) {
    this->shouldBeVisible = visible;
    this->button->setVisibleAnimated(visible);
    if(this->osNameLabel != nullptr)
        this->osNameLabel->setVisible(visible);

    this->updateActionButtonVisibility();
}

void LinboOsSelectButton::setVisible(bool visible) {
    this->shouldBeVisible = visible;
    this->button->setVisible(visible);
    if(this->osNameLabel != nullptr)
        this->osNameLabel->setVisible(visible);

    this->updateActionButtonVisibility(true);
}

void LinboOsSelectButton::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    if(!this->showActionButtons || this->os->getBackend()->getConfig()->getUseMinimalLayout()) {
        this->button->setGeometry((this->width() - this->height()) / 2, 0, this->height(), this->height());
    }
    else {
        this->button->setGeometry(0, 0, event->size().height(), event->size().height());

        int x = this->height();
        int spacing = this->width() * 0.04;
        int actionButtonSize = (this->width() - this->height()) / 4 - spacing;

        if(actionButtonSize <= 0) {
            // only the big button is visible
            actionButtonSize = 2;
            x = 0;
            spacing = 0;
        }

        for(LinboPushButton* actionButton : this->startActionButtons) {
            if(actionButtonSize == 2) {
                actionButton->setVisible(false);
                actionButton->setDisabled(true);
            }
            else {
                actionButton->setVisible(false);
                actionButton->setDisabled(false);
            }
            actionButton->setGeometry(x + spacing, this->height() - actionButtonSize, actionButtonSize, actionButtonSize);
            x += actionButtonSize + spacing;
        }

        x = this->height();

        for(LinboPushButton* actionButton : this->rootActionButtons) {
            if(actionButtonSize == 2) {
                actionButton->setVisible(false);
                actionButton->setDisabled(true);
            }
            else {
                actionButton->setVisible(false);
                actionButton->setDisabled(false);
            }

            actionButton->setGeometry(x + spacing, this->height() - actionButtonSize, actionButtonSize, actionButtonSize);
            x += actionButtonSize + spacing;
        }

        this->osNameLabel->setGeometry(this->height() + spacing, 0, this->width() - this->height() - spacing * 2, actionButtonSize);
        QFont fontCache = this->osNameLabel->font();
        fontCache.setPixelSize(actionButtonSize * 0.5);
        this->osNameLabel->setFont(fontCache);
    }

    this->updateActionButtonVisibility();
}


void LinboOsSelectButton::setShowActionButtons(bool showActionButtons) {
    if(this->showActionButtons == showActionButtons && this->inited)
        return;

    this->showActionButtons = showActionButtons;
    this->button->setCheckable(!showActionButtons);

    this->updateActionButtonVisibility();
}

void LinboOsSelectButton::handleBackendStateChange(LinboBackend::LinboState state) {
    this->showDefaultAction = false;

    bool checkedOverlayMuted = true;

    switch (state) {
    case LinboBackend::Idle:
        this->button->setToolTip(this->getTooltipContentForAction(this->os->getDefaultAction()));
        if(!this->os->getBackend()->getConfig()->getUseMinimalLayout())
            this->showDefaultAction = true;
        break;
    case LinboBackend::Root:
        if(!this->os->getBackend()->getConfig()->getUseMinimalLayout())
            this->showDefaultAction = true;

        //% "Create image of %1"
        this->button->setToolTip(qtTrId("createImageOfOS"));

        checkedOverlayMuted = false;
        break;
    case LinboBackend::Autostarting:
        this->showDefaultAction = true;
        break;
    default:
        break;
    }

    this->button->setOverlayTypeMuted(LinboPushButtonOverlay::OnChecked, checkedOverlayMuted);
    this->updateActionButtonVisibility();
}

QString LinboOsSelectButton::getTooltipContentForAction(LinboOs::LinboOsStartAction action) {
    QMap<LinboOs::LinboOsStartAction, QString> startActionButtonIcons = {
        //% "Start %1"
        {LinboOs::StartOs, qtTrId("startOS")},
        //% "Sync and start %1"
        {LinboOs::SyncOs, qtTrId("syncOS")},
        //% "Reinstall %1"
        {LinboOs::ReinstallOs, qtTrId("reinstallOS")}
    };

    return startActionButtonIcons[action].arg(this->os->getName());
}

void LinboOsSelectButton::updateActionButtonVisibility(bool doNotAnimate) {

    bool startActionVisible = this->shouldBeVisible && this->os->getBackend()->getState() < LinboBackend::Root;
    bool rootActionVisible = this->shouldBeVisible && this->os->getBackend()->getState() >= LinboBackend::Root;

    if(this->inited && !doNotAnimate) {
        this->defaultStartActionOverlay->setVisibleAnimated(startActionVisible && this->showDefaultAction);
        this->defaultRootActionOverlay->setVisibleAnimated(rootActionVisible && this->showDefaultAction);
    }
    else {
        this->defaultStartActionOverlay->setVisible(startActionVisible && this->showDefaultAction);
        this->defaultRootActionOverlay->setVisible(rootActionVisible && this->showDefaultAction);
    }

    if(this->os->getBackend()->getConfig()->getUseMinimalLayout())
        return;

    if(!this->showActionButtons) {
        for(LinboPushButton* actionButton : this->startActionButtons)
            actionButton->setVisible(false);

        for(LinboPushButton* actionButton : this->rootActionButtons)
            actionButton->setVisible(false);

        this->defaultStartActionOverlay->setVisible(false);
        this->defaultRootActionOverlay->setVisible(false);

        return;
    }

    for(LinboPushButton* actionButton : this->startActionButtons)
        if(this->inited && !doNotAnimate)
            actionButton->setVisibleAnimated(startActionVisible);
        else
            actionButton->setVisible(startActionVisible);

    for(LinboPushButton* actionButton : this->rootActionButtons)
        if(this->inited && !doNotAnimate)
            actionButton->setVisibleAnimated(rootActionVisible);
        else
            actionButton->setVisible(rootActionVisible);

    this->inited = true;
}

