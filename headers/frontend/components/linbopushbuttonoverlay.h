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

#ifndef QMODERNPUSHBUTTONOVERLAY_H
#define QMODERNPUSHBUTTONOVERLAY_H

#include <QObject>
#include <QSvgWidget>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDebug>

// This class automatically enables and disabled the QGraphicsOpacityEffect as needed, to prevent
// "QPainter::begin: A paint device can only be painted by one painter at a time." Error messages
// When applying a QGraphicalEffect to the parent
class LinboPushButtonOverlay : public QObject
{
    Q_OBJECT
public:
    friend class LinboPushButton;

    enum OverlayType {
        Background,
        OnHover,
        OnPressed,
        OnChecked
    };

    explicit LinboPushButtonOverlay(OverlayType type, QWidget* overlayWidget, bool managedAutomatically, QObject *parent = nullptr);

    void setVisible(bool visible);
    void setVisibleAnimated(bool visible);
private:

    bool shouldBeVisible;
    bool managedAutomatically;
    OverlayType type;
    QWidget* widget;
    QGraphicsOpacityEffect* effect;
    QPropertyAnimation* animation;

    void setAnimationDuration(int duration);
    void setEffectEnabled(bool enabled);
    OverlayType getType();

private slots:
    void handleAnimationStateChanged(QAbstractAnimation::State newState, QAbstractAnimation::State);

signals:

};

#endif // QMODERNPUSHBUTTONOVERLAY_H
