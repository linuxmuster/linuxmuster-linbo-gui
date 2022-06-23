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

#ifndef LINBOTOOLBUTTON_H
#define LINBOTOOLBUTTON_H

#include <linbopushbutton.h>
#include <QObject>
#include <QWidget>

#include "linboguitheme.h"

class LinboToolButton : public LinboPushButton
{
    Q_OBJECT
public:
    LinboToolButton(QString text, QWidget* parent = nullptr);
    LinboToolButton(LinboTheme::Icon icon, QWidget* parent = nullptr);
    LinboToolButton(QString text, LinboTheme::Icon icon, QWidget* parent = nullptr);
    LinboToolButton(QString text, LinboTheme::Icon icon, LinboTheme::ColorRole colorRole, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    LinboTheme::ColorRole _colorRole;

};

#endif // LINBOTOOLBUTTON_H
