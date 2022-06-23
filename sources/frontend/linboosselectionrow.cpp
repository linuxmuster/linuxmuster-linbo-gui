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
    this->_inited = false;
    this->_sizeOverride = nullptr;

    this->_backend = backend;
    connect(this->_backend, &LinboBackend::stateChanged, this, &LinboOsSelectionRow::_handleLinboStateChanged);

    this->_showOnlySelectedButton = false;

    this->_sizeAnimation = new QPropertyAnimation(this, "minimumSize", this);
    this->_sizeAnimation->setDuration(100);
    this->_sizeAnimation->setEasingCurve(QEasingCurve::OutQuad);

    this->_osButtonGroup = new QButtonGroup();
    this->_osButtonGroup->setExclusive(true);

    for(LinboOs* os : backend->config()->operatingSystems()) {
        if(this->_osButtons.length() >= 4)
            break;

#ifdef TEST_ENV
        LinboOsSelectButton* osButton = new LinboOsSelectButton(TEST_ENV"/icons/" + os->iconName(), os, backend, this->_osButtonGroup, this);
#else
        LinboOsSelectButton* osButton = new LinboOsSelectButton("/icons/" + os->iconName(), os, backend, this->_osButtonGroup, this);
#endif
        connect(osButton->_button, &LinboPushButton::toggled, this, &LinboOsSelectionRow::_handleButtonToggled);
        connect(osButton, &LinboOsSelectButton::imageCreationRequested, this, &LinboOsSelectionRow::imageCreationRequested);
        connect(osButton, &LinboOsSelectButton::imageUploadRequested, this, &LinboOsSelectionRow::imageUploadRequested);

        this->_osButtons.append(osButton);
    }

    if(this->_osButtons.length() == 0) {
        //% "No Operating system configured in start.conf"
        this->_noOsLabel = new QLabel(qtTrId("osSelection_noOperatingSystems"), this);
        this->_noOsLabel->hide();
        this->_noOsLabel->setAlignment(Qt::AlignCenter);
        this->_noOsLabelFont.setBold(true);
        this->_noOsLabel->setFont(this->_noOsLabelFont);
        this->_noOsLabel->setStyleSheet(gTheme->insertValues("QLabel { color: %TextColor; }"));

        QString environmentValuesText;
        //% "Hostname"
        environmentValuesText += qtTrId("hostname") + ":  " + this->_backend->config()->hostname() + "\n";
        //% "IP-Address"
        environmentValuesText += qtTrId("ip") + ":  " + this->_backend->config()->ipAddress() + "\n";
        //% "Mac"
        environmentValuesText += qtTrId("client_info_mac") + ":  " + this->_backend->config()->macAddress() + "\n";

        this->_environmentValuesLabel = new QLabel(environmentValuesText, this);
        this->_environmentValuesLabel->hide();
        this->_environmentValuesLabel->setAlignment(Qt::AlignCenter);
        this->_environmentValuesLabel->setFont(this->_environmentValuesLabelFont);
        this->_environmentValuesLabel->setStyleSheet(gTheme->insertValues("QLabel { color: %TextColor; }"));
    }
    else {
        this->_osButtons[0]->_button->setChecked(true);
    }

    this->_handleLinboStateChanged(this->_backend->state());
}

void LinboOsSelectionRow::_resizeAndPositionAllButtons(int heightOverride, int widthOverride) {

    heightOverride = this->height();
    widthOverride = this->width();

    if(this->_sizeOverride != nullptr)
        heightOverride = this->_sizeOverride->height();

    if(this->_sizeOverride != nullptr)
        widthOverride = this->_sizeOverride->width();

    if(this->_osButtons.length() > 0) {

        int buttonCount = this->_osButtons.length();

        int spacing;
        int buttonWidth;
        int buttonHeight;
        int totalWidth;

        if (buttonCount <= 2) {
            spacing = heightOverride * 0.1;
            buttonWidth = std::min((widthOverride - spacing) / buttonCount, int(heightOverride * 1.5) - spacing);
            buttonHeight = heightOverride / 2 - spacing / 2;
            totalWidth = buttonWidth * buttonCount + spacing * (buttonCount + 1);
        }
        else if(buttonCount > 2) {
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

        for(int i = 0; i < this->_osButtons.length(); i++) {

            bool visible = true;
            QRect geometry = this->_osButtons[i]->geometry();

            if(this->_osButtons[i] != this->_selectedButton || !this->_showOnlySelectedButton) {
                // "normal" buttons
                visible = !this->_showOnlySelectedButton;
                if(buttonCount > 2)
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

            if(this->_inited) {
                this->_osButtons[i]->_setVisibleAnimated(visible);

                QPropertyAnimation* moveAnimation = new QPropertyAnimation(this);
                moveAnimation->setPropertyName("geometry");
                moveAnimation->setEasingCurve(QEasingCurve::InOutQuad);
                moveAnimation->setDuration(300);
                moveAnimation->setTargetObject(this->_osButtons[i]);
                moveAnimation->setStartValue(this->_osButtons[i]->geometry());
                moveAnimation->setEndValue(geometry);
                moveAnimation->start();
                connect(moveAnimation, &QPropertyAnimation::finished, moveAnimation, &QPropertyAnimation::deleteLater);

                continue;
            }
            else {
                // Do not animate the first time
                this->_osButtons[i]->setVisible(visible);
                this->_osButtons[i]->setGeometry(geometry);
            }
        }
    }
    else {
        int infoLabelHeight = heightOverride;
        int infoLabelWidth = widthOverride * 0.8;
        int noOsLabelHeight = heightOverride * 0.15;
        this->_noOsLabelFont.setPixelSize(gTheme->toFontSize(noOsLabelHeight * 0.8));
        this->_noOsLabel->setFont(this->_noOsLabelFont);
        this->_noOsLabel->setGeometry((widthOverride - infoLabelWidth) / 2, 0, infoLabelWidth, noOsLabelHeight);
        this->_noOsLabel->show();

        this->_environmentValuesLabelFont.setPixelSize(gTheme->toFontSize(infoLabelHeight * 0.1));
        this->_environmentValuesLabel->setFont(this->_environmentValuesLabelFont);
        this->_environmentValuesLabel->setGeometry((widthOverride - infoLabelWidth) / 2, noOsLabelHeight, infoLabelWidth, infoLabelHeight);
        this->_environmentValuesLabel->show();
    }

    this->_inited = true;
}

void LinboOsSelectionRow::_handleButtonToggled(bool checked) {
    if(checked)
        this->_selectedButton = this->_getSelectedButton();
}

LinboOs* LinboOsSelectionRow::getSelectedOs() {
    LinboOsSelectButton* selectedButton = this->_getSelectedButton();
    if(selectedButton != nullptr)
        return selectedButton->_getOs();
    return nullptr;
}

LinboOsSelectButton* LinboOsSelectionRow::_getSelectedButton() {
    for(LinboOsSelectButton* button : this->_osButtons) {
        if(button->_button->isChecked())
            return button;
    }
    return nullptr;
}

void LinboOsSelectionRow::setShowOnlySelectedButton(bool value) {
    // find selected button
    // set its x so it is in the middle (animated)
    // set Opacity of all other buttons to 0 (animated)
    if(value == this->_showOnlySelectedButton)
        return;

    this->_showOnlySelectedButton = value;

    if(this->_inited)
        this->_resizeAndPositionAllButtons();
}

void LinboOsSelectionRow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    //qDebug() << "RESIZE EVENT: width: " << width() << " height: " << height();
    this->_resizeAndPositionAllButtons();
}

void LinboOsSelectionRow::_handleLinboStateChanged(LinboBackend::LinboState newState) {

    switch (newState) {
    case LinboBackend::Idle:
    case LinboBackend::Root:
        for(LinboOsSelectButton* osButton : this->_osButtons)
            osButton->setEnabled(true);
        this->setShowOnlySelectedButton(false);
        break;

    case LinboBackend::Autostarting:
    case LinboBackend::Starting:
    case LinboBackend::Syncing:
    case LinboBackend::Reinstalling:
    case LinboBackend::CreatingImage:
    case LinboBackend::UploadingImage:
        for(LinboOsSelectButton* osButton : this->_osButtons)
            osButton->setEnabled(false);
        this->setShowOnlySelectedButton(true);
        break;

    case LinboBackend::Partitioning:
    case LinboBackend::UpdatingCache:
    case LinboBackend::RootActionSuccess:
        for(LinboOsSelectButton* osButton : this->_osButtons)
            osButton->setEnabled(false);
        break;

    default:
        break;
    }
}

void LinboOsSelectionRow::setMinimumSizeAnimated(QSize size) {
    if(size.height() < this->height()) {
        this->_sizeOverride = new QSize(size);
        this->_resizeAndPositionAllButtons();
        this->_sizeAnimation->setStartValue(this->size());
        this->_sizeAnimation->setEndValue(size);
        connect(this->_sizeAnimation, &QPropertyAnimation::finished, this, [=] {this->setMinimumSize(size); delete this->_sizeOverride; this->_sizeOverride = nullptr;});
        QTimer::singleShot(300, this, [=] {this->_sizeAnimation->start();});
    }
    else {
        if(this->_sizeOverride != nullptr) {
            delete this->_sizeOverride;
            this->_sizeOverride = nullptr;
        }

        delete this->_sizeOverride;
        this->setFixedSize(size);
    }
}
