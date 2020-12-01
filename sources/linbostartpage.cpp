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

#include "linbostartpage.h"

LinboStartPage::LinboStartPage(LinboBackend* backend, QWidget *parent) : QWidget(parent)
{
    this->inited = false;

    this->backend = backend;
    connect(this->backend, SIGNAL(stateChanged(LinboBackend::LinboState)), this, SLOT(handleLinboStateChanged(LinboBackend::LinboState)));

    this->setGeometry(QRect(0,0,parent->width(), parent->height()));

    // create an instance of the old GUI (as a backup)
    //linboGUIImpl* legacyGui = new linboGUIImpl(this);
    //legacyGui->setStyleSheet( "QDialog { background: white }");

    // create the main layout

    // main layout
    QWidget* mainLayoutWidget = new QWidget(this);
    mainLayoutWidget->setGeometry(this->geometry());
    QVBoxLayout* mainLayout = new QVBoxLayout(mainLayoutWidget);
    mainLayout->setSpacing(this->height()*0.025);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addItem(new QSpacerItem(0,0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // OS Buttons
    osSelectionRow = new LinboOsSelectionRow(this->backend);
    mainLayout->addWidget(osSelectionRow);
    osSelectionRow->setFixedHeight(this->height() * 0.25);
    osSelectionRow->setFixedWidth(this->width());

    // action buttons
    this->startActionsWidget = new LinboStartActions(this->backend, this->osSelectionRow);
    mainLayout->addWidget(this->startActionsWidget);
    this->startActionsWidget->setFixedHeight(this->height() * 0.45);
    this->startActionsWidget->setFixedWidth(this->width());

    QLabel* versionLabel = new QLabel(backend->getConfig()->getLinboVersion() );//+ " - GUI by Dorian Zedler");
    versionLabel->setFont(QFont("Segoe UI"));
    mainLayout->addWidget(versionLabel);


    // power and settings Buttons
    QWidget* powerActionsLayoutWidget = new QWidget(this);
    int height = this->height() * 0.2;
    int width = height / 3;
    int margins = width * 0.1;
    int buttonWidth = width * 0.6;
    powerActionsLayoutWidget->setGeometry(QRect(this->width() - (width + margins), this->height() - (height + margins), width * 1.1, height));

    QModernPushButton* settingsActionButton = new QModernPushButton(":/svgIcons/settingsAction.svg");
    //connect(settingsActionButton, SIGNAL(clicked()), legacyGui, SLOT(loginAndOpen()));
    this->powerActionButtons.append(settingsActionButton);
    settingsActionButton->setFixedHeight(buttonWidth);
    settingsActionButton->setFixedWidth(buttonWidth);

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
    powerActionsLayout->addWidget(settingsActionButton);
    powerActionsLayout->addWidget(rebootActionButton);
    powerActionsLayout->addWidget(shutdownActionButton);

    this->handleLinboStateChanged(this->backend->getState());
}

void LinboStartPage::handleLinboStateChanged(LinboBackend::LinboState newState) {
    bool powerActionButtonsVisible = true;
    switch (newState) {
    case LinboBackend::Autostarting:
    case LinboBackend::Starting:
    case LinboBackend::Syncing:
    case LinboBackend::Reinstalling:
        powerActionButtonsVisible = false;
        break;

    default:
        break;
    }

    for(QModernPushButton* powerActionButton : this->powerActionButtons)
        if(this->inited)
            powerActionButton->setVisibleAnimated(powerActionButtonsVisible);
        else
            powerActionButton->setVisible(powerActionButtonsVisible);

    this->inited = true;
}
