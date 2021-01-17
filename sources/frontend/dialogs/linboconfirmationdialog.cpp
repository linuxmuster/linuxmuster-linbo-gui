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
    this->questionLabel = new QLabel(question, this);
    this->questionLabel->setAlignment(Qt::AlignCenter);

    //= yes
    LinboToolButton* toolButtonCache = new LinboToolButton(tr("yes"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SIGNAL(accepted()));
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(autoClose()));

    //= no
    toolButtonCache = new LinboToolButton(tr("no"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(autoClose()));
    connect(toolButtonCache, SIGNAL(clicked()), this, SIGNAL(rejected()));

    connect(this, SIGNAL(closedByUser()), this, SIGNAL(rejected()));
}


void LinboConfirmationDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int rowHeight = gTheme->getSize(LinboGuiTheme::RowHeight);
    int margins = gTheme->getSize(LinboGuiTheme::Margins);

    this->questionLabel->setGeometry(0, (this->height() - rowHeight) / 2, this->width() - margins * 2, rowHeight);
    QFont font = this->questionLabel->font();
    font.setPixelSize(gTheme->getSize(LinboGuiTheme::RowFontSize));
    this->questionLabel->setFont(font);
}
