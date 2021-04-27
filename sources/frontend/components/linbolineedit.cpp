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

#include "../../../headers/frontend/components/linbolineedit.h"

LinboLineEdit::LinboLineEdit(QWidget* parent) : QLineEdit(parent)
{
    this->setStyleSheet(
        "QLineEdit {"
        "border: 0 0 0 0;"
        "border-bottom: 1px solid " + gTheme->getColor(LinboTheme::LineColor).name() + ";"
        "background: " + gTheme->getColor(LinboTheme::ElevatedBackgroundColor).name() + ";"
        "padding-left: 10px;"
        "color: " + gTheme->getColor(LinboTheme::TextColor).name() + ";"
        "}"
        "QLineEdit:focus {"
        "border-bottom: 1px solid " + gTheme->getColor(LinboTheme::AccentColor).name() + ";"
        "}"
        "QLineEdit::disabled {"
        "border-bottom: 0;"
        "}"
    );
}

void LinboLineEdit::resizeEvent(QResizeEvent *event) {
    QLineEdit::resizeEvent(event);
}
