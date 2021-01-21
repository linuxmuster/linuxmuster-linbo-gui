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

#ifndef QMODERNDIALOG_H
#define QMODERNDIALOG_H

#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QtDebug>
#include <QHBoxLayout>

#include "linboguitheme.h"
#include "linbotoolbutton.h"

class ModalOverlay;

class LinboDialog : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double scale READ getScale WRITE setScale)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle)

public:
    LinboDialog(QWidget* parent);

    virtual void setTitle(QString title);
    virtual QString getTitle();

    virtual void centerInParent();

protected:
    void resizeEvent(QResizeEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    void keyPressEvent(QKeyEvent *ev) override;
    virtual void setVisibleAnimated(bool visible);
    virtual bool isFrameless();
    void addToolButton(LinboToolButton* toolButton);

private:
    double scale;
    bool busy;
    QString title;

    ModalOverlay* modalOverlayWidget;

    QGraphicsOpacityEffect* opacityEffect;
    QPropertyAnimation* opacityEffectAnimation;

    QRect originalGeometry;

    QWidget* toolBarWidget;
    QHBoxLayout* toolBarLayout;
    QLabel* titleLabel;
    LinboToolButton* closeButton;
    QGraphicsOpacityEffect* toolBarOpacityEffect;
    QPropertyAnimation* toolBarOopacityEffectAnimation;

    QWidget* bottomToolBarWidget;
    QHBoxLayout* bottomToolBarLayout;
    QGraphicsOpacityEffect* bottomToolBarOpacityEffect;
    QPropertyAnimation* bottomToolBarOopacityEffectAnimation;
    QList<LinboToolButton*> toolButtons;

    QWidget* firstChild;

public slots:
    void open();
    void close();
    void autoClose();

    void setScale(double scale);
    double getScale();

private slots:
    void animationFinished();

signals:
    void opened();
    void closedByUser();
};

class ModalOverlay : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor color READ getColor WRITE setColor)

public:
    friend class LinboDialog;

protected:
    void mouseReleaseEvent (QMouseEvent* event) override;

private:
    ModalOverlay(QWidget* parent);

    QColor getColor();
    void setColor(QColor color);

    void setVisibleAnimated(bool visible);

    QPropertyAnimation* opacityAnimation;
    QColor color;

signals:
    void clicked();
};

#endif // QMODERNDIALOG_H
