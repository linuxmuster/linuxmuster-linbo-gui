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

    const QString& getPath() const {
        return this->path;
    }
    const QString& getId() const {
        return this->id;
    }
    const QString& getFstype() const {
        return this->fstype;
    }
    const unsigned int& getSize() const {
        return this->size;
    }
    const bool& getBootable() const {
        return this->bootable;
    }

private:
    explicit LinboDiskPartition(QObject *parent = nullptr);

    void setPath( const QString& path ) {
        this->path = path;
    }
    void setId( const QString& id ) {
        this->id = id;
    }
    void setFstype( const QString& fstype ) {
        this->fstype = fstype;
    }
    void setSize( const unsigned int& size ) {
        this->size = size;
    }
    void setBootable( const bool& bootable ) {
        this->bootable = bootable;
    }

    QString path;
    QString id;
    QString fstype;
    unsigned int size;
    bool bootable;

signals:

};

#endif // LINBODISKPARTITION_H
