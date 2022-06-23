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

#ifndef QMODERNPROGRESSBAR_H
#define QMODERNPROGRESSBAR_H

#include <QProgressBar>
#include <QObject>
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QTimer>
#include <QPropertyAnimation>

#include "linboguitheme.h"

class LinboProgressBar : public QProgressBar
{
    Q_OBJECT
public:
    LinboProgressBar(QWidget* parent = nullptr);

    void setIndeterminate(bool indeterminate);
    bool indeterminate();
    void setReversed(bool reversed);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    struct _FromTo {
        double from = 0.0;
        double to = 0.0;
    };

    bool _indeterminate;
    bool _reversed;
    int _preIndeterminateValue;
    int _preIndeterminateMinimum;
    int _preIndeterminateMaximum;
    QPropertyAnimation* _indeterminateAnimtion;

    void _setIndeterminate();
    void _setDeterminate();
    _FromTo _calculateFromTo();
    _FromTo _calculateFromToIndeterminate();
    _FromTo _calculateFromToDeterminate();
    _FromTo _reverseFromTo(_FromTo values);
    void _paint(QPaintEvent* e, _FromTo values);

};

#endif // QMODERNPROGRESSBAR_H
