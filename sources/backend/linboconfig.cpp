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

#include "linboconfig.h"
#include <QtDebug>

LinboConfig::LinboConfig(QObject *parent) : QObject(parent)
{
    this->_autoPartition = 0;
    this->_autoInitCache = 0;
    this->_locale = "";
    this->_downloadMethod = LinboConfig::Rsync;
    this->_autoFormat = 0;
    this->_guiDisabled = false;
    this->_rootTimeout = 0;
    this->_autoPartition = false;
    this->_autoFormat = false;
    this->_operatingSystems = {};
    this->_themeName = "";
    this->_clientDetailsVisibleByDefault = false;
    this->_theme = new LinboTheme();
}

QList<LinboImage*> LinboConfig::getImagesOfOs(LinboOs* os, bool includeImagesWithoutOs, bool includeNonExistantImages) {
    QList<LinboImage*> filteredImages;
    QList<LinboImage*> imagesWithoutOs;

    for(LinboImage* image : qAsConst(this->_images))
        if(!image->existsOnDisk() && !includeNonExistantImages)
            continue;
        else if(image->os() == os)
            filteredImages.append(image);
        else if(includeImagesWithoutOs && !image->hasOs())
            imagesWithoutOs.append(image);

    filteredImages.append(imagesWithoutOs);

    return filteredImages;
}

LinboImage* LinboConfig::getImageByName(QString name) {
    if(this->_images.contains(name))
        return this->_images[name];
    else
        return nullptr;
}

LinboConfig::DownloadMethod LinboConfig::stringToDownloadMethod(const QString& value) {
    if(value.toLower() == "rsync")
        return LinboConfig::Rsync;
    else if(value.toLower() == "multicast")
        return LinboConfig::Multicast;
    else if(value.toLower() == "torrent")
        return LinboConfig::Torrent;
    else
        return LinboConfig::Rsync;
}

QString LinboConfig::downloadMethodToString(const LinboConfig::DownloadMethod& value) {
    switch (value) {
    case LinboConfig::Rsync:
        return "rsync";
    case LinboConfig::Multicast:
        return "multicast";
    case LinboConfig::Torrent:
        return "torrent";
    default:
        return "rsync";
    }
}


QString LinboConfig::deviceRoleToString(const LinboConfig::LinboDeviceRole& deviceRole) {
    switch (deviceRole) {
    case LinboConfig::ClassroomStudentComputerRole:
        return "classroom-studentcomputer";
    case LinboConfig::ClassroomTeacherComputerRole:
        return "classroom-teachercomputer";
    case LinboConfig::FacultyTeacherComputerRole:
        return "faculty-teachercomputer";
    case LinboConfig::StaffComputerRole:
        return "staffcomputer";
    default:
        return "";
    }
}
