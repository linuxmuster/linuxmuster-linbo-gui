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

#ifndef LINBODISKPARTITION_H
#define LINBODISKPARTITION_H

#include <QObject>

class LinboDiskPartition : public QObject
{
    Q_OBJECT
public:
    friend class LinboConfigReader;

    const QString& path() const {
        return this->_path;
    }
    const QString& id() const {
        return this->_id;
    }
    const QString& fstype() const {
        return this->_fstype;
    }
    const unsigned int& size() const {
        return this->_size;
    }
    const bool& bootable() const {
        return this->_bootable;
    }

private:
    explicit LinboDiskPartition(QObject *parent = nullptr);

    QString _path;
    QString _id;
    QString _fstype;
    unsigned int _size;
    bool _bootable;

signals:

};

#endif // LINBODISKPARTITION_H
