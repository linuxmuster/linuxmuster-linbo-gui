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

#include "linbostackedwidget.h"

LinboStackedWidget::LinboStackedWidget(QWidget* parent) : QStackedWidget(parent)
{
    this->animationState = Idle;
    opacityAnimation = new QPropertyAnimation();
    opacityAnimation->setPropertyName("opacity");
    opacityAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    opacityAnimation->setDuration(400);
    opacityAnimation->setLoopCount(1);
    connect(opacityAnimation, SIGNAL(finished()), this, SLOT(handleAnimationFinished()));
}

void LinboStackedWidget::setCurrentWidgetAnimated(QWidget* widget) {
    if(widget == nullptr || (widget == this->currentWidget() && this->animationState == Idle))
        return;
    else if(widget == this->currentWidget() && this->animationState != Idle) {
        this->newWidget = widget;
        if(this->animationState == FadingOut) {
            return;
        }
        else {
            this->handleAnimationFinished();
            return;
        }
    }

    this->newWidget = widget;

    // fade old widget out
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    this->currentWidget()->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(1);

    opacityAnimation->setStartValue(1);
    opacityAnimation->setEndValue(0);
    opacityAnimation->setTargetObject(opacityEffect);
    opacityAnimation->start();

    this->animationState = FadingOut;
}

void LinboStackedWidget::handleAnimationFinished() {
    // disable graphical effect to prevent errors
    this->currentWidget()->graphicsEffect()->setEnabled(false);

    if(this->newWidget == nullptr) {
        this->animationState = Idle;
        return;
    }

    // hide new widget
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    this->newWidget->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(0);

    // set new widget
    this->setCurrentWidget(this->newWidget);
    this->newWidget = nullptr;

    // fade new widget in
    opacityAnimation->setTargetObject(opacityEffect);
    opacityAnimation->setStartValue(0);
    opacityAnimation->setEndValue(1);
    opacityAnimation->start();

    this->animationState = FadingIn;
}
