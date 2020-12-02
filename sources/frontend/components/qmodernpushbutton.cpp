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


QModernPushButton::QModernPushButton(QString icon, QWidget* parent) : QModernPushButton(icon, "", parent) {

}

QModernPushButton::QModernPushButton(QString icon, QString label, QWidget* parent) : QAbstractButton(parent)
{
    this->svgIcon = nullptr;
    this->label = nullptr;

    this->setMouseTracking(true);
    this->geometryAnimation = new QPropertyAnimation(this, "geometry", this);
    this->geometryAnimation->setDuration(400);
    this->geometryAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    QStringList overlays;
    overlays.append(icon);
    overlays.append(label);
    overlays.append(":svgIcons/overlayHovered.svg");
    overlays.append(":svgIcons/overlayPressed.svg");
    overlays.append(":svgIcons/overlayChecked.svg");

    this->setObjectName(icon);

    for(QString overlayName : overlays) {
        if(overlayName.isEmpty())
            continue;

        QWidget* overlayWidget;

        if(overlayName.startsWith(":") || overlayName.startsWith("/")) {
            QSvgWidget* svgWidget = new QSvgWidget(overlayName, this);
            if(overlayName == icon)
                this->svgIcon = svgWidget;
            else if(overlayName.contains("overlayHovered"))
                this->hoveredOverlay = svgWidget;
            overlayWidget = svgWidget;
        }
        else {
            QLabel* labelWidget = new QLabel(overlayName, this);
            labelWidget->setAlignment(Qt::AlignCenter);
            if(overlayName == label)
                this->label = labelWidget;
            overlayWidget = labelWidget;
        }

        QModernPushButtonOverlay* overlay = new QModernPushButtonOverlay(overlayWidget, this);
        this->overlays.append(overlay);
    }

    this->overlays[0]->setVisibleAnimated(true);
    this->overlays[0]->setAnimationDuration(200);
    if(this->overlays.length() == 5) {
        this->overlays[1]->setVisibleAnimated(true);
        this->overlays[1]->setAnimationDuration(200);
    }
    this->overlays[this->overlays.length()-3]->setAnimationDuration(200);
    this->overlays[this->overlays.length()-2]->setAnimationDuration(100);

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
        font.setPixelSize(this->height() * 0.5);
        this->label->setFont(font);
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
        this->overlays[this->overlays.length()-3]->setVisibleAnimated(true);
    return QAbstractButton::enterEvent(e);
}

void QModernPushButton::leaveEvent(QEvent *e) {
    if(this->overlays.length() >= 2)
        this->overlays[this->overlays.length()-3]->setVisibleAnimated(false);
    return QAbstractButton::leaveEvent(e);
}

void QModernPushButton::mousePressEvent(QMouseEvent *e) {
    if(this->overlays.length() >= 3 && this->isEnabled())
        this->overlays[this->overlays.length()-2]->setVisibleAnimated(true);
    return QAbstractButton::mousePressEvent(e);
}

void QModernPushButton::mouseReleaseEvent(QMouseEvent *e) {
    if(this->overlays.length() >= 3)
        this->overlays[this->overlays.length()-2]->setVisibleAnimated(false);
    return QAbstractButton::mouseReleaseEvent(e);
}
