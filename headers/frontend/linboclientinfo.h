/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020-2021  Dorian Zedler <dorian@itsblue.de>
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

#ifndef LINBOCLIENTINFO_H
#define LINBOCLIENTINFO_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QSvgWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtDebug>

#include "linboconfig.h"
#include "linboguitheme.h"

class LinboClientInfo : public QWidget
{
    Q_OBJECT
public:
    explicit LinboClientInfo(LinboConfig* config, QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QWidget* _mainWidget;
    QGridLayout* _mainLayout;

    QSvgWidget* _networkIconWidget;
    QSvgWidget* _desktopIconWidget;

signals:

};

#endif // LINBOCLIENTINFO_H
