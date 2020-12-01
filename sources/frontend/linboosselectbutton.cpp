/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020  Dorian Zedler <dorian@itsblue.de>
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

#include "linboosselectbutton.h"

LinboOsSelectButton::LinboOsSelectButton(QString icon, LinboOs* os, QButtonGroup* buttonGroup, QWidget* parent) : QWidget(parent)
{
    this->buttonGroup = buttonGroup;

    this->button = new QModernPushButton(icon, this);
    this->button->setCheckable(true);
    this->buttonGroup->addButton(this->button);

    this->os = os;

    QWidget::setVisible(true);
}

LinboOs* LinboOsSelectButton::getOs() {
    return this->os;
}

void LinboOsSelectButton::setVisibleAnimated(bool visible) {
    this->button->setVisibleAnimated(visible);
}

void LinboOsSelectButton::setVisible(bool visible) {
    this->button->setVisible(visible);
}

void LinboOsSelectButton::resizeEvent(QResizeEvent *event) {
    //qDebug() << "Resize: width: " << event->size().width() << " height: " << event->size().height();
    this->button->setGeometry(QRect(event->size().width() / 2 - event->size().height() / 2 , 0, event->size().height(), event->size().height()));
}
