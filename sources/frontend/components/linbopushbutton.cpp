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
    this->svgIcon = nullptr;
    this->label = nullptr;
    this->shouldBeVisible = true;
    this->isPressed = false;
    this->isHovered = false;
    this->isFocused = false;

    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::TabFocus);

    this->geometryAnimation = new QPropertyAnimation(this, "geometry", this);
    this->geometryAnimation->setDuration(400);
    this->geometryAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    this->setObjectName(icon);

    // Background
    if(!icon.isEmpty()) {
        this->svgIcon = new QSvgWidget(icon);
        this->overlays.append(new LinboPushButtonOverlay(LinboPushButtonOverlay::Background, this->svgIcon, true));
    }

    this->label = new QLabel(label, this);
    this->label->setAlignment(Qt::AlignCenter);
    this->overlays.append(new LinboPushButtonOverlay(LinboPushButtonOverlay::Background, this->label, true));
    this->overlays.append(this->getOverlaysOfType(LinboPushButtonOverlay::Background, extraOverlays));

    // Hover
    this->hoveredOverlay = new QSvgWidget(gTheme->getIconPath(LinboTheme::OverlayHoveredIcon));
    this->overlays.append(
        new LinboPushButtonOverlay(
            LinboPushButtonOverlay::OnHover,
            this->hoveredOverlay,
            true
        )
    );

    this->overlays.append(this->getOverlaysOfType(LinboPushButtonOverlay::OnHover, extraOverlays));

    // Pressed
    this->overlays.append(
        new LinboPushButtonOverlay(
            LinboPushButtonOverlay::OnPressed,
            new QSvgWidget(gTheme->getIconPath(LinboTheme::OverlayPressedIcon)),
            true
        )
    );

    this->overlays.append(this->getOverlaysOfType(LinboPushButtonOverlay::OnPressed, extraOverlays));

    // Checked
    this->overlays.append(this->getOverlaysOfType(LinboPushButtonOverlay::OnChecked, extraOverlays));

    // KeyboardFocus
    QSvgWidget* keyboardFocusOverlay = new QSvgWidget(gTheme->getIconPath(LinboTheme::OverlayKeyboardFocusIcon));
    this->overlays.append(
    new LinboPushButtonOverlay {
        LinboPushButtonOverlay::OnKeyboardFocus,
        keyboardFocusOverlay,
        true
    }
    );

    // set defaults
    for(LinboPushButtonOverlay* overlay : this->overlays) {
        overlay->setParent(this);
        overlay->widget->setParent(this);
    }

    for(LinboPushButtonOverlay* overlay : this->getOverlaysOfType(LinboPushButtonOverlay::Background)) {
        if(overlay->managedAutomatically)
            overlay->setVisibleAnimated(true);
        overlay->setAnimationDuration(200);
    }

    for(LinboPushButtonOverlay* overlay : this->getOverlaysOfType(LinboPushButtonOverlay::OnHover)) {
        overlay->setAnimationDuration(200);
    }

    for(LinboPushButtonOverlay* overlay : this->getOverlaysOfType(LinboPushButtonOverlay::OnPressed)) {
        overlay->setAnimationDuration(100);
    }

    connect(this, SIGNAL(toggled(bool)), this, SLOT(handleToggled(bool)));
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void LinboPushButton::handleToggled(bool checked) {
    if(checked)
        emit this->checked();
}

void LinboPushButton::resizeEvent(QResizeEvent *event) {
    QAbstractButton::resizeEvent(event);

    QList<QWidget*> doNotResizeWidgets;

    if(this->label != nullptr && !this->label->text().isEmpty()) {

        this->label->setFixedHeight(this->height());

        QFont font = this->label->font();
        font.setPixelSize(gTheme->toFontSize(this->height() * 0.5));
        this->label->setFont(font);

        if(this->svgIcon != nullptr) {
            this->svgIcon->setGeometry(QRect(0, 0, this->height(), this->height()));

            this->label->move(this->height() * 1.1, 0);
            this->label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            doNotResizeWidgets.append(this->svgIcon);
            doNotResizeWidgets.append(this->label);
        }
        else {
            this->label->setAlignment(Qt::AlignCenter);
        }

        this->label->adjustSize();
        this->setMinimumWidth((this->label->x() + this->label->sizeHint().width()) * 1.1);
    }

    for(LinboPushButtonOverlay* overlay : this->overlays) {
        if(doNotResizeWidgets.contains(overlay->widget))
            continue;

        overlay->widget->setGeometry(QRect(0, 0, this->width(), this->height()));
    }
}

void LinboPushButton::setGeometryAnimated(const QRect& geometry) {
    this->geometryAnimation->setStartValue(this->geometry());
    this->geometryAnimation->setEndValue(geometry);
    this->geometryAnimation->start();
}

void LinboPushButton::setVisible(bool visible) {
    this->shouldBeVisible = visible;
    QAbstractButton::setVisible(visible);
}

void LinboPushButton::setVisibleAnimated(bool visible) {
    if(visible == this->shouldBeVisible)
        return;

    if(!this->isVisible()) {
        // if the parent was hidden
        // -> Show it to be able to fade the overlays in
        for(LinboPushButtonOverlay* overlay : this->overlays)
            overlay->setVisible(false);

        this->setVisible(true);
    }

    this->shouldBeVisible = visible;

    if(!visible) {
        for(LinboPushButtonOverlay* overlay : this->overlays)
            if(overlay->managedAutomatically)
                overlay->setVisibleAnimated(false);

        // if the button is still supposed to be hidden after 400ms (the default animation duration)
        // -> hide the button to prevent it from overlaying other stuff
        QTimer::singleShot(400, [=] {
            if(!this->shouldBeVisible)
                this->setVisible(false);
        });
    }
    else
        for(LinboPushButtonOverlay* overlay : this->getOverlaysOfType(LinboPushButtonOverlay::Background))
            if(overlay->managedAutomatically)
                overlay->setVisibleAnimated(true);
}

void LinboPushButton::setOverlayTypeMuted(LinboPushButtonOverlay::OverlayType overlayType, bool muted) {
    if(this->mutedOverlayTypes.contains(overlayType) && !muted)
        this->mutedOverlayTypes.removeAt(this->mutedOverlayTypes.indexOf(overlayType));
    else if(!this->mutedOverlayTypes.contains(overlayType) && muted)
        this->mutedOverlayTypes.append(overlayType);

    this->repaint();
}

void LinboPushButton::paintEvent(QPaintEvent *e) {

    for(LinboPushButtonOverlay* overlay : this->getOverlaysOfType(LinboPushButtonOverlay::OnChecked))
        overlay->setVisibleAnimated(this->isChecked() && !this->overlayTypeIsMuted(LinboPushButtonOverlay::OnChecked));

    if(this->isEnabled() && this->isHovered)
        for(LinboPushButtonOverlay* overlay : this->getOverlaysOfType(LinboPushButtonOverlay::OnHover))
            overlay->setVisibleAnimated(!this->overlayTypeIsMuted(LinboPushButtonOverlay::OnHover));
    else
        for(LinboPushButtonOverlay* overlay : this->getOverlaysOfType(LinboPushButtonOverlay::OnHover))
            overlay->setVisibleAnimated(false);

    if(this->isEnabled() && this->isFocused)
        for(LinboPushButtonOverlay* overlay : this->getOverlaysOfType(LinboPushButtonOverlay::OnKeyboardFocus))
            overlay->setVisibleAnimated(!this->overlayTypeIsMuted(LinboPushButtonOverlay::OnKeyboardFocus));
    else
        for(LinboPushButtonOverlay* overlay : this->getOverlaysOfType(LinboPushButtonOverlay::OnKeyboardFocus))
            overlay->setVisibleAnimated(false);

    if(this->isEnabled() && this->isPressed)
        for(LinboPushButtonOverlay* overlay : this->getOverlaysOfType(LinboPushButtonOverlay::OnPressed))
            overlay->setVisibleAnimated(!this->overlayTypeIsMuted(LinboPushButtonOverlay::OnPressed));
    else
        for(LinboPushButtonOverlay* overlay : this->getOverlaysOfType(LinboPushButtonOverlay::OnPressed))
            overlay->setVisibleAnimated(false);

    QWidget::paintEvent(e);
}

void LinboPushButton::keyPressEvent(QKeyEvent *e) {
    if(e->key() == Qt::Key_Return)
        this->isPressed = true;
    return QAbstractButton::keyPressEvent(e);
}

void LinboPushButton::keyReleaseEvent(QKeyEvent *e) {
    if(e->key() == Qt::Key_Return && this->isPressed) {
        this->isPressed = false;
        emit this->clicked();
    }
    return QAbstractButton::keyReleaseEvent(e);
}

void LinboPushButton::focusInEvent(QFocusEvent *e)  {
    if(e->reason() == Qt::ActiveWindowFocusReason)
        return QAbstractButton::focusInEvent(e);

    emit this->hovered();
    this->isFocused = true;
    return QAbstractButton::focusInEvent(e);
}

void LinboPushButton::focusOutEvent(QFocusEvent *e)  {
    this->isFocused = false;
    QAbstractButton::focusOutEvent(e);
    emit this->defocused(e->reason());
}

void LinboPushButton::enterEvent(QEvent *e) {
    this->isHovered = true;
    QAbstractButton::enterEvent(e);
    emit this->hovered();
}

void LinboPushButton::leaveEvent(QEvent *e) {
    this->isHovered = false;
    QAbstractButton::leaveEvent(e);
}

void LinboPushButton::mousePressEvent(QMouseEvent *e) {
    this->isPressed = true;
    QAbstractButton::mousePressEvent(e);
}

void LinboPushButton::mouseReleaseEvent(QMouseEvent *e) {
    this->isPressed = false;
    QAbstractButton::mouseReleaseEvent(e);
}

void LinboPushButton::mouseDoubleClickEvent(QMouseEvent *e) {
    QAbstractButton::mouseDoubleClickEvent(e);
    emit this->doubleClicked();
}

QList<LinboPushButtonOverlay*> LinboPushButton::getOverlaysOfType(LinboPushButtonOverlay::OverlayType type) {
    return this->getOverlaysOfType(type, this->overlays);
}

QList<LinboPushButtonOverlay*> LinboPushButton::getOverlaysOfType(LinboPushButtonOverlay::OverlayType type, QList<LinboPushButtonOverlay*> overlays) {
    QList<LinboPushButtonOverlay*> filteredOverlays;

    for(LinboPushButtonOverlay* overlay : overlays) {
        if(overlay->getType() == type)
            filteredOverlays.append(overlay);
    }

    return filteredOverlays;
}

bool LinboPushButton::overlayTypeIsMuted(LinboPushButtonOverlay::OverlayType overlayType) {
    return this->mutedOverlayTypes.contains(overlayType);
}
