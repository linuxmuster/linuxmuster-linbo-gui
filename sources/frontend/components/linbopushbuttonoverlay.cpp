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

#include "linbopushbuttonoverlay.h"

LinboPushButtonOverlay::LinboPushButtonOverlay(OverlayType type, QWidget* overlayWidget, bool managedAutomatically, QObject *parent) : QObject(parent)
{
    this->_shouldBeVisible = false;
    this->_managedAutomatically = managedAutomatically;
    this->_type = type;
    this->_widget = overlayWidget;
    this->_widget->setVisible(false);

    this->_effect = new QGraphicsOpacityEffect(overlayWidget);
    this->_effect->setOpacity(0);
    this->_effect->setEnabled(false);
    this->_widget->setGraphicsEffect(this->_effect);

    this->_animation = new QPropertyAnimation(this->_effect, "opacity");
    this->_animation->setDuration(400);
    this->_animation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));

    this->_setEffectEnabled(false);

    connect(this->_animation, &QPropertyAnimation::stateChanged, this, &LinboPushButtonOverlay::_handleAnimationStateChanged);
}


void LinboPushButtonOverlay::_setAnimationDuration(int duration) {
    this->_animation->setDuration(duration);
}

void LinboPushButtonOverlay::setVisible(bool visible) {
    if(this->_shouldBeVisible == visible)
        return;

    this->_shouldBeVisible = visible;

    this->_effect->setOpacity(0);
    this->_widget->setVisible(visible);
}

void LinboPushButtonOverlay::setVisibleAnimated(bool visible) {
    if(this->_shouldBeVisible == visible)
        return;

    this->_shouldBeVisible = visible;

    int startValue = visible ? 0:1;
    this->_animation->stop();
    this->_animation->setStartValue(startValue);
    this->_animation->setEndValue(1-startValue);
    this->_animation->start();
}

void LinboPushButtonOverlay::_setEffectEnabled(bool enabled) {
    if(enabled) {
        this->_effect->setEnabled(true);
        this->_widget->setVisible(true);
    }
    else {
        this->_widget->setVisible(this->_effect->opacity() > 0);
        this->_effect->setEnabled(false);
    }
}

void LinboPushButtonOverlay::_handleAnimationStateChanged(QAbstractAnimation::State newState, QAbstractAnimation::State) {
    switch (newState) {
    case QAbstractAnimation::Running:
        this->_setEffectEnabled(true);
        break;
    case QAbstractAnimation::Stopped:
        this->_setEffectEnabled(false);
        break;
    default:
        break;
    }
}

LinboPushButtonOverlay::OverlayType LinboPushButtonOverlay::_getType() {
    return this->_type;
}
