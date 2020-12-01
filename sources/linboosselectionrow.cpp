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

#include "linboosselectionrow.h"

LinboOsSelectionRow::LinboOsSelectionRow(LinboBackend* backend, QWidget *parent) : QWidget(parent)
{
    this->inited = false;

    this->backend = backend;
    connect(this->backend, SIGNAL(stateChanged(LinboBackend::LinboState)), this, SLOT(handleLinboStateChanged(LinboBackend::LinboState)));

    this->showOnlySelectedButton = false;

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

        // auto select current OS
        if(this->backend->getCurrentOs() == os)
            osButton->button->setChecked(true);

        this->osButtons.append(osButton);
    }

    if(this->osButtons.length() == 0) {
        this->noOsLabel = new QLabel(tr("No Operating system configured in start.conf"), this);
        this->noOsLabel->hide();
        this->noOsLabel->setAlignment(Qt::AlignCenter);
        this->noOsLabelFont = QFont("Segoe UI");
        this->noOsLabelFont.setBold(true);
        this->noOsLabel->setFont(this->noOsLabelFont);

        QString environmentValuesText;
        environmentValuesText += tr("Host") + ":  " + this->backend->getConfig()->getHostname() + "\n";
        //noOsText += tr("Group") + ": " + this->backend->getConfig()->getHostGroup() + "\n";
        environmentValuesText += tr("IP-Address") + ":  " + this->backend->getConfig()->getIpAddress() + "\n";
        //noOsText += tr("Server IP-Address") + ": " + this->backend->getConfig()->getServerIpAddress() + "\n";
        environmentValuesText += tr("Mac-Address") + ":  " + this->backend->getConfig()->getMacAddress() + "\n";

        this->environmentValuesLabel = new QLabel(environmentValuesText, this);
        this->environmentValuesLabel->hide();
        this->environmentValuesLabel->setAlignment(Qt::AlignCenter);
        this->environmentValuesLabelFont = QFont("Segoe UI");
        this->environmentValuesLabel->setFont(this->environmentValuesLabelFont);
    }

    this->handleLinboStateChanged(this->backend->getState());
}

void LinboOsSelectionRow::resizeAndPositionAllButtons() {

    if(this->osButtons.length() > 0) {
        int buttonWidth = 0;
        buttonWidth = this->width() / this->osButtonGroup->buttons().length();

        for(int i = 0; i < this->osButtons.length(); i++) {

        bool visible = true;
        QRect geometry = this->osButtons[i]->geometry();

        if(!this->osButtons[i]->button->isChecked() || !this->showOnlySelectedButton) {
            visible = !this->showOnlySelectedButton;
            geometry = QRect(buttonWidth * i, 0, buttonWidth, this->height());
        }
        else {
            visible = true;
            geometry = QRect((this->width() - buttonWidth) / 2, 0, buttonWidth, this->height());
        }

        if(this->inited) {
            this->osButtons[i]->setVisibleAnimated(visible);

            QPropertyAnimation* moveAnimation = new QPropertyAnimation(this->osButtons[i], "geometry");
            moveAnimation->setEasingCurve(QEasingCurve::InOutQuad);
            moveAnimation->setDuration(400);
            moveAnimation->setStartValue(this->osButtons[i]->geometry());
            moveAnimation->setEndValue(geometry);
            moveAnimation->start();
        }
        else {
            // Do not animate the first time
            this->osButtons[i]->setVisible(visible);
            this->osButtons[i]->setGeometry(geometry);
        }
    }
    }
    else {
        int infoLabelHeight = this->height();
        int infoLabelWidth = this->width() * 0.8;
        int noOsLabelHeight = this->height() * 0.2;
        this->noOsLabelFont.setPixelSize(noOsLabelHeight * 0.8);
        this->noOsLabel->setFont(this->noOsLabelFont);
        this->noOsLabel->setGeometry((this->width() - infoLabelWidth) / 2, 0, infoLabelWidth, noOsLabelHeight);
        this->noOsLabel->show();

        this->environmentValuesLabelFont.setPixelSize(infoLabelHeight * 0.1);
        this->environmentValuesLabel->setFont(this->environmentValuesLabelFont);
        this->environmentValuesLabel->setGeometry((this->width() - infoLabelWidth) / 2, noOsLabelHeight, infoLabelWidth, infoLabelHeight);
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
        this->setShowOnlySelectedButton(false);
        break;

    case LinboBackend::Autostarting:
    case LinboBackend::Starting:
    case LinboBackend::Syncing:
    case LinboBackend::Reinstalling:
        this->setShowOnlySelectedButton(true);
        break;

    default:
        break;
    }
}
