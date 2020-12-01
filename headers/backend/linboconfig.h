/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020  Dorian Zedler <dorian@itsblue.de>
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

class LinboConfig : public QObject
{
    Q_OBJECT
public:
    friend class LinboBackend;

    const QString& getServerIpAddress() const {return this->serverIpAddress;}
    const QString& getIpAddress() const {return this->ipAddress;}
    const QString& getMacAddress() const {return this->macAddress;}
    const QString& getLinboVersion() const {return this->LinboVersion;}
    const QString& getHostname() const {return this->hostname;}
    const QString& getCpuModel() const {return this->cpuModel;}
    const QString& getRamSize() const {return this->ramSize;}
    const QString& getCacheSize() const {return this->cacheSize;}
    const QString& getHddSize() const {return this->hddSize;}

    const QString& getCachePath() const {return this->cachePath;}
    const QString& getHostGroup() const {return this->hostGroup;}
    const unsigned int& getRootTimeout() const {return this->rootTimeout;}
    const bool& getAutoPartition() {return this->autoPartition;}
    const bool& getAutoInitCache() {return this->autoInitCache;}
    const QString& getBackgroundFontcolor() {return this->backgroundFontcolor;}
    const QString& getConsoleFontcolorStdout() {return this->consoleFontcolorStdout;}
    const QString& getConsoleFontcolorStderr() {return this->consoleFontcolorStderr;}
    const QString& getDownloadType() {return this->downloadType;}
    const bool& getAutoFormat() {return this->autoFormat;}

private:
    explicit LinboConfig(QObject *parent = nullptr);

    void setServerIpAddress( const QString& serverIpAddress ) {this->serverIpAddress = serverIpAddress;}
    void setIpAddress( const QString& ipAddress ) {this->ipAddress = ipAddress;}
    void setMacAddress( const QString& macAddress ) {this->macAddress = macAddress;}
    void setLinboVersion( const QString& linboVersion ) {this->LinboVersion = linboVersion;}
    void setHostname( const QString& hostname ) {this->hostname = hostname;}
    void setCpuModel( const QString& cpuModel ) {this->cpuModel = cpuModel;}
    void setRamSize( const QString& ramSize ) {this->ramSize = ramSize;}
    void setCacheSize( const QString& cacheSize ) {this->cacheSize = cacheSize;}
    void setHddSize( const QString& hddSize ) {this->hddSize = hddSize;}
    void setCachePath( const QString& cachePath ) {this->cachePath = cachePath;}
    void setHostGroup( const QString& hostGroup ) {this->hostGroup = hostGroup;}
    void setRootTimeout( const unsigned int& rootTimeout ) {this->rootTimeout = rootTimeout;}
    void setAutoPartition( const bool& autoPartition ){this->autoPartition = autoPartition;}
    void setAutoInitCache( const bool& autoInitCache ){this->autoInitCache = autoInitCache;}
    void setBackgroundFontcolor( const QString& backgroundFontcolor ){this->backgroundFontcolor = backgroundFontcolor;}
    void setConsoleFontcolorStdout( const QString& consoleFontcolorStdout ){this->consoleFontcolorStdout = consoleFontcolorStdout;}
    void setConsoleFontcolorStderr( const QString& consoleFontcolorStderr ){this->consoleFontcolorStderr = consoleFontcolorStderr;}
    void setDownloadType( const QString& downloadType ){this->downloadType = downloadType;}
    void setAutoFormat( const bool& autoFormat ){this->autoFormat = autoFormat;}

  QString serverIpAddress;
  QString ipAddress;
  QString macAddress;
  QString LinboVersion;
  QString hostname;
  QString cpuModel;
  QString ramSize;
  QString cacheSize;
  QString hddSize;
  QString cachePath;
  QString hostGroup;
  QString downloadType;
  QString backgroundFontcolor;
  QString consoleFontcolorStdout;
  QString consoleFontcolorStderr;
  unsigned int rootTimeout;
  bool autoPartition;
  bool autoInitCache;
  bool autoFormat;

signals:

};

#endif // LINBOCONFIG_H
