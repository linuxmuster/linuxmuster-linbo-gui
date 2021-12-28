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

#include "linbopushbutton.h"


LinboPushButton::LinboPushButton(QString icon, QWidget* parent) : LinboPushButton(icon, "", parent)
{
}

LinboPushButton::LinboPushButton(QString icon, QString label, QWidget* parent) : LinboPushButton(icon, label, {}, parent)
{
}

LinboPushButton::LinboPushButton(QString icon, QString label, QList<LinboPushButtonOverlay*> extraOverlays, QWidget* parent) : QAbstractButton(parent) {
    this->_svgIcon = nullptr;
    this->_label = nullptr;
    this->_shouldBeVisible = true;
    this->_isPressed = false;
    this->_isHovered = false;
    this->_isFocused = false;

    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::TabFocus);

    this->_geometryAnimation = new QPropertyAnimation(this, "geometry", this);
    this->_geometryAnimation->setDuration(400);
    this->_geometryAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    this->setObjectName(icon);

    // Background
    if(!icon.isEmpty()) {
        this->_svgIcon = new QSvgWidget(icon);
        this->_overlays.append(new LinboPushButtonOverlay(LinboPushButtonOverlay::Background, this->_svgIcon, true));
    }

    this->_label = new QLabel(label, this);
    this->_label->setAlignment(Qt::AlignCenter);
    this->_overlays.append(new LinboPushButtonOverlay(LinboPushButtonOverlay::Background, this->_label, true));
    this->_overlays.append(this->_getOverlaysOfType(LinboPushButtonOverlay::Background, extraOverlays));

    // Hover
    this->_hoveredOverlay = new QSvgWidget(gTheme->iconPath(LinboTheme::OverlayHoveredIcon));
    this->_overlays.append(
        new LinboPushButtonOverlay(
            LinboPushButtonOverlay::OnHover,
            this->_hoveredOverlay,
            true
        )
    );

    this->_overlays.append(this->_getOverlaysOfType(LinboPushButtonOverlay::OnHover, extraOverlays));

    // Pressed
    this->_overlays.append(
        new LinboPushButtonOverlay(
            LinboPushButtonOverlay::OnPressed,
            new QSvgWidget(gTheme->iconPath(LinboTheme::OverlayPressedIcon)),
            true
        )
    );

    this->_overlays.append(this->_getOverlaysOfType(LinboPushButtonOverlay::OnPressed, extraOverlays));

    // Checked
    this->_overlays.append(this->_getOverlaysOfType(LinboPushButtonOverlay::OnChecked, extraOverlays));

    // KeyboardFocus
    QSvgWidget* keyboardFocusOverlay = new QSvgWidget(gTheme->iconPath(LinboTheme::OverlayKeyboardFocusIcon));
    this->_overlays.append(
    new LinboPushButtonOverlay {
        LinboPushButtonOverlay::OnKeyboardFocus,
        keyboardFocusOverlay,
        true
    }
    );

    // set defaults
    for(LinboPushButtonOverlay* overlay : this->_overlays) {
        overlay->setParent(this);
        overlay->_widget->setParent(this);
    }

    for(LinboPushButtonOverlay* overlay : this->_getOverlaysOfType(LinboPushButtonOverlay::Background)) {
        if(overlay->_managedAutomatically)
            overlay->setVisibleAnimated(true);
        overlay->_setAnimationDuration(200);
    }

    for(LinboPushButtonOverlay* overlay : this->_getOverlaysOfType(LinboPushButtonOverlay::OnHover)) {
        overlay->_setAnimationDuration(200);
    }

    for(LinboPushButtonOverlay* overlay : this->_getOverlaysOfType(LinboPushButtonOverlay::OnPressed)) {
        overlay->_setAnimationDuration(100);
    }

    connect(this, &LinboPushButton::toggled, this, &LinboPushButton::_handleToggled);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void LinboPushButton::_handleToggled(bool checked) {
    if(checked)
        emit this->checked();
}

void LinboPushButton::resizeEvent(QResizeEvent *event) {
    QAbstractButton::resizeEvent(event);

    QList<QWidget*> doNotResizeWidgets;

    if(this->_label != nullptr && !this->_label->text().isEmpty()) {

        this->_label->setFixedHeight(this->height());

        QFont font = this->_label->font();
        font.setPixelSize(gTheme->toFontSize(this->height() * 0.5));
        this->_label->setFont(font);

        if(this->_svgIcon != nullptr) {
            this->_svgIcon->setGeometry(QRect(0, 0, this->height(), this->height()));

            this->_label->move(this->height() * 1.1, 0);
            this->_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            doNotResizeWidgets.append(this->_svgIcon);
            doNotResizeWidgets.append(this->_label);
        }
        else {
            this->_label->setAlignment(Qt::AlignCenter);
        }

        this->_label->adjustSize();
        this->setMinimumWidth((this->_label->x() + this->_label->sizeHint().width()) * 1.1);
    }

    for(LinboPushButtonOverlay* overlay : this->_overlays) {
        if(doNotResizeWidgets.contains(overlay->_widget))
            continue;

        overlay->_widget->setGeometry(QRect(0, 0, this->width(), this->height()));
    }
}

void LinboPushButton::setGeometryAnimated(const QRect& geometry) {
    this->_geometryAnimation->setStartValue(this->geometry());
    this->_geometryAnimation->setEndValue(geometry);
    this->_geometryAnimation->start();
}

void LinboPushButton::setVisible(bool visible) {
    this->_shouldBeVisible = visible;
    QAbstractButton::setVisible(visible);
}

void LinboPushButton::setVisibleAnimated(bool visible) {
    if(visible == this->_shouldBeVisible)
        return;

    if(!this->isVisible()) {
        // if the parent was hidden
        // -> Show it to be able to fade the overlays in
        for(LinboPushButtonOverlay* overlay : this->_overlays)
            overlay->setVisible(false);

        this->setVisible(true);
    }

    this->_shouldBeVisible = visible;

    if(!visible) {
        for(LinboPushButtonOverlay* overlay : this->_overlays)
            if(overlay->_managedAutomatically)
                overlay->setVisibleAnimated(false);

        // if the button is still supposed to be hidden after 400ms (the default animation duration)
        // -> hide the button to prevent it from overlaying other stuff
        QTimer::singleShot(400, this, [=] {
            if(!this->_shouldBeVisible)
                this->setVisible(false);
        });
    }
    else
        for(LinboPushButtonOverlay* overlay : this->_getOverlaysOfType(LinboPushButtonOverlay::Background))
            if(overlay->_managedAutomatically)
                overlay->setVisibleAnimated(true);
}

void LinboPushButton::setOverlayTypeMuted(LinboPushButtonOverlay::OverlayType overlayType, bool muted) {
    if(this->_mutedOverlayTypes.contains(overlayType) && !muted)
        this->_mutedOverlayTypes.removeAt(this->_mutedOverlayTypes.indexOf(overlayType));
    else if(!this->_mutedOverlayTypes.contains(overlayType) && muted)
        this->_mutedOverlayTypes.append(overlayType);

    this->repaint();
}

void LinboPushButton::paintEvent(QPaintEvent *e) {

    for(LinboPushButtonOverlay* overlay : this->_getOverlaysOfType(LinboPushButtonOverlay::OnChecked))
        overlay->setVisibleAnimated(this->isChecked() && !this->_overlayTypeIsMuted(LinboPushButtonOverlay::OnChecked));

    if(this->isEnabled() && this->_isHovered)
        for(LinboPushButtonOverlay* overlay : this->_getOverlaysOfType(LinboPushButtonOverlay::OnHover))
            overlay->setVisibleAnimated(!this->_overlayTypeIsMuted(LinboPushButtonOverlay::OnHover));
    else
        for(LinboPushButtonOverlay* overlay : this->_getOverlaysOfType(LinboPushButtonOverlay::OnHover))
            overlay->setVisibleAnimated(false);

    if(this->isEnabled() && this->_isFocused)
        for(LinboPushButtonOverlay* overlay : this->_getOverlaysOfType(LinboPushButtonOverlay::OnKeyboardFocus))
            overlay->setVisibleAnimated(!this->_overlayTypeIsMuted(LinboPushButtonOverlay::OnKeyboardFocus));
    else
        for(LinboPushButtonOverlay* overlay : this->_getOverlaysOfType(LinboPushButtonOverlay::OnKeyboardFocus))
            overlay->setVisibleAnimated(false);

    if(this->isEnabled() && this->_isPressed)
        for(LinboPushButtonOverlay* overlay : this->_getOverlaysOfType(LinboPushButtonOverlay::OnPressed))
            overlay->setVisibleAnimated(!this->_overlayTypeIsMuted(LinboPushButtonOverlay::OnPressed));
    else
        for(LinboPushButtonOverlay* overlay : this->_getOverlaysOfType(LinboPushButtonOverlay::OnPressed))
            overlay->setVisibleAnimated(false);

    QWidget::paintEvent(e);
}

void LinboPushButton::keyPressEvent(QKeyEvent *e) {
    if(e->key() == Qt::Key_Return)
        this->_isPressed = true;
    return QAbstractButton::keyPressEvent(e);
}

void LinboPushButton::keyReleaseEvent(QKeyEvent *e) {
    if(e->key() == Qt::Key_Return && this->_isPressed) {
        this->_isPressed = false;
        emit this->clicked();
    }
    return QAbstractButton::keyReleaseEvent(e);
}

void LinboPushButton::focusInEvent(QFocusEvent *e)  {
    if(e->reason() == Qt::ActiveWindowFocusReason)
        return QAbstractButton::focusInEvent(e);

    emit this->hovered();
    this->_isFocused = true;
    return QAbstractButton::focusInEvent(e);
}

void LinboPushButton::focusOutEvent(QFocusEvent *e)  {
    this->_isFocused = false;
    QAbstractButton::focusOutEvent(e);
    emit this->defocused(e->reason());
}

void LinboPushButton::enterEvent(QEnterEvent *e) {
    this->_isHovered = true;
    QAbstractButton::enterEvent(e);
    emit this->hovered();
}

void LinboPushButton::leaveEvent(QEvent *e) {
    this->_isHovered = false;
    QAbstractButton::leaveEvent(e);
}

void LinboPushButton::mousePressEvent(QMouseEvent *e) {
    this->_isPressed = true;
    QAbstractButton::mousePressEvent(e);
}

void LinboPushButton::mouseReleaseEvent(QMouseEvent *e) {
    this->_isPressed = false;
    QAbstractButton::mouseReleaseEvent(e);
}

void LinboPushButton::mouseDoubleClickEvent(QMouseEvent *e) {
    QAbstractButton::mouseDoubleClickEvent(e);
    emit this->doubleClicked();
}

QList<LinboPushButtonOverlay*> LinboPushButton::_getOverlaysOfType(LinboPushButtonOverlay::OverlayType type) {
    return this->_getOverlaysOfType(type, this->_overlays);
}

QList<LinboPushButtonOverlay*> LinboPushButton::_getOverlaysOfType(LinboPushButtonOverlay::OverlayType type, QList<LinboPushButtonOverlay*> overlays) {
    QList<LinboPushButtonOverlay*> filteredOverlays;

    for(LinboPushButtonOverlay* overlay : overlays) {
        if(overlay->_getType() == type)
            filteredOverlays.append(overlay);
    }

    return filteredOverlays;
}

bool LinboPushButton::_overlayTypeIsMuted(LinboPushButtonOverlay::OverlayType overlayType) {
    return this->_mutedOverlayTypes.contains(overlayType);
}
