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
#include <QMap>
#include <QPair>
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

public:
    LinboDialog(QWidget* parent);

    virtual void setTitle(QString title);
    virtual QString title();

    virtual void centerInParent();

protected:
    void resizeEvent(QResizeEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    void keyPressEvent(QKeyEvent *ev) override;
    virtual void setVisibleAnimated(bool visible);
    virtual bool isFrameless();
    void addToolButton(LinboToolButton* toolButton);

private:
    struct _AnimatedWidget {
        bool isValid;
        QWidget* widget;
        QGraphicsOpacityEffect* effect;
        QPropertyAnimation* animation;
    };
    typedef QList<_AnimatedWidget> AnimatedWidgets;

    bool _busy;
    QString _title;

    ModalOverlay* _modalOverlayWidget;

    AnimatedWidgets _animatedWidgets;

    QRect _originalGeometry;

    QWidget* _toolBarWidget;
    QHBoxLayout* _toolBarLayout;
    QLabel* _titleLabel;
    LinboToolButton* _closeButton;

    QWidget* _bottomToolBarWidget;
    QHBoxLayout* _bottomToolBarLayout;
    QList<LinboToolButton*> _toolButtons;

    QWidget* _firstChild;

    void _initColors();
    void _initModalWidget();
    void _initOpacityEffectForWidget(QWidget* widget);
    void _initToolbar();
    void _initBottomToolbar();

    void _updateToolbarsEnabled();

    void _setAllAnimatedWidgetsVisible(bool visible);
    void _setAnimatedWidgetVisible(bool visible, _AnimatedWidget widget);
    void _hideAnimatedWidget(_AnimatedWidget widget);
    void _showAnimatedWidget(_AnimatedWidget widget);
    _AnimatedWidget _findAnimatedWidget(QPropertyAnimation* animation);

public slots:
    void open();
    void close();
    void autoClose();

private slots:
    void _handleAnimationFinished();

signals:
    void opened();
    void closedByUser();
};

class ModalOverlay : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged)

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

    static constexpr QColor _INVISIBLE_COLOR = QColor(0,0,0,0);
    static constexpr QColor _VISIBLE_COLOR = QColor(0,0,0,66);

signals:
    void clicked();
    void colorChanged(QColor color);
};

#endif // QMODERNDIALOG_H
