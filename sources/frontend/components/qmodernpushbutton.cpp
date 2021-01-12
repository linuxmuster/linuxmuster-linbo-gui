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

#include "qmodernpushbutton.h"


QModernPushButton::QModernPushButton(QString icon, QWidget* parent) : QModernPushButton(icon, "", parent)
{
}

QModernPushButton::QModernPushButton(QString icon, QString label, QWidget* parent) : QModernPushButton(icon, label, {}, parent)
{
}

QModernPushButton::QModernPushButton(QString icon, QString label, QList<QModernPushButtonOverlay*> extraOverlays, QWidget* parent) : QAbstractButton(parent) {
    this->svgIcon = nullptr;
    this->label = nullptr;
    this->shouldBeVisible = true;
    this->isPressed = false;
    this->isHovered = false;

    this->setMouseTracking(true);

    this->geometryAnimation = new QPropertyAnimation(this, "geometry", this);
    this->geometryAnimation->setDuration(400);
    this->geometryAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    this->setObjectName(icon);

    // Background
    if(!icon.isEmpty()) {
        this->svgIcon = new QSvgWidget(icon);
        this->overlays.append(new QModernPushButtonOverlay(QModernPushButtonOverlay::Background, this->svgIcon, true));
    }

    this->label = new QLabel(label, this);
    this->label->setAlignment(Qt::AlignCenter);
    this->overlays.append(new QModernPushButtonOverlay(QModernPushButtonOverlay::Background, this->label, true));
    this->overlays.append(this->getOverlaysOfType(QModernPushButtonOverlay::Background, extraOverlays));

    // Hover
    this->hoveredOverlay = new QSvgWidget(":/icons/universal/overlayHovered.svg");
    this->overlays.append(
                new QModernPushButtonOverlay(
                    QModernPushButtonOverlay::OnHover,
                    this->hoveredOverlay,
                    true
                    )
                );

    this->overlays.append(this->getOverlaysOfType(QModernPushButtonOverlay::OnHover, extraOverlays));

    // Pressed
    this->overlays.append(
                new QModernPushButtonOverlay(
                    QModernPushButtonOverlay::OnPressed,
                    new QSvgWidget(":/icons/universal/overlayPressed.svg"),
                    true
                    )
                );

    this->overlays.append(this->getOverlaysOfType(QModernPushButtonOverlay::OnPressed, extraOverlays));

    // Checked
    this->overlays.append(this->getOverlaysOfType(QModernPushButtonOverlay::OnChecked, extraOverlays));

    // set defaults
    for(QModernPushButtonOverlay* overlay : this->overlays) {
        overlay->setParent(this);
        overlay->widget->setParent(this);
    }

    for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::Background)) {
        if(overlay->managedAutomatically)
            overlay->setVisibleAnimated(true);
        overlay->setAnimationDuration(200);
    }

    for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::OnHover)) {
        overlay->setAnimationDuration(200);
    }

    for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::OnPressed)) {
        overlay->setAnimationDuration(100);
    }

    connect(this, SIGNAL(toggled(bool)), this, SLOT(handleToggled(bool)));

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void QModernPushButton::handleToggled(bool checked) {
    if(checked)
        emit this->checked();
}

void QModernPushButton::resizeEvent(QResizeEvent *event) {
    QAbstractButton::resizeEvent(event);

    QList<QWidget*> doNotResizeWidgets;

    if(this->label != nullptr && !this->label->text().isEmpty()) {

        this->label->setFixedHeight(this->height());

        QFont font = this->label->font();
        font.setPixelSize(int(this->height() * 0.5) <= 0 ? 1 : this->height() * 0.5);
        this->label->setFont(font);

        if(this->svgIcon != nullptr) {
            this->svgIcon->setGeometry(QRect(0, 0, this->height(), this->height()));

            this->label->move(this->height() * 1.1, 0);
            this->label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            this->label->adjustSize();
            this->setMinimumWidth((this->label->x() + this->label->sizeHint().width()) * 1.1);

            doNotResizeWidgets.append(this->svgIcon);
            doNotResizeWidgets.append(this->label);
        }
        else {
            this->label->setAlignment(Qt::AlignCenter);
        }
    }

    for(QModernPushButtonOverlay* overlay : this->overlays) {
        if(doNotResizeWidgets.contains(overlay->widget))
            continue;

        overlay->widget->setGeometry(QRect(0, 0, this->width(), this->height()));
    }
}

void QModernPushButton::setGeometryAnimated(const QRect& geometry) {
    this->geometryAnimation->setStartValue(this->geometry());
    this->geometryAnimation->setEndValue(geometry);
    this->geometryAnimation->start();
}

void QModernPushButton::setVisible(bool visible) {
    this->shouldBeVisible = visible;
    QAbstractButton::setVisible(visible);
}

void QModernPushButton::setVisibleAnimated(bool visible) {
    if(visible == this->shouldBeVisible)
        return;

    if(!this->isVisible()) {
        // if the parent was hidden
        // -> Show it to be able to fade the overlays in
        for(QModernPushButtonOverlay* overlay : this->overlays)
            overlay->setVisible(false);

        this->setVisible(true);
    }

    this->shouldBeVisible = visible;

    if(!visible) {
        for(QModernPushButtonOverlay* overlay : this->overlays)
            if(overlay->managedAutomatically)
                overlay->setVisibleAnimated(false);

        // if the button is still supposed to be hidden after 400ms (the default animation duration)
        // -> hide the button to prevent it from overlaying other stuff
        QTimer::singleShot(400, [=]{
            if(!this->shouldBeVisible)
                this->setVisible(false);
        });
    }
    else
        for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::Background))
            if(overlay->managedAutomatically)
                overlay->setVisibleAnimated(true);
}

void QModernPushButton::setOverlayTypeMuted(QModernPushButtonOverlay::OverlayType overlayType, bool muted) {
    if(this->mutedOverlayTypes.contains(overlayType) && !muted)
        this->mutedOverlayTypes.removeAt(this->mutedOverlayTypes.indexOf(overlayType));
    else if(!this->mutedOverlayTypes.contains(overlayType) && muted)
        this->mutedOverlayTypes.append(overlayType);

    this->repaint();
}

void QModernPushButton::paintEvent(QPaintEvent *e) {

    for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::OnChecked))
        overlay->setVisibleAnimated(this->isChecked() && !this->overlayTypeIsMuted(QModernPushButtonOverlay::OnChecked));

    if(this->isEnabled() && this->isHovered)
        for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::OnHover))
            overlay->setVisibleAnimated(!this->overlayTypeIsMuted(QModernPushButtonOverlay::OnHover));
    else
        for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::OnHover))
            overlay->setVisibleAnimated(false);

    if(this->isEnabled() && this->isPressed)
        for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::OnPressed))
            overlay->setVisibleAnimated(!this->overlayTypeIsMuted(QModernPushButtonOverlay::OnPressed));
    else
        for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::OnPressed))
            overlay->setVisibleAnimated(false);

    QWidget::paintEvent(e);
}

void QModernPushButton::keyPressEvent(QKeyEvent *e) {
    // TODO
    return QAbstractButton::keyPressEvent(e);
}

void QModernPushButton::keyReleaseEvent(QKeyEvent *e) {
    // TODO
    return QAbstractButton::keyReleaseEvent(e);
}

void QModernPushButton::enterEvent(QEvent *e) {
    this->isHovered = true;
    QAbstractButton::enterEvent(e);
    emit this->hovered();
}

void QModernPushButton::leaveEvent(QEvent *e) {
    this->isHovered = false;
    QAbstractButton::leaveEvent(e);
}

void QModernPushButton::mousePressEvent(QMouseEvent *e) {
    this->isPressed = true;
    QAbstractButton::mousePressEvent(e);
}

void QModernPushButton::mouseReleaseEvent(QMouseEvent *e) {
    this->isPressed = false;
    QAbstractButton::mouseReleaseEvent(e);
}

void QModernPushButton::mouseDoubleClickEvent(QMouseEvent *e) {
    QAbstractButton::mouseDoubleClickEvent(e);
    emit this->doubleClicked();
}

QList<QModernPushButtonOverlay*> QModernPushButton::getOverlaysOfType(QModernPushButtonOverlay::OverlayType type) {
    return this->getOverlaysOfType(type, this->overlays);
}

QList<QModernPushButtonOverlay*> QModernPushButton::getOverlaysOfType(QModernPushButtonOverlay::OverlayType type, QList<QModernPushButtonOverlay*> overlays) {
    QList<QModernPushButtonOverlay*> filteredOverlays;

    for(QModernPushButtonOverlay* overlay : overlays) {
        if(overlay->getType() == type)
            filteredOverlays.append(overlay);
    }

    return filteredOverlays;
}

bool QModernPushButton::overlayTypeIsMuted(QModernPushButtonOverlay::OverlayType overlayType) {
    return this->mutedOverlayTypes.contains(overlayType);
}
