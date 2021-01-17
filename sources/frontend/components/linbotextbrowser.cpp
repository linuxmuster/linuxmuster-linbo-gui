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

#include "linbotextbrowser.h"

LinboTextBrowser::LinboTextBrowser(QWidget* parent) : QTextBrowser(parent)
{
    this->setStyleSheet("QTextBrowser {"
                        "border: 0 0 0 0;"
                        "border-bottom: 1px solid " + gTheme->getColor(LinboGuiTheme::LineColor).name() + ";"
                        "background: " + gTheme->getColor(LinboGuiTheme::ElevatedBackgroundColor).name() + ";"
                        "padding-left: 5px;"
                        "color: " + gTheme->getColor(LinboGuiTheme::TextColor).name() + ";"
                        "}"
                        "QTextBrowser:focus {"
                        "border-bottom: 1px solid " + gTheme->getColor(LinboGuiTheme::AccentColor).name() + ";"
                        "}"
                        "QScrollBar:vertical {"
                        "    background:lightgrey;"
                        "    width:10px;    "
                        "    margin: 0px 0px 0px 0px;"
                        "}"
                        "QScrollBar::handle:vertical {"
                        "    background: grey;"
                        "    min-height: 0px;"
                        "}"
                        "QScrollBar::add-line:vertical {"
                        "    background: grey;"
                        "    height: 0px;"
                        "    subcontrol-position: bottom;"
                        "    subcontrol-origin: margin;"
                        "}"
                        "QScrollBar::sub-line:vertical {"
                        "    background: grey;"
                        "    height: 0 px;"
                        "    subcontrol-position: top;"
                        "    subcontrol-origin: margin;"
                        "}");
}
