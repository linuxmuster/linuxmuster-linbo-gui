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

    const QString& getServerIpAddress() const {
        return this->serverIpAddress;
    }
    const QString& getIpAddress() const {
        return this->ipAddress;
    }
    const QString& getSubnetMask() const {
        return this->subnetMask;
    }
    const QString& getSubnetBitmask() const {
        return this->subnetBitmask;
    }
    const QString& getMacAddress() const {
        return this->macAddress;
    }
    const QString& getLinboVersion() const {
        return this->linboVersion;
    }
    const QString& getHostname() const {
        return this->hostname;
    }
    const QString& getCpuModel() const {
        return this->cpuModel;
    }
    const QString& getRamSize() const {
        return this->ramSize;
    }
    const QString& getCacheSize() const {
        return this->cacheSize;
    }
    const QString& getHddSize() const {
        return this->hddSize;
    }
    const QString& getCachePath() const {
        return this->cachePath;
    }
    const QString& getHostGroup() const {
        return this->hostGroup;
    }
    const unsigned int& getRootTimeout() const {
        return this->rootTimeout;
    }
    const bool& getAutoPartition() {
        return this->autoPartition;
    }
    const bool& getAutoInitCache() {
        return this->autoInitCache;
    }
    const QString& getBackgroundColor() {
        return this->backgroundColor;
    }
    const DownloadMethod& getDownloadMethod() {
        return this->downloadMethod;
    }
    const bool& getAutoFormat() {
        return this->autoFormat;
    }
    const bool& getUseMinimalLayout() {
        return this->useMinimalLayout;
    }
    const bool& getClientDetailsVisibleByDefault() {
        return this->clientDetailsVisibleByDefault;
    }
    const QString& getLocale() const {
        return this->locale;
    }
    bool getGuiDisabled() {
        return this->guiDisabled;
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

    bool isBackgroundColorDark();

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

    QString serverIpAddress;
    QString ipAddress;
    QString subnetMask;
    QString subnetBitmask;
    QString macAddress;
    QString linboVersion;
    QString hostname;
    QString cpuModel;
    QString ramSize;
    QString cacheSize;
    QString hddSize;
    QString cachePath;
    QString hostGroup;
    DownloadMethod downloadMethod;
    QString backgroundColor;
    QString locale;
    unsigned int rootTimeout;
    bool autoPartition;
    bool autoInitCache;
    bool autoFormat;
    bool useMinimalLayout;
    bool guiDisabled;
    bool clientDetailsVisibleByDefault;

signals:

};

#endif // LINBOCONFIG_H
