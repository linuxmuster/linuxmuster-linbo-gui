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

#include "linboconfirmationdialog.h"

LinboConfirmationDialog::LinboConfirmationDialog(QString title, QString question, QWidget* parent) : LinboDialog(parent)
{
    this->setTitle(title);
    this->_questionLabel = new QLabel(question, this);
    this->_questionLabel->setAlignment(Qt::AlignCenter);

    //% "yes"
    LinboToolButton* toolButtonCache = new LinboToolButton(qtTrId("yes"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, &LinboToolButton::clicked, this, &LinboConfirmationDialog::accepted);
    connect(toolButtonCache, &LinboToolButton::clicked, this, &LinboConfirmationDialog::autoClose);

    //% "no"
    toolButtonCache = new LinboToolButton(qtTrId("no"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, &LinboToolButton::clicked, this, &LinboConfirmationDialog::autoClose);
    connect(toolButtonCache, &LinboToolButton::clicked, this, &LinboConfirmationDialog::rejected);

    connect(this, &LinboConfirmationDialog::closedByUser, this, &LinboConfirmationDialog::rejected);
}


void LinboConfirmationDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int rowHeight = gTheme->size(LinboTheme::RowHeight);
    int margins = gTheme->size(LinboTheme::Margins);

    this->_questionLabel->setGeometry(0, (this->height() - rowHeight) / 2, this->width() - margins * 2, rowHeight);
    QFont font = this->_questionLabel->font();
    font.setPixelSize(gTheme->size(LinboTheme::RowFontSize));
    this->_questionLabel->setFont(font);
}
