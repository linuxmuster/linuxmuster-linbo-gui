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

#include "qmodernpushbuttonoverlay.h"

class QModernPushButton : public QAbstractButton
{
    Q_OBJECT
public:
    QModernPushButton(QString icon, QWidget* parent = nullptr);
    QModernPushButton(QString icon, QString label, QWidget* parent = nullptr);

    void setVisibleAnimated(bool visible);

    void setGeometryAnimated(const QRect& geometry);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    bool shouldBeVisible;
    QPropertyAnimation* geometryAnimation;
    QList<QModernPushButtonOverlay*> overlays;
    QSvgWidget* svgIcon;
    QSvgWidget* hoveredOverlay;
    QLabel *label;

private slots:
    void handleToggled(bool checked);

signals:
    void checked();
};

#endif // QMODERNPUSHBUTTON_H
