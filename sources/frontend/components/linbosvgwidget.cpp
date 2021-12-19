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

#include "linbosvgwidget.h"

LinboSvgWidget::LinboSvgWidget(int height, QString path, QWidget* parent) : QLabel(parent)
{
    QSvgRenderer* renderer = new QSvgRenderer(QString(path));
    renderer->setAspectRatioMode(Qt::KeepAspectRatio);

    int linboLogoWidth = height * (renderer->defaultSize().width() / renderer->defaultSize().height());

    QImage* image = new QImage(linboLogoWidth, height, QImage::Format_ARGB32);
    image->fill(QColor(0,0,0,0));

    QPainter painter(image);
    renderer->render(&painter);

    this->setPixmap(QPixmap::fromImage(*image));
    this->setFixedHeight(height);
    this->setFixedWidth(linboLogoWidth);
}
