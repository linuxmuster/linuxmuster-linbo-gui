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

#include "linboosselectionrow.h"

LinboOsSelectionRow::LinboOsSelectionRow(LinboBackend* backend, QWidget *parent) : QWidget(parent)
{
    this->inited = false;
    this->sizeOverride = nullptr;

    this->backend = backend;
    connect(this->backend, SIGNAL(stateChanged(LinboBackend::LinboState)), this, SLOT(handleLinboStateChanged(LinboBackend::LinboState)));

    this->showOnlySelectedButton = false;

    this->sizeAnimation = new QPropertyAnimation(this, "minimumSize", this);
    this->sizeAnimation->setDuration(100);
    this->sizeAnimation->setEasingCurve(QEasingCurve::OutQuad);

    this->osButtonGroup = new QButtonGroup();
    this->osButtonGroup->setExclusive(true);

    for(LinboOs* os : backend->getOperatingSystems()) {
        if(this->osButtons.length() >= 4)
            break;

#ifdef TEST_ENV
        LinboOsSelectButton* osButton = new LinboOsSelectButton(TEST_ENV"/icons/" + os->getIconName(), os, this->osButtonGroup, this);
#else
        LinboOsSelectButton* osButton = new LinboOsSelectButton("/icons/" + os->getIconName(), os, this->osButtonGroup, this);
#endif
        connect(osButton->button, SIGNAL(toggled(bool)), this, SLOT(handleButtonToggled(bool)));
        connect(osButton, &LinboOsSelectButton::imageCreationRequested, this, &LinboOsSelectionRow::imageCreationRequested);
        connect(osButton, &LinboOsSelectButton::imageUploadRequested, this, &LinboOsSelectionRow::imageUploadRequested);

        osButton->setShowActionButtons(!this->backend->getConfig()->getUseMinimalLayout());

        // auto select current OS
        if(this->backend->getCurrentOs() == os)
            osButton->button->setChecked(true);

        this->osButtons.append(osButton);
    }

    if(this->osButtons.length() == 0) {
        //% "No Operating system configured in start.conf"
        this->noOsLabel = new QLabel(qtTrId("osSelection_noOperatingSystems"), this);
        this->noOsLabel->hide();
        this->noOsLabel->setAlignment(Qt::AlignCenter);
        this->noOsLabelFont.setBold(true);
        this->noOsLabel->setFont(this->noOsLabelFont);
        this->noOsLabel->setStyleSheet("QLabel { color: " + gTheme->getColor(LinboGuiTheme::TextColor).name() + " }");

        QString environmentValuesText;
        //% "Hostname"
        environmentValuesText += qtTrId("hostname") + ":  " + this->backend->getConfig()->getHostname() + "\n";
        //% "IP-Address"
        environmentValuesText += qtTrId("ip") + ":  " + this->backend->getConfig()->getIpAddress() + "\n";
        //% "Mac"
        environmentValuesText += qtTrId("client_info_mac") + ":  " + this->backend->getConfig()->getMacAddress() + "\n";

        this->environmentValuesLabel = new QLabel(environmentValuesText, this);
        this->environmentValuesLabel->hide();
        this->environmentValuesLabel->setAlignment(Qt::AlignCenter);
        this->environmentValuesLabel->setFont(this->environmentValuesLabelFont);
        this->environmentValuesLabel->setStyleSheet("QLabel { color: " + gTheme->getColor(LinboGuiTheme::TextColor).name() + " }");
    }

    this->handleLinboStateChanged(this->backend->getState());
}

void LinboOsSelectionRow::resizeAndPositionAllButtons(int heightOverride, int widthOverride) {

    heightOverride = this->height();
    widthOverride = this->width();

    if(this->sizeOverride != nullptr)
        heightOverride = this->sizeOverride->height();

    if(this->sizeOverride != nullptr)
        widthOverride = this->sizeOverride->width();

    if(this->osButtons.length() > 0) {

        bool useMinimalLayout = this->backend->getConfig()->getUseMinimalLayout();
        int buttonCount = this->osButtons.length();

        int spacing;
        int buttonWidth;
        int buttonHeight;
        int totalWidth;

        if(useMinimalLayout) {
            spacing = heightOverride * 0.2;
            buttonWidth = std::min(int(((widthOverride - spacing) / buttonCount) * 0.9), heightOverride * 3);
            buttonHeight = heightOverride;
            totalWidth = buttonWidth * buttonCount + spacing * (buttonCount + 1);
        }
        else if (!useMinimalLayout && buttonCount <= 2) {
            spacing = heightOverride * 0.1;
            buttonWidth = std::min((widthOverride - spacing) / buttonCount, int(heightOverride * 1.5) - spacing);
            buttonHeight = heightOverride / 2 - spacing / 2;
            totalWidth = buttonWidth * buttonCount + spacing * (buttonCount + 1);
        }
        else if(!useMinimalLayout && buttonCount > 2) {
            // if we have more than two buttons -> we have multiple rows with two buttons each
            spacing = heightOverride * 0.1;
            buttonWidth = std::min((widthOverride - spacing) / 2, int(heightOverride * 1.5) - spacing);
            buttonHeight = heightOverride / 2 - spacing / 2;
            totalWidth = buttonWidth * 2 + spacing * (2 + 1);
        }
        else {
            qWarning() << "[ERROR] More than 4 OSs defined";
            return;
        }

        int x = (widthOverride - totalWidth) / 2;

        for(int i = 0; i < this->osButtons.length(); i++) {

            bool visible = true;
            QRect geometry = this->osButtons[i]->geometry();

            if(this->osButtons[i]->getOs() != this->backend->getCurrentOs() || !this->showOnlySelectedButton) {
                // "normal" buttons
                visible = !this->showOnlySelectedButton;
                if(!useMinimalLayout && buttonCount > 2)
                    if(i < 2)
                        geometry = QRect(x + (buttonWidth  * i) + (spacing * (i+1)), 0, buttonWidth, buttonHeight);
                    else
                        geometry = QRect(x + (buttonWidth * (i-2)) + (spacing * ((i-2)+1)), buttonHeight + spacing, buttonWidth, buttonHeight);
                else
                    geometry = QRect(x + (buttonWidth * i) + (spacing * (i+1)), (heightOverride - buttonHeight) / 2, buttonWidth, buttonHeight);
            }
            else {
                // singular button
                visible = true;
                geometry = QRect((widthOverride - heightOverride) / 2, 0, heightOverride, heightOverride);
            }

            if(this->inited) {
                this->osButtons[i]->setVisibleAnimated(visible);

                QPropertyAnimation* moveAnimation = new QPropertyAnimation(this);
                moveAnimation->setPropertyName("geometry");
                moveAnimation->setEasingCurve(QEasingCurve::InOutQuad);
                moveAnimation->setDuration(300);
                moveAnimation->setTargetObject(this->osButtons[i]);
                moveAnimation->setStartValue(this->osButtons[i]->geometry());
                moveAnimation->setEndValue(geometry);
                moveAnimation->start();
                connect(moveAnimation, &QPropertyAnimation::finished, moveAnimation, &QPropertyAnimation::deleteLater);

                continue;
            }
            else {
                // Do not animate the first time
                this->osButtons[i]->setVisible(visible);
                this->osButtons[i]->setGeometry(geometry);
            }
        }
    }
    else {
        int infoLabelHeight = heightOverride;
        int infoLabelWidth = widthOverride * 0.8;
        int noOsLabelHeight = heightOverride * 0.15;
        this->noOsLabelFont.setPixelSize(gTheme->toFontSize(noOsLabelHeight * 0.8));
        this->noOsLabel->setFont(this->noOsLabelFont);
        this->noOsLabel->setGeometry((widthOverride - infoLabelWidth) / 2, 0, infoLabelWidth, noOsLabelHeight);
        this->noOsLabel->show();

        this->environmentValuesLabelFont.setPixelSize(gTheme->toFontSize(infoLabelHeight * 0.1));
        this->environmentValuesLabel->setFont(this->environmentValuesLabelFont);
        this->environmentValuesLabel->setGeometry((widthOverride - infoLabelWidth) / 2, noOsLabelHeight, infoLabelWidth, infoLabelHeight);
        this->environmentValuesLabel->show();
    }

    this->inited = true;
}

void LinboOsSelectionRow::handleButtonToggled(bool checked) {
    if(checked)
        this->backend->setCurrentOs(this->getSelectedOs());
}

LinboOs* LinboOsSelectionRow::getSelectedOs() {
    for(LinboOsSelectButton* button : this->osButtons) {
        if(button->button->isChecked())
            return button->getOs();
    }
    return nullptr;
}

void LinboOsSelectionRow::setShowOnlySelectedButton(bool value) {
    // find selected button
    // set its x so it is in the middle (animated)
    // set Opacity of all other buttons to 0 (animated)
    if(value == this->showOnlySelectedButton)
        return;

    this->showOnlySelectedButton = value;

    if(this->inited)
        this->resizeAndPositionAllButtons();
}

void LinboOsSelectionRow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    //qDebug() << "RESIZE EVENT: width: " << width() << " height: " << height();
    this->resizeAndPositionAllButtons();
}

void LinboOsSelectionRow::handleLinboStateChanged(LinboBackend::LinboState newState) {

    switch (newState) {
    case LinboBackend::Idle:
    case LinboBackend::Root:
        for(LinboOsSelectButton* osButton : this->osButtons)
            osButton->setEnabled(true);
        this->setShowOnlySelectedButton(false);
        break;

    case LinboBackend::Autostarting:
    case LinboBackend::Starting:
    case LinboBackend::Syncing:
    case LinboBackend::Reinstalling:
    case LinboBackend::CreatingImage:
    case LinboBackend::UploadingImage:
        for(LinboOsSelectButton* osButton : this->osButtons)
            osButton->setEnabled(false);
        this->setShowOnlySelectedButton(true);
        break;

    case LinboBackend::Partitioning:
    case LinboBackend::UpdatingCache:
    case LinboBackend::RootActionSuccess:
        for(LinboOsSelectButton* osButton : this->osButtons)
            osButton->setEnabled(false);
        break;

    default:
        break;
    }
}

void LinboOsSelectionRow::setMinimumSizeAnimated(QSize size) {
    if(size.height() < this->height()) {
        this->sizeOverride = new QSize(size);
        this->resizeAndPositionAllButtons();
        this->sizeAnimation->setStartValue(this->size());
        this->sizeAnimation->setEndValue(size);
        connect(this->sizeAnimation, &QPropertyAnimation::finished, [=] {this->setMinimumSize(size); delete this->sizeOverride; this->sizeOverride = nullptr;});
        QTimer::singleShot(300, [=] {this->sizeAnimation->start();});
    }
    else {
        if(this->sizeOverride != nullptr) {
            delete this->sizeOverride;
            this->sizeOverride = nullptr;
        }

        delete this->sizeOverride;
        this->setFixedSize(size);
    }
}
