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

#ifndef QMODERNPROGRESSBAR_H
#define QMODERNPROGRESSBAR_H

#include <QProgressBar>
#include <QObject>
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QTimer>
#include <QPropertyAnimation>

class QModernProgressBar : public QProgressBar
{
    Q_OBJECT
public:
    QModernProgressBar(QWidget* parent = nullptr);

    void setIndeterminate(bool indeterminate);
    bool getIndeterminate();
    void setReversed(bool reversed);

protected:
    QTimer* refreshTimer;
    QPropertyAnimation* indeterminateAnimtion;

    void paintEvent(QPaintEvent *e) override;

private:
    bool indeterminate;
    bool reversed;
    int preIndeterminateValue;
    int preIndeterminateMinimum;
    int preIndeterminateMaximum;

protected slots:
    void updateIndeterminate();

};

#endif // QMODERNPROGRESSBAR_H
