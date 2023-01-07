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

#ifndef LINBOOS_H
#define LINBOOS_H

#include <QObject>
#include <QtDebug>

#include "linboimage.h"
#include "linbopostprocessactions.h"

class LinboOs : public QObject
{
    Q_OBJECT
public:
    friend class LinboConfigReader;

    enum LinboOsStartAction {
        UnknownAction = -1,
        StartOs,
        SyncOs,
        ReinstallOs
    };

    const QString& name() const {
        return this->_name;
    }
    const QString& description() const {
        return this->_description;
    }
    const QString& version() const {
        return this->_version;
    }
    LinboImage* baseImage() const {
        return this->_baseImage;
    }
    const QString& iconName() const {
        return this->_iconName;
    }
    const QString& rootPartition() const {
        return this->_rootPartition;
    }
    const QString& bootPartition() const {
        return this->_bootPartition;
    }
    const QString& kernel() const {
        return this->_kernel;
    }
    const QString& initrd() const {
        return this->_initrd;
    }
    const QString& kernelOptions() const {
        return this->_kernelOptions;
    }
    bool syncActionEnabled() const {
        return this->_baseImage != nullptr && this->_syncButtonEnabled;
    }
    bool startActionEnabled() const {
        return this->_startButtonEnabled;
    }
    bool reinstallActionEnabled() const {
        return this->_baseImage != nullptr && this->_reinstallButtonEnabled;
    }
    bool autostartEnabled() const {
        return this->_baseImage != nullptr && this->_autostartEnabled;
    }
    const int& autostartTimeout() const {
        return this->_autostartTimeout;
    }
    LinboOsStartAction defaultAction();
    const bool& getHidden() const {
        return this->_hidden;
    }

    bool actionEnabled(LinboOsStartAction action);
    static LinboOsStartAction startActionFromString(const QString& name);

    bool executeStart();
    bool executeSync();
    bool executeReinstall();
    bool executeDefaultAction();

    bool createBaseImage(QString description = "", LinboPostProcessActions::Flags postProcessActions = LinboPostProcessActions::NoAction);
    bool createDiffImage(QString description = "", LinboPostProcessActions::Flags postProcessActions = LinboPostProcessActions::NoAction);

private:
    explicit LinboOs(LinboBackend *parent = nullptr);

    void _setBaseImage(LinboImage* baseImage);

    LinboBackend* _backend;

    QString _name;
    QString _version;
    QString _description;
    QString _iconName;
    QString _rootPartition;
    QString _bootPartition;
    QString _image;
    QString _kernel;
    QString _initrd;
    QString _kernelOptions;

    int _autostartTimeout;

    bool _syncButtonEnabled;
    bool _startButtonEnabled;
    bool _reinstallButtonEnabled;
    bool _autostartEnabled;
    bool _hidden;

    LinboOsStartAction _defaultAction;
    LinboImage* _baseImage;
};

#endif // LINBOOS_H
