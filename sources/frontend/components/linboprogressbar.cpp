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

#include "linboprogressbar.h"

LinboProgressBar::LinboProgressBar(QWidget* parent) : QProgressBar(parent)
{
    this->_refreshTimer = new QTimer();
    this->_refreshTimer->setSingleShot(false);
    this->_refreshTimer->setInterval(400);
    connect(this->_refreshTimer, &QTimer::timeout, this, &LinboProgressBar::updateIndeterminate);


    this->_indeterminateAnimtion = new QPropertyAnimation(this, "value");
    this->_indeterminateAnimtion->setDuration(2000);
    this->_indeterminateAnimtion->setStartValue(0);
    this->_indeterminateAnimtion->setEndValue(1000);
    this->_indeterminateAnimtion->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    this->_indeterminateAnimtion->setLoopCount(-1);

    this->setValue(0);
}

void LinboProgressBar::setIndeterminate(bool indeterminate) {
    if(this->_indeterminate == indeterminate)
        return;

    this->_indeterminate = indeterminate;

    if(this->_indeterminate) {
        this->_preIndeterminateValue = this->value();
        this->_preIndeterminateMinimum = this->minimum();
        this->_preIndeterminateMaximum = this->maximum();
        // finer steps, so the Animation is fluid
        this->setMinimum(0);
        this->setMaximum(1000);
        this->_indeterminateAnimtion->start();
    }
    else {
        // reset minimum and maximum
        this->setMinimum(this->_preIndeterminateMinimum);
        this->setMaximum(this->_preIndeterminateMaximum);
        this->setValue(this->_preIndeterminateValue);
        this->_indeterminateAnimtion->stop();
    }
}

void LinboProgressBar::setReversed(bool reversed) {
    if(this->_reversed == reversed)
        return;

    this->_reversed = reversed;
    this->update();
}

bool LinboProgressBar::getIndeterminate() {
    return this->_indeterminate;
}

void LinboProgressBar::updateIndeterminate() {
    qDebug() << "update indeterminate";
}

void LinboProgressBar::paintEvent(QPaintEvent *e) {
    QPainter painter;
    painter.begin(this);
    // background
    painter.fillRect(e->rect(), gTheme->color(LinboTheme::ElevatedBackgroundColor));

    double from = 0;
    double to = 0;

    // progress
    if(this->_indeterminate) {
        int maximum = this->maximum() / 2;
        if(this->value() <= maximum)
            // for the first half -> fill from left
            to = double(double(this->value()) / double(maximum));
        else {
            // for the second half -> empty from right
            from = double(double(this->value()- (maximum)) / double(maximum));
            to = 1;
        }
    }
    else {
        to = double(double(this->value()) / double(this->maximum()));
    }

    if(this->_reversed) {
        // if reversed -> reverse and swap from and to
        double tmp = 1 - from;
        from = 1 - to;
        to = tmp;

    }

    painter.fillRect(QRect(e->rect().width() * from, 0, e->rect().width() * to, e->rect().height()), gTheme->color(LinboTheme::AccentColor));

    painter.end();

    QWidget::paintEvent(e);
}

