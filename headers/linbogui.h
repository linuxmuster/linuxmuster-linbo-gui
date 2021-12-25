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

#ifndef LINBOGUI_H
#define LINBOGUI_H

#include <QDialog>
#include <QObject>
#include <QApplication>
#include <QBoxLayout>
#include <QFont>
#include <QFontDatabase>
#include <QScreen>
#include <QMainWindow>
#include <QTranslator>

#include "linbomainpage.h"
#include "linbobackend.h"
#include "linboguitheme.h"

class LinboGui : public QMainWindow
{
    Q_OBJECT
public:
    LinboGui();

    //void done(int r) override;

private:
    LinboBackend* _backend;
    LinboGuiTheme* _theme;
    LinboMainPage* _startPage;

};

#endif // LINBOGUI_H
