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

#ifndef QMODERNPUSHBUTTON_H
#define QMODERNPUSHBUTTON_H

#include <QPushButton>
#include <QObject>
#include <QSvgWidget>
#include <QGraphicsOpacityEffect>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QEventLoop>
#include <QLabel>
#include <QTimer>

#include "linboguitheme.h"
#include "linbopushbuttonoverlay.h"

class LinboPushButton : public QAbstractButton
{
    Q_OBJECT
public:
    LinboPushButton(QString icon, QWidget* parent = nullptr);
    LinboPushButton(QString icon, QString label, QWidget* parent = nullptr);
    LinboPushButton(QString icon, QString label, QList<LinboPushButtonOverlay*> extraOverlays, QWidget* parent = nullptr);

    void setVisible(bool visible) override;
    void setVisibleAnimated(bool visible);

    void setGeometryAnimated(const QRect& geometry);

    void setOverlayTypeMuted(LinboPushButtonOverlay::OverlayType overlayType, bool muted);

protected:
    void resizeEvent(QResizeEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
    void enterEvent(QEnterEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;

private:
    bool _shouldBeVisible;
    bool _isHovered;
    bool _isFocused;
    bool _isPressed;
    QPropertyAnimation* _geometryAnimation;
    QList<LinboPushButtonOverlay*> _overlays;
    QList<LinboPushButtonOverlay::OverlayType> _mutedOverlayTypes;
    QSvgWidget* _svgIcon;
    QSvgWidget* _hoveredOverlay;
    QLabel *_label;

    bool _overlayTypeIsMuted(LinboPushButtonOverlay::OverlayType overlayType);
    QList<LinboPushButtonOverlay*> _getOverlaysOfType(LinboPushButtonOverlay::OverlayType type);
    QList<LinboPushButtonOverlay*> _getOverlaysOfType(LinboPushButtonOverlay::OverlayType type, QList<LinboPushButtonOverlay*> overlays);

private slots:
    void _handleToggled(bool checked);

signals:
    void checked();
    void hovered();
    void defocused(Qt::FocusReason reason);
    void doubleClicked();
};

#endif // QMODERNPUSHBUTTON_H
