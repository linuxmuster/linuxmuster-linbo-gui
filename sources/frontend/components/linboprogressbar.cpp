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
    this->refreshTimer = new QTimer();
    this->refreshTimer->setSingleShot(false);
    this->refreshTimer->setInterval(400);
    connect(this->refreshTimer, SIGNAL(timeout()), this, SLOT(updateIndeterminate()));


    this->indeterminateAnimtion = new QPropertyAnimation(this, "value");
    this->indeterminateAnimtion->setDuration(2000);
    this->indeterminateAnimtion->setStartValue(0);
    this->indeterminateAnimtion->setEndValue(1000);
    this->indeterminateAnimtion->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    this->indeterminateAnimtion->setLoopCount(-1);

    this->setValue(0);
}

void LinboProgressBar::setIndeterminate(bool indeterminate) {
    if(this->indeterminate == indeterminate)
        return;

    this->indeterminate = indeterminate;

    if(this->indeterminate) {
        this->preIndeterminateValue = this->value();
        this->preIndeterminateMinimum = this->minimum();
        this->preIndeterminateMaximum = this->maximum();
        // finer steps, so the Animation is fluid
        this->setMinimum(0);
        this->setMaximum(1000);
        this->indeterminateAnimtion->start();
    }
    else {
        // reset minimum and maximum
        this->setMinimum(this->preIndeterminateMinimum);
        this->setMaximum(this->preIndeterminateMaximum);
        this->setValue(this->preIndeterminateValue);
        this->indeterminateAnimtion->stop();
    }
}

void LinboProgressBar::setReversed(bool reversed) {
    if(this->reversed == reversed)
        return;

    this->reversed = reversed;
    this->update();
}

bool LinboProgressBar::getIndeterminate() {
    return this->indeterminate;
}

void LinboProgressBar::updateIndeterminate() {
    qDebug() << "update indeterminate";
}

void LinboProgressBar::paintEvent(QPaintEvent *e) {
    QPainter painter;
    painter.begin(this);
    // background
    painter.fillRect(e->rect(), gTheme->getColor(LinboTheme::ElevatedBackgroundColor));

    double from = 0;
    double to = 0;

    // progress
    if(this->indeterminate) {
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

    if(this->reversed) {
        // if reversed -> reverse and swap from and to
        double tmp = 1 - from;
        from = 1 - to;
        to = tmp;

    }

    painter.fillRect(QRect(e->rect().width() * from, 0, e->rect().width() * to, e->rect().height()), gTheme->getColor(LinboTheme::AccentColor));

    painter.end();

    QWidget::paintEvent(e);
}

