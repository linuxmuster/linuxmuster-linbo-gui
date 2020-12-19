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

#include "qmodernpushbuttonoverlay.h"

QModernPushButtonOverlay::QModernPushButtonOverlay(OverlayType type, QWidget* overlayWidget, bool managedAutomatically, QObject *parent) : QObject(parent)
{
    this->shouldBeVisible = false;
    this->managedAutomatically = managedAutomatically;
    this->type = type;
    this->widget = overlayWidget;
    this->widget->setVisible(false);

    this->effect = new QGraphicsOpacityEffect(overlayWidget);
    this->effect->setOpacity(0);
    this->effect->setEnabled(false);
    this->widget->setGraphicsEffect(this->effect);

    this->animation = new QPropertyAnimation(this->effect, "opacity");
    this->animation->setDuration(400);
    this->animation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));

    this->setEffectEnabled(false);

    connect(this->animation, SIGNAL(stateChanged(QAbstractAnimation::State, QAbstractAnimation::State)), this, SLOT(handleAnimationStateChanged(QAbstractAnimation::State, QAbstractAnimation::State)));
}


void QModernPushButtonOverlay::setAnimationDuration(int duration) {
    this->animation->setDuration(duration);
}

void QModernPushButtonOverlay::setVisible(bool visible) {
    if(this->shouldBeVisible == visible)
        return;

    this->shouldBeVisible = visible;

    this->effect->setOpacity(0);
    this->widget->setVisible(visible);
}

void QModernPushButtonOverlay::setVisibleAnimated(bool visible) {
    if(this->shouldBeVisible == visible)
        return;

    this->shouldBeVisible = visible;

    int startValue = visible ? 0:1;
    this->animation->stop();
    this->animation->setStartValue(startValue);
    this->animation->setEndValue(1-startValue);
    this->animation->start();
}

void QModernPushButtonOverlay::setEffectEnabled(bool enabled) {
    if(enabled) {
        this->effect->setEnabled(true);
        this->widget->setVisible(true);
    }
    else {
        this->widget->setVisible(this->effect->opacity() > 0);
        this->effect->setEnabled(false);
    }
}

void QModernPushButtonOverlay::handleAnimationStateChanged(QAbstractAnimation::State newState, QAbstractAnimation::State) {
    switch (newState) {
    case QAbstractAnimation::Running:
        this->setEffectEnabled(true);
        break;
    case QAbstractAnimation::Stopped:
        this->setEffectEnabled(false);
        break;
    default:
        break;
    }
}

QModernPushButtonOverlay::OverlayType QModernPushButtonOverlay::getType() {
    return this->type;
}
