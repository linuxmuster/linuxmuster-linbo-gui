/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020-2021  Dorian Zedler <dorian@itsblue.de>
 **
 ** Portions of the code in this file are based on code by:
 ** Copyright (C) 2007 Martin Oehler <oehler@knopper.net>
 ** Copyright (C) 2007 Klaus Knopper <knopper@knopper.net>
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
#ifndef LINBOCONFIG_H
#define LINBOCONFIG_H

#include <QObject>
#include <QColor>

#include "linboimage.h"
#include "linbodiskpartition.h"
#include "linboos.h"
#include "linbotheme.h"

class LinboConfig : public QObject
{
    Q_OBJECT
public:
    friend class LinboConfigReader;

    enum DownloadMethod {
        Rsync,
        Multicast,
        Torrent
    };

    enum LinboDeviceRole {
        ClassroomStudentComputerRole,
        ClassroomTeacherComputerRole,
        FacultyTeacherComputerRole,
        StaffComputerRole,
        DeviceRoleCount
    };

    const QString& serverIpAddress() const {
        return this->_serverIpAddress;
    }
    const QString& ipAddress() const {
        return this->_ipAddress;
    }
    const QString& subnetMask() const {
        return this->_subnetMask;
    }
    const QString& subnetBitmask() const {
        return this->_subnetBitmask;
    }
    const QString& macAddress() const {
        return this->_macAddress;
    }
    const QString& linboVersion() const {
        return this->_linboVersion;
    }
    const QString& hostname() const {
        return this->_hostname;
    }
    const QString& cpuModel() const {
        return this->_cpuModel;
    }
    const QString& ramSize() const {
        return this->_ramSize;
    }
    const QString& cacheSize() const {
        return this->_cacheSize;
    }
    const QString& diskSize() const {
        return this->_diskSize;
    }
    QString cachePath() const {
        return this->_cachePath;
    }
    const QString& hostGroup() const {
        return this->_hostGroup;
    }
    const unsigned int& rootTimeout() const {
        return this->_rootTimeout;
    }
    const bool& autoPartition() {
        return this->_autoPartition;
    }
    const bool& autoInitCache() {
        return this->_autoInitCache;
    }
    const DownloadMethod& downloadMethod() {
        return this->_downloadMethod;
    }
    const bool& autoFormat() {
        return this->_autoFormat;
    }
    const bool& clientDetailsVisibleByDefault() {
        return this->_clientDetailsVisibleByDefault;
    }
    const QString& locale() const {
        return this->_locale;
    }
    bool guiDisabled() {
        return this->_guiDisabled;
    }
    const QString& themeName() const {
        return this->_themeName;
    }

    QMap<QString, LinboImage*> images() {
        return this->_images;
    }
    QList<LinboOs*> operatingSystems() {
        return this->_operatingSystems;
    }
    QList<LinboDiskPartition*> diskPartitions() {
        return this->_diskPartitions;
    }
    LinboTheme* theme() {
        return this->_theme;
    }

    QList<LinboImage*> getImagesOfOs(LinboOs* os, bool includeImagesWithoutOs = true, bool includeNonExistantImages = true);
    LinboImage* getImageByName(QString name);

    static LinboConfig::DownloadMethod stringToDownloadMethod(const QString& value);
    static QString downloadMethodToString(const LinboConfig::DownloadMethod& value);
    static QString deviceRoleToString(const LinboConfig::LinboDeviceRole& deviceRole);

private:
    explicit LinboConfig(QObject *parent = nullptr);

    QMap<QString, LinboImage*> _images;
    QList<LinboOs*> _operatingSystems;
    QList<LinboDiskPartition*> _diskPartitions;
    LinboTheme* _theme;

    QString _serverIpAddress;
    QString _ipAddress;
    QString _subnetMask;
    QString _subnetBitmask;
    QString _macAddress;
    QString _linboVersion;
    QString _hostname;
    QString _cpuModel;
    QString _ramSize;
    QString _cacheSize;
    QString _diskSize;
    QString _cachePath;
    QString _hostGroup;
    QString _themeName;
    DownloadMethod _downloadMethod;
    QString _locale;
    unsigned int _rootTimeout;
    bool _autoPartition;
    bool _autoInitCache;
    bool _autoFormat;
    bool _guiDisabled;
    bool _clientDetailsVisibleByDefault;

signals:

};

#endif // LINBOCONFIG_H
