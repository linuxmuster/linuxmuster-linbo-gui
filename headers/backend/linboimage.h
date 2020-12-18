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

#ifndef LINBOIMAGE_H
#define LINBOIMAGE_H

#include <QObject>

class LinboImage : public QObject
{
    Q_OBJECT
public:
    friend class LinboBackend;

    enum ImageType {
        BaseImage,
        DifferentialImage
    };

    const QString& getDescription() const {return this->description;}
    const QString& getName() const {return this->name;}
    const ImageType& getType() const {return this->type;}

    ImageType getType();

protected:
    explicit LinboImage(QString name, ImageType type, QObject *parent = nullptr);

    void setDescription (const QString& description) {this->description = description;}
    void setName(const QString& name) {this->name = name;}
    void setType(const ImageType& type) {this->type = type;}

private:
  QString name;
  QString description;
  ImageType type;


};

#endif // LINBOIMAGE_H
