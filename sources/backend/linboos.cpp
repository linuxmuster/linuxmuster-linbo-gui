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

#include "linboos.h"
#include "linbobackend.h"

LinboOs::LinboOs(LinboBackend *parent) : QObject(parent)
{
    this->parent = parent;
    this->autostartEnabled = false;
    this->autostartTimeout = 0;
    this->hidden = false;
    this->defaultAction = SyncOs;
    this->iconName = QString("defaultIcon.svg");
    this->baseImage = nullptr;
}

bool LinboOs::start() {
    this->parent->setCurrentOs(this);
    return this->parent->startCurrentOs();
}

bool LinboOs::sync() {
    this->parent->setCurrentOs(this);
    return this->parent->syncCurrentOs();
}

bool LinboOs::reinstall() {
    this->parent->setCurrentOs(this);
    return this->parent->reinstallCurrentOs();
}

void LinboOs::setBaseImage (LinboImage* baseImage) {
    baseImage->setOs(this);
    this->baseImage = baseImage;
}

bool LinboOs::getActionEnabled(LinboOsStartAction action) {
    switch (action) {
    case StartOs: return this->getStartActionEnabled();
    case SyncOs: return this->getSyncActionEnabled();
    case ReinstallOs: return this->getReinstallActionEnabled();
    default: return false;
    }
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
