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

#ifndef LINBOOS_H
#define LINBOOS_H

#include <QObject>
#include <QtDebug>

#include "linboimage.h"

class LinboBackend;

class LinboOs : public QObject
{
    Q_OBJECT
public:
    friend class LinboBackend;

    enum LinboOsStartAction {
        UnknownAction = -1,
        StartOs,
        SyncOs,
        ReinstallOs
    };

    const QString& getName() const {return this->name;}
    const QString& getDescription() const {return this->description;}
    const QString& getVersion() const {return this->version;}
    LinboImage* getBaseImage() const {return this->baseImage;}
    const QString& getIconName() const {return this->iconName;}
    const QString& getRootPartition() const {return this->rootPartition;}
    const QString& getBootPartition() const {return this->bootPartition;}
    const QString& getKernel() const {return this->kernel;}
    const QString& getInitrd() const {return this->initrd;}
    const QString& getKernelOptions() const {return this->kernelOptions;}
    bool getSyncActionEnabled() const {return this->baseImage != nullptr && this->syncButtonEnabled;}
    bool getStartActionEnabled() const {return this->baseImage != nullptr && this->startButtonEnabled;}
    bool getReinstallActionEnabled() const {return this->baseImage != nullptr && this->reinstallButtonEnabled;}
    bool getAutostartEnabled() const {return this->baseImage != nullptr && this->autostartEnabled;}
    const int& getAutostartTimeout() const {return this->autostartTimeout;}
    const LinboOsStartAction& getDefaultAction() const {return this->defaultAction;}
    const bool& getHidden() const {return this->hidden;}

    bool getActionEnabled(LinboOsStartAction action);
    static LinboOsStartAction startActionFromString(const QString& name);

    bool start();
    bool sync();
    bool reinstall();

    LinboBackend* getBackend() {return this->parent;}

private:
    explicit LinboOs(LinboBackend *parent = nullptr);

    void setName (const QString& name) {this->name = name;}
    void setDescription  (const QString& description) {this->description = description;}
    void setVersion (const QString& version) {this->version = version;}
    void setBaseImage (LinboImage* baseImage);
    void setIconName (const QString& iconName) {this->iconName = iconName;}
    void setRootPartition (const QString& rootPartition) {this->rootPartition = rootPartition;}
    void setBootPartition (const QString& bootPartition) {this->bootPartition = bootPartition;}
    void setKernel (const QString& kernel) {this->kernel = kernel;}
    void setInitrd (const QString& initrd) {this->initrd = initrd;}
    void setKernelOptions (const QString& kernelOptions) {this->kernelOptions = kernelOptions;}
    void setSyncButtonEnabled (const bool& syncButtonEnabled) {this->syncButtonEnabled = syncButtonEnabled;}
    void setStartButtonEnabled (const bool& startButtonEnabled) {this->startButtonEnabled = startButtonEnabled;}
    void setReinstallButtonEnabled (const bool& reinstallButtonEnabled) {this->reinstallButtonEnabled = reinstallButtonEnabled;}
    void setAutostartEnabled (const bool& autostartEnabled) {this->autostartEnabled = autostartEnabled;}
    void setAutostartTimeout (const int& autostartTimeout) {this->autostartTimeout = autostartTimeout;}
    void setDefaultAction (const LinboOsStartAction& defaultAction) {this->defaultAction = defaultAction;}
    void setHidden (const bool& hidden) {this->hidden = hidden;}

    LinboBackend* parent;

    QString name;
    QString version;
    QString description;
    QString iconName;
    QString rootPartition;
    QString bootPartition;
    QString image;
    QString kernel;
    QString initrd;
    QString kernelOptions;

    int autostartTimeout;

    bool syncButtonEnabled;
    bool startButtonEnabled;
    bool reinstallButtonEnabled;
    bool autostartEnabled;
    bool hidden;

    LinboOsStartAction defaultAction;
    LinboImage* baseImage;
};

#endif // LINBOOS_H
