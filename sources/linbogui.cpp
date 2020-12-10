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

#include "linbogui.h"

LinboGui::LinboGui()
{
#ifdef TEST_ENV
    this->setFixedHeight(QGuiApplication::screens().at(0)->geometry().height() / 2);
    this->setFixedWidth(QGuiApplication::screens().at(0)->geometry().width() / 2);
#else
    // fill whole screen
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    this->setGeometry(QGuiApplication::screens().at(0)->geometry());
    this->setFixedHeight(QGuiApplication::screens().at(0)->geometry().height());
    this->setFixedWidth(QGuiApplication::screens().at(0)->geometry().width());
#endif

    // Load segoe font
    QFontDatabase::addApplicationFont(":/fonts/SegoeUI.ttf");
    qApp->setFont(QFont("Segoe UI"));

    // white bakground
    // linuxmuster background color: #394f5e
    this->setStyleSheet( "QMainWindow { background: white }");

    // create the backend
    this->backend = new LinboBackend(this);

    // create start page
    this->startPage = new LinboStartPage(this->backend, this);

}

// prevent closing (eg. by pressing escape key)
/*void LinboGui::done(int r) {

#ifdef TEST_ENV
    QDialog::done(r);
#else
    Q_UNUSED(r)
#endif
}*/
