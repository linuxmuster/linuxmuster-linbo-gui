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

#ifndef LINBOIMAGE_H
#define LINBOIMAGE_H

#include <QObject>

#include "linbopostprocessactions.h"

class LinboBackend;
class LinboOs;

class LinboImage : public QObject
{
    Q_OBJECT
public:
    friend class LinboConfigReader;
    friend class LinboBackend;
    friend class LinboOs;

    QString getDescription();
    bool upload(LinboPostProcessActions::Flags postProcessActions);

    QString name() {
        return this->_name;
    }
    LinboOs* os() {
        return this->_os;
    }
    bool hasOs() {
        return this->_os != nullptr;
    }
    bool existsOnDisk() {
        return this->_existsOnDisk;
    }

protected:
    explicit LinboImage(QString name, LinboBackend *parent = nullptr);

    bool setDescription (const QString& description);

private:
    LinboBackend* _backend;
    LinboOs* _os;
    QString _name;
    bool _existsOnDisk;
};

#endif // LINBOIMAGE_H
