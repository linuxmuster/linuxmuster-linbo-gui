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
    this->_backend = backend;
    this->_mainWindow = mainWindow;
}

QString LinboGuiTheme::getIconPath(LinboTheme::LinboThemeIcon icon) {
    if(icon == LinboTheme::NoIcon)
        return "";

    if(
        !this->_backend->getConfig()->theme()->getIconPath(icon).isEmpty()
    ) {
        return this->_backend->getConfig()->theme()->getIconPath(icon);
    }

    QString iconPath = ":/icons/";
    if(this->_universalIcons.contains(icon))
        iconPath += "universal/";
    else
        iconPath += this->_isBackgroundColorDark() ? "light/":"dark/";

    // remove "Icon"
    QString iconName = this->_backend->getConfig()->theme()->getIconName(icon);
    // de-capitalize first letter
    iconName.replace(0, 1, iconName.at(0).toLower());

    iconPath += iconName + ".svg";
    return iconPath;
}

QColor LinboGuiTheme::getColor(LinboTheme::LinboThemeColorRole colorRole) {

    if(
        this->_backend->getConfig()->theme()->getColor(colorRole).isValid()
    ) {
        return this->_backend->getConfig()->theme()->getColor(colorRole);
    }

    switch (colorRole) {
    case LinboTheme::PrimaryColor:
        return this->_backend->getConfig()->theme()->getColor(LinboTheme::PrimaryColor);
    case LinboTheme::BackgroundColor:
        return this->getColor(LinboTheme::PrimaryColor);
    case LinboTheme::ElevatedBackgroundColor:
        if(this->_isBackgroundColorDark())
            return this->getColor(LinboTheme::BackgroundColor).lighter(120);
        else
            return this->getColor(LinboTheme::BackgroundColor).darker(105);
    case LinboTheme::LineColor:
        if(this->_isBackgroundColorDark())
            return this->getColor(LinboTheme::PrimaryColor).lighter(170);
        else
            return this->getColor(LinboTheme::PrimaryColor).darker(120);
    case LinboTheme::ToolButtonColor:
        return this->getColor(LinboTheme::AccentColor);
    case LinboTheme::DisabledToolButtonColor:
        if(this->_isBackgroundColorDark())
            return this->getColor(LinboTheme::ToolButtonColor).lighter(170);
        else
            return this->getColor(LinboTheme::ToolButtonColor).darker(120);
    default:
        if(this->_isBackgroundColorDark())
            return this->_lightColors[colorRole];
        else
            return this->_darkColors[colorRole];
    }

}

int LinboGuiTheme::getSize(LinboTheme::LinboThemeSizeRole sizeRole) {
    int rootHeight = this->_mainWindow->height();
    switch (sizeRole) {
    case LinboTheme::Margins:
        return this->getSize(LinboTheme::RowHeight) * 0.4;
    case LinboTheme::RowHeight:
        return rootHeight * 0.05;
    case LinboTheme::RowLabelHeight:
        return this->getSize(LinboTheme::RowHeight) * 0.8;
    case LinboTheme::RowFontSize:
        return this->getSize(LinboTheme::RowLabelHeight) * 0.5;
    case LinboTheme::DialogWidth:
        return rootHeight * 0.8;
    case LinboTheme::DialogHeight:
        return rootHeight * 0.8;
    case LinboTheme::TopLogoHeight:
        return rootHeight * 0.08; // original 0.13
    case LinboTheme::BottomLogoHeight:
        return rootHeight * 0.04; // original 0.06
    }
    return 0;
}

int LinboGuiTheme::toFontSize(int size) {
    if(size <= 0)
        return 1;
    else
        return size;
}

bool LinboGuiTheme::_isBackgroundColorDark() {
    QColor backgroundColor = QColor(this->getColor(LinboTheme::BackgroundColor));
    int h, s, v;
    backgroundColor.getHsv(&h, &s, &v);
    return v < 210;
}
