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

#include "linbostackedwidget.h"

LinboStackedWidget::LinboStackedWidget(QWidget* parent) : QStackedWidget(parent)
{
    this->_animationState = Idle;
    _opacityAnimation = new QPropertyAnimation();
    _opacityAnimation->setPropertyName("opacity");
    _opacityAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    _opacityAnimation->setDuration(400);
    _opacityAnimation->setLoopCount(1);
    connect(_opacityAnimation, &QPropertyAnimation::finished, this, &LinboStackedWidget::handleAnimationFinished);
}

void LinboStackedWidget::setCurrentWidgetAnimated(QWidget* widget) {
    if(widget == nullptr || (widget == this->currentWidget() && this->_animationState == Idle))
        return;
    else if(widget == this->currentWidget() && this->_animationState != Idle) {
        this->_newWidget = widget;
        if(this->_animationState == FadingOut) {
            return;
        }
        else {
            this->handleAnimationFinished();
            return;
        }
    }

    this->_newWidget = widget;

    // fade old widget out
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    this->currentWidget()->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(1);

    _opacityAnimation->setStartValue(1);
    _opacityAnimation->setEndValue(0);
    _opacityAnimation->setTargetObject(opacityEffect);
    _opacityAnimation->start();

    this->_animationState = FadingOut;
}

void LinboStackedWidget::handleAnimationFinished() {
    // disable graphical effect to prevent errors
    this->currentWidget()->graphicsEffect()->setEnabled(false);

    if(this->_newWidget == nullptr) {
        this->_animationState = Idle;
        return;
    }

    // hide new widget
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    this->_newWidget->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(0);

    // set new widget
    this->setCurrentWidget(this->_newWidget);
    this->_newWidget = nullptr;

    // fade new widget in
    _opacityAnimation->setTargetObject(opacityEffect);
    _opacityAnimation->setStartValue(0);
    _opacityAnimation->setEndValue(1);
    _opacityAnimation->start();

    this->_animationState = FadingIn;
}
