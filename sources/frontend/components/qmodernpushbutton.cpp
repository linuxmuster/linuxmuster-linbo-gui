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

    this->label = new QLabel(label);
    this->label->setAlignment(Qt::AlignCenter);
    this->overlays.append(new QModernPushButtonOverlay(QModernPushButtonOverlay::Background, this->label, true));
    this->overlays.append(this->getOverlaysOfType(QModernPushButtonOverlay::Background, extraOverlays));

    // Hover
    this->hoveredOverlay = new QSvgWidget(":svgIcons/overlayHovered.svg");
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
                    new QSvgWidget(":svgIcons/overlayPressed.svg"),
                    true
                    )
                );

    this->overlays.append(this->getOverlaysOfType(QModernPushButtonOverlay::OnPressed, extraOverlays));

    // Checked
    this->overlays.append(
                new QModernPushButtonOverlay(
                    QModernPushButtonOverlay::OnChecked,
                    new QSvgWidget(":svgIcons/overlayChecked.svg"),
                    true
                    )
                );

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

    for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::OnChecked)) {
        overlay->setVisibleAnimated(checked);
    }
}

void QModernPushButton::resizeEvent(QResizeEvent *event) {
    // make border width match button size
    QFile file(":svgIcons/overlayHovered.svg");
    file.open(QFile::ReadOnly);
    QString overlayHoveredString = file.readAll();
    QString width = QString::number(this->width());
    QString height = QString::number(this->height());
    overlayHoveredString.replace("width=\"500px\"", "width=\"" + width + "px\"");
    overlayHoveredString.replace("height=\"500px\"", "height=\"" + height + "px\"");
    overlayHoveredString.replace("viewBox=\"0 0 500px 500px\"", "viewBox=\"0 0 " + width + "px " + height + "px\"");
    overlayHoveredString.replace("stroke-width:15px", "stroke-width:2px");
    this->hoveredOverlay->load(overlayHoveredString.toUtf8());

    for(QModernPushButtonOverlay* overlay : this->overlays) {
        overlay->widget->setGeometry(QRect(0, 0, event->size().width(), event->size().height()));
    }

    if(this->label != nullptr && this->svgIcon != nullptr) {
        // place icon besides text
        this->svgIcon->setGeometry(QRect(0, 0, event->size().height(), event->size().height()));
        this->label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        this->label->setGeometry(QRect(event->size().height() * 1.1, 0, event->size().width() - event->size().height(), event->size().height()));
    }
    if(this->label != nullptr) {
        QFont font = this->label->font();
        font.setPixelSize(int(this->height() * 0.5) <= 0 ? 1 : this->height() * 0.5);
        this->label->setFont(font);
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

void QModernPushButton::paintEvent(QPaintEvent *e) {
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

    if(this->isEnabled())
        for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::OnHover))
            overlay->setVisibleAnimated(true);

    return QAbstractButton::enterEvent(e);
}

void QModernPushButton::leaveEvent(QEvent *e) {
        for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::OnHover))
            overlay->setVisibleAnimated(false);

    return QAbstractButton::leaveEvent(e);
}

void QModernPushButton::mousePressEvent(QMouseEvent *e) {
    if(this->isEnabled())
        for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::OnPressed))
            overlay->setVisibleAnimated(true);

    return QAbstractButton::mousePressEvent(e);
}

void QModernPushButton::mouseReleaseEvent(QMouseEvent *e) {
    for(QModernPushButtonOverlay* overlay : this->getOverlaysOfType(QModernPushButtonOverlay::OnPressed))
        overlay->setVisibleAnimated(false);

    return QAbstractButton::mouseReleaseEvent(e);
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
