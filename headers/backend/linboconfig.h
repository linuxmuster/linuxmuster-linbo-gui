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

class LinboConfig : public QObject
{
    Q_OBJECT
public:
    friend class LinboBackend;

    enum DownloadMethod {
        Rsync,
        Multicast,
        Torrent
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
        return this->LinboVersion;
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
    bool isBackgroundColorDark();
    const DownloadMethod& getDownloadMethod() {
        return this->downloadMethod;
    }
    const bool& getAutoFormat() {
        return this->autoFormat;
    }
    const bool& getUseMinimalLayout() {
        return this->useMinimalLayout;
    }

    const QString& getLocale() const {
        return this->locale;
    }

    bool getGuiDisabled() {
        return this->guiDisabled;
    }

private:
    explicit LinboConfig(QObject *parent = nullptr);

    void setServerIpAddress( const QString& serverIpAddress ) {
        this->serverIpAddress = serverIpAddress;
    }
    void setIpAddress( const QString& ipAddress ) {
        this->ipAddress = ipAddress;
    }
    void setSubnetMask(const QString& subnetMask) {
        this->subnetMask = subnetMask;
    }
    void setSubnetBitmask(const QString& subnetBitmask) {
        this->subnetBitmask = subnetBitmask;
    }
    void setMacAddress( const QString& macAddress ) {
        this->macAddress = macAddress;
    }
    void setLinboVersion( const QString& linboVersion ) {
        this->LinboVersion = linboVersion;
    }
    void setHostname( const QString& hostname ) {
        this->hostname = hostname;
    }
    void setCpuModel( const QString& cpuModel ) {
        this->cpuModel = cpuModel;
    }
    void setRamSize( const QString& ramSize ) {
        this->ramSize = ramSize;
    }
    void setCacheSize( const QString& cacheSize ) {
        this->cacheSize = cacheSize;
    }
    void setHddSize( const QString& hddSize ) {
        this->hddSize = hddSize;
    }
    void setCachePath( const QString& cachePath ) {
        this->cachePath = cachePath;
    }
    void setHostGroup( const QString& hostGroup ) {
        this->hostGroup = hostGroup;
    }
    void setRootTimeout( const unsigned int& rootTimeout ) {
        this->rootTimeout = rootTimeout;
    }
    void setAutoPartition( const bool& autoPartition ) {
        this->autoPartition = autoPartition;
    }
    void setAutoInitCache( const bool& autoInitCache ) {
        this->autoInitCache = autoInitCache;
    }
    void setBackgroundColor( const QString& backgroundColor ) {
        this->backgroundColor = backgroundColor;
    }
    void setDownloadMethod( const DownloadMethod& downloadMethod ) {
        this->downloadMethod = downloadMethod;
    }
    void setAutoFormat( const bool& autoFormat ) {
        this->autoFormat = autoFormat;
    }
    void setUseMinimalLayout(const bool& useMinimalLayout) {
        this->useMinimalLayout = useMinimalLayout;
    }

    void setLocale( const QString& locale ) {
        this->locale = locale;
    }

    void setGuiDisabled(bool guiDisabled) {
        this->guiDisabled = guiDisabled;
    }

    QString serverIpAddress;
    QString ipAddress;
    QString subnetMask;
    QString subnetBitmask;
    QString macAddress;
    QString LinboVersion;
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

signals:

};

#endif // LINBOCONFIG_H
