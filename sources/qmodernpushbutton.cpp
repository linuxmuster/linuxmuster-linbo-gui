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

QModernPushButton::QModernPushButton(QString icon, QWidget* parent) : QAbstractButton(parent)
{
    this->setMouseTracking(true);
    this->geometryAnimation = new QPropertyAnimation(this, "geometry", this);
    this->geometryAnimation->setDuration(400);
    this->geometryAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    QStringList overlays;
    overlays.append(icon);
    overlays.append(":svgIcons/overlayHovered.svg");
    overlays.append(":svgIcons/overlayPressed.svg");
    overlays.append(":svgIcons/overlayChecked.svg");

    this->setObjectName(icon);

    for(QString overlayName : overlays) {

        QSvgWidget* overlayWidget = new QSvgWidget(this);
        overlayWidget->load(overlayName);

        QModernPushButtonOverlay* overlay = new QModernPushButtonOverlay(overlayWidget, this);
        this->overlays.append(overlay);
    }

    this->overlays[0]->setVisibleAnimated(true);
    this->overlays[0]->setAnimationDuration(200);
    this->overlays[1]->setAnimationDuration(200);
    this->overlays[2]->setAnimationDuration(100);

    connect(this, SIGNAL(toggled(bool)), this, SLOT(handleToggled(bool)));
}

void QModernPushButton::handleToggled(bool checked) {
    if(checked)
        emit this->checked();
    if(this->overlays.length() >= 4)
        this->overlays[3]->setVisibleAnimated(checked);
}

void QModernPushButton::resizeEvent(QResizeEvent *event) {
    // make border width match button size
    //QFile file(":svgIcons/overlayNormal.svg");
    //file.open(QFile::ReadOnly);
    //QString overlayNormalString = file.readAll();
    //overlayNormalString.replace("stroke-width:15px", "stroke-width:" + QString::number(500/event->size().width() * 5) + "px");
    //this->overlayHoveredWidget->load(overlayNormalString.toUtf8());

    for(QModernPushButtonOverlay* overlay : this->overlays) {
        overlay->widget->setGeometry(QRect(0, 0, event->size().width(), event->size().height()));
    }
}

void QModernPushButton::setGeometryAnimated(const QRect& geometry) {
    this->geometryAnimation->setStartValue(this->geometry());
    this->geometryAnimation->setEndValue(geometry);
    this->geometryAnimation->start();
}

void QModernPushButton::setVisibleAnimated(bool visible) {
    if(!this->isVisible()) {
        // if the parent was hidden
        // -> Show it to be able to fade the overlays in
        // -> Hide overlays to prevent them from appearing without an animation!
        this->setVisible(true);
        for(QModernPushButtonOverlay* overlay : this->overlays)
            overlay->setVisible(false);
    }

    this->setEnabled(visible);

    if(!visible)
        for(QModernPushButtonOverlay* overlay : this->overlays)
            overlay->setVisibleAnimated(false);
    else
        this->overlays[0]->setVisible(true);
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

    if(this->overlays.length() >= 2 && this->isEnabled())
        this->overlays[1]->setVisibleAnimated(true);
    return QAbstractButton::enterEvent(e);
}

void QModernPushButton::leaveEvent(QEvent *e) {
    if(this->overlays.length() >= 2)
        this->overlays[1]->setVisibleAnimated(false);
    return QAbstractButton::leaveEvent(e);
}

void QModernPushButton::mousePressEvent(QMouseEvent *e) {
    if(this->overlays.length() >= 3 && this->isEnabled())
        this->overlays[2]->setVisibleAnimated(true);
    return QAbstractButton::mousePressEvent(e);
}

void QModernPushButton::mouseReleaseEvent(QMouseEvent *e) {
    if(this->overlays.length() >= 3)
        this->overlays[2]->setVisibleAnimated(false);
    return QAbstractButton::mouseReleaseEvent(e);
}
