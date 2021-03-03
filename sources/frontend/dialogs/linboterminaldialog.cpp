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

#include "linboterminaldialog.h"

LinboTerminalDialog::LinboTerminalDialog(QWidget* parent) : LinboDialog(parent)
{
    //% "Terminal"
    this->setTitle(qtTrId("dialog_terminal_title"));
    this->terminal = new LinboTerminal(this);
    connect(this->terminal, &LinboTerminal::processExited, this, &LinboDialog::autoClose);
}

void LinboTerminalDialog::setVisibleAnimated(bool visible) {
    LinboDialog::setVisibleAnimated(visible);
}

void LinboTerminalDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int margins = gTheme->getSize(LinboGuiTheme::Margins);
    this->terminal->setGeometry(margins, margins, this->width() - 2 * margins, this->height() - 2 * margins);
}
