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

#include "linboosselectbutton.h"

LinboOsSelectButton::LinboOsSelectButton(QString icon, LinboOs* os, QButtonGroup* buttonGroup, QWidget* parent) : QWidget(parent)
{
    this->showActionButtons = false;
    this->inited = false;
    this->buttonGroup = buttonGroup;
    this->os = os;
    this->shouldBeVisible = true;
    this->showDefaultAction = true;

    connect(os->getBackend(), &LinboBackend::stateChanged, this, &LinboOsSelectButton::handleBackendStateChange);


    QString iconType = os->getBackend()->getConfig()->isBackgroundColorDark() ? "light":"dark";

    QMap<LinboOs::LinboOsStartAction, QString> defaultStartActionOverlayPaths = {
        {LinboOs::StartOs, ":/icons/universal/overlayStart.svg"},
        {LinboOs::SyncOs, ":/icons/universal/overlaySync.svg"},
        {LinboOs::ReinstallOs, ":/icons/universal/overlayReinstall.svg"},
        {LinboOs::UnknownAction, ""}
    };

    this->defaultStartActionOverlay = new QModernPushButtonOverlay (
                QModernPushButtonOverlay::Background,
                new QSvgWidget(defaultStartActionOverlayPaths[this->os->getDefaultAction()]),
            false
            );

    this->defaultRootActionOverlay = new QModernPushButtonOverlay (
                QModernPushButtonOverlay::Background,
                new QSvgWidget(":/icons/universal/overlayImage.svg"),
                false
                );

    this->button = new QModernPushButton(icon, "", {this->defaultStartActionOverlay, this->defaultRootActionOverlay}, this);
    this->setToolTip(this->os->getDescription());

    if(!os->getBackend()->getConfig()->getUseMinimalLayout()) {
        connect(this->button, &QModernPushButton::clicked, this, &LinboOsSelectButton::handlePrimaryButtonClicked);

        QMap<LinboOs::LinboOsStartAction, QString> startActionButtonIcons = {
            {LinboOs::StartOs, ":/icons/universal/startBg.svg"},
            {LinboOs::SyncOs, ":/icons/universal/syncBg.svg"},
            {LinboOs::ReinstallOs, ":/icons/universal/reinstallBg.svg"}
        };

        for(LinboOs::LinboOsStartAction startAction : startActionButtonIcons.keys()) {
            QString startActionIconPath = startActionButtonIcons[startAction];
            bool disabled = !this->os->getActionEnabled(startAction) || this->os->getDefaultAction() == startAction;

            if(disabled)
                continue;

            QModernPushButton* actionButton = new QModernPushButton(startActionIconPath, this);
            actionButton->setEnabled(!disabled);
            actionButton->setVisible(false);
            this->startActionButtons.append(actionButton);

            switch (startAction) {
            case LinboOs::StartOs: connect(actionButton, &QModernPushButton::clicked, this->os, &LinboOs::start); break;
            case LinboOs::SyncOs: connect(actionButton, &QModernPushButton::clicked, this->os, &LinboOs::sync); break;
            case LinboOs::ReinstallOs: connect(actionButton, &QModernPushButton::clicked, this->os, &LinboOs::reinstall); break;
            default: break;
            }
        }


        // root action button
        QModernPushButton* actionButton = new QModernPushButton(":/icons/universal/uploadBg.svg", this);
        actionButton->setGeometry(0,0,0,0);
        actionButton->setVisible(false);
        connect(actionButton, &QModernPushButton::clicked, this, &LinboOsSelectButton::imageUploadRequested);
        this->rootActionButtons.append(actionButton);
    }
    else {
        connect(this->button, &QModernPushButton::hovered, [=]{this->button->setChecked(true);});
        connect(this->button, &QModernPushButton::doubleClicked, this, &LinboOsSelectButton::handlePrimaryButtonClicked);
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
    else if (this->os->getBackend()->getState() == LinboBackend::Root){
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

    this->updateActionButtonVisibility();
}

void LinboOsSelectButton::setVisible(bool visible) {
    this->shouldBeVisible = visible;
    this->button->setVisible(visible);

    this->updateActionButtonVisibility(true);
}

void LinboOsSelectButton::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    if(!this->showActionButtons || this->os->getBackend()->getConfig()->getUseMinimalLayout()) {
        this->button->setGeometry((this->width() - this->height()) / 2 , 0, this->height(), this->height());
    }
    else {
        this->button->setGeometry(0, 0, event->size().height(), event->size().height());

        //qDebug() << "OS " << this->os->getName() << " Width: " << this->width() << " Height: " << this->height();

        int x = this->height();
        int spacing = this->width() * 0.04;
        int actionButtonSize = (this->width() - this->height()) / 4 - spacing;

        if(actionButtonSize < 0) {
            // only the big button is visible
            actionButtonSize = 0;
            x = 0;
            spacing = 0;
        }

        //qDebug() << "OS " << this->os->getName() << " X: " << x << " spacing: " << spacing << " actionButtonSize: " << actionButtonSize << " Button length: ";

        for(QModernPushButton* actionButton : this->startActionButtons) {
            actionButton->setGeometry(x + spacing, this->height() - actionButtonSize, actionButtonSize, actionButtonSize);
            //qDebug() << "Setting geometry to: " << actionButton->geometry();
            x += actionButtonSize + spacing;
        }

        x = this->height();

        for(QModernPushButton* actionButton : this->rootActionButtons) {
            actionButton->setGeometry(x + spacing, this->height() - actionButtonSize, actionButtonSize, actionButtonSize);
            x += actionButtonSize + spacing;
        }
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
    case LinboBackend::Root:
        if(!this->os->getBackend()->getConfig()->getUseMinimalLayout())
            this->showDefaultAction = true;

        checkedOverlayMuted = false;
        break;
    case LinboBackend::Autostarting:
        this->showDefaultAction = true;
        break;
    default:
        break;
    }

    this->button->setOverlayTypeMuted(QModernPushButtonOverlay::OnChecked, checkedOverlayMuted);
    this->updateActionButtonVisibility();
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
        for(QModernPushButton* actionButton : this->startActionButtons)
            actionButton->setVisible(false);

        for(QModernPushButton* actionButton : this->rootActionButtons)
            actionButton->setVisible(false);

        this->defaultStartActionOverlay->setVisible(false);
        this->defaultRootActionOverlay->setVisible(false);

        return;
    }

    for(QModernPushButton* actionButton : this->startActionButtons)
        if(this->inited && !doNotAnimate)
            actionButton->setVisibleAnimated(startActionVisible);
        else
            actionButton->setVisible(startActionVisible);

    for(QModernPushButton* actionButton : this->rootActionButtons)
        if(this->inited && !doNotAnimate)
            actionButton->setVisibleAnimated(rootActionVisible);
        else
            actionButton->setVisible(rootActionVisible);

    this->inited = true;
}

