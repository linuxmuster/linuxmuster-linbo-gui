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

#include "linboos.h"
#include "linbobackend.h"

LinboOs::LinboOs(LinboBackend *parent) : QObject(parent)
{
    this->_backend = parent;
    this->_autostartEnabled = false;
    this->_autostartTimeout = 0;
    this->_hidden = false;
    this->_defaultAction = UnknownAction;
    this->_iconName = QString("defaultIcon.svg");
    this->_baseImage = nullptr;
}

bool LinboOs::executeStart() {
    return this->_backend->startOs(this);
}

bool LinboOs::executeSync() {
    return this->_backend->syncOs(this);
}

bool LinboOs::executeReinstall() {
    return this->_backend->reinstallOs(this);
}

bool LinboOs::executeDefaultAction() {
    switch (this->defaultAction()) {
    case LinboOs::StartOs:
        return this->executeStart();
    case LinboOs::SyncOs:
        return this->executeSync();
    case LinboOs::ReinstallOs:
        return this->executeReinstall();
    default:
        return false;
    }
}

bool LinboOs::createBaseImage(QString description, LinboPostProcessActions::Flags postProcessActions) {
    return this->_backend->createBaseImageOfOs(this, description, postProcessActions);
}

bool LinboOs::createDiffImage(QString description, LinboPostProcessActions::Flags postProcessActions) {
    return this->_backend->createDiffImageOfOs(this, description, postProcessActions);
}

void LinboOs::_setBaseImage (LinboImage* baseImage) {
    baseImage->_os = this;
    this->_baseImage = baseImage;
}

bool LinboOs::actionEnabled(LinboOsStartAction action) {
    switch (action) {
    case StartOs:
        return this->startActionEnabled();
    case SyncOs:
        return this->syncActionEnabled();
    case ReinstallOs:
        return this->reinstallActionEnabled();
    default:
        return false;
    }
}


LinboOs::LinboOsStartAction LinboOs::defaultAction() {
    if(this->_baseImage != nullptr)
        return this->_defaultAction;
    else
        return StartOs;
}

LinboOs::LinboOsStartAction LinboOs::startActionFromString(const QString& name) {
    if(name == "start")
        return StartOs;
    else if(name == "sync")
        return SyncOs;
    else if(name == "new")
        return ReinstallOs;
    return UnknownAction;
}
