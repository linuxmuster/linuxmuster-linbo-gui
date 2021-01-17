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

#include "linboguitheme.h"

LinboGuiTheme* gTheme = nullptr;

LinboGuiTheme::LinboGuiTheme(LinboBackend* backend, QMainWindow* mainWindow, QObject *parent) : QObject(parent)
{
    gTheme = this;
    this->backend = backend;
    this->mainWindow = mainWindow;
    this->iconMetaEnum = QMetaEnum::fromType<LinboGuiIcon>();
    this->colorRoleMetaEnum = QMetaEnum::fromType<LinboGuiColorRole>();
}


QString LinboGuiTheme::getIconPath(LinboGuiIcon icon) {
    if(icon == NoIcon)
        return "";

    QString iconPath = ":/icons/";
    if(this->universalIcons.contains(icon))
        iconPath += "universal/";
    else
        iconPath += this->backend->getConfig()->isBackgroundColorDark() ? "light/":"dark/";

    // remove "Icon"
    QString iconName = QString(this->iconMetaEnum.valueToKey(icon)).replace("Icon", "");
    // de-capitalize first letter
    iconName.replace(0, 1, iconName.at(0).toLower());

    iconPath += iconName + ".svg";
    return iconPath;
}

QColor LinboGuiTheme::getColor(LinboGuiColorRole colorRole) {

    bool isDark = this->backend->getConfig()->isBackgroundColorDark();

    switch (colorRole) {
    case BackgroundColor:
        return this->backend->getConfig()->getBackgroundColor();
    case ElevatedBackgroundColor:
        if(isDark)
            return this->getColor(BackgroundColor).lighter(120);
        else
            return this->getColor(BackgroundColor).darker(105);
    case LineColor:
        if(isDark)
            return this->getColor(BackgroundColor).lighter(170);
        else
            return this->getColor(BackgroundColor).darker(120);
    default:
        if(isDark)
            return this->lightColors[colorRole];
        else
            return this->darkColors[colorRole];
    }

}

int LinboGuiTheme::getSize(LinboGuiTheme::LinboGuiSizeRole sizeRole) {
    int rootHeight = this->mainWindow->height();
    switch (sizeRole) {
    case Margins:
        return this->getSize(RowHeight) * 0.4;
    case RowHeight:
        return rootHeight * 0.05;
    case RowLabelHeight:
        return this->getSize(RowHeight) * 0.8;
    case RowFontSize:
        return this->getSize(RowLabelHeight) * 0.5;
    case DialogWidth:
        return rootHeight * 0.8;
    case DialogHeight:
        return rootHeight * 0.8;
    }
    return 0;
}
