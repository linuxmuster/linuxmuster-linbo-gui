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

#include "linbotoolbutton.h"

LinboToolButton::LinboToolButton(QString text, QWidget* parent) : LinboToolButton(text, LinboGuiTheme::NoIcon, LinboGuiTheme::ToolButtonColor, parent)
{
}

LinboToolButton::LinboToolButton(LinboGuiTheme::LinboGuiIcon icon, QWidget* parent) : LinboToolButton("", icon, LinboGuiTheme::ToolButtonColor, parent)
{
}

LinboToolButton::LinboToolButton(QString text, LinboGuiTheme::LinboGuiIcon icon, QWidget* parent) : LinboToolButton(text, icon, LinboGuiTheme::ToolButtonColor, parent)
{
}

LinboToolButton::LinboToolButton(QString text, LinboGuiTheme::LinboGuiIcon icon, LinboGuiTheme::LinboGuiColorRole colorRole, QWidget* parent) : LinboPushButton(gTheme->getIconPath(icon), text, parent)
{
    this->colorRole = colorRole;
}

void LinboToolButton::paintEvent(QPaintEvent *e) {
    LinboPushButton::paintEvent(e);
    if(this->colorRole == LinboGuiTheme::ToolButtonColor)
        LinboPushButton::setStyleSheet("QLabel { color: " + gTheme->getColor(this->colorRole).name() + "; font-weight: bold;} QLabel:disabled { color: " + gTheme->getColor(LinboGuiTheme::DisabledToolButtonColor).name() + ";}");
    else
        LinboPushButton::setStyleSheet("QLabel { color: " + gTheme->getColor(this->colorRole).name() + ";}");
}
