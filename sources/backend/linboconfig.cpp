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

#include "linboconfig.h"
#include <QtDebug>

LinboConfig::LinboConfig(QObject *parent) : QObject(parent)
{
    this->autoPartition = 0;
    this->autoInitCache = 0;
    this->backgroundColor = "#2a4457";
    this->locale = "";
    this->downloadMethod = LinboConfig::Rsync;
    this->autoFormat = 0;
    this->useMinimalLayout = false;
    this->guiDisabled = false;
}

bool LinboConfig::isBackgroundColorDark() {
    QColor backgroundColor = QColor(this->getBackgroundColor());
    int h, s, v;
    backgroundColor.getHsv(&h, &s, &v);
    return v < 210;
}
