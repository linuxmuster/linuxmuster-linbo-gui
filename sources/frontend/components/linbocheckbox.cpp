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

#include "linbocheckbox.h"

LinboCheckBox::LinboCheckBox(QString label, QWidget* parent) : QCheckBox(label, parent)
{
}

void LinboCheckBox::paintEvent(QPaintEvent *e) {
    QString indicatorSize = QString::number(this->font().pixelSize());
    this->setStyleSheet("QCheckBox {"
                        "color: " + gTheme->getColor(LinboTheme::TextColor).name() + ";"
                        "}"
                        "QCheckBox::indicator {"
                        "width: " + indicatorSize + "px;"
                        "height: " + indicatorSize + "px;"
                        "}"
                        "QCheckBox::indicator:unchecked {"
                        "image: url(" + gTheme->getIconPath(LinboTheme::CheckBoxUncheckedIcon) + ");"
                        "}"
                        "QCheckBox::indicator:checked {"
                        "image: url(" + gTheme->getIconPath(LinboTheme::CheckBoxCheckedIcon) + ");"
                        "}"
                        "");

    QCheckBox::paintEvent(e);
}
