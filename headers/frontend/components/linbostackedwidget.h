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

#ifndef QMODERNSTACKEDWIDGET_H
#define QMODERNSTACKEDWIDGET_H

#include <QStackedWidget>
#include <QObject>
#include <QtDebug>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class LinboStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    LinboStackedWidget(QWidget* parent = nullptr);

    void setCurrentWidgetAnimated(QWidget* widget);

private:
    enum _AnimationState {
        Idle,
        FadingOut,
        FadingIn
    };

    _AnimationState _animationState;
    QWidget* _newWidget;
    QPropertyAnimation* _opacityAnimation;

private slots:
    void handleAnimationFinished();
};

#endif // QMODERNSTACKEDWIDGET_H
