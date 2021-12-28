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

#ifndef LINBOGUITHEME_H
#define LINBOGUITHEME_H

#include <QObject>
#include <QMetaEnum>
#include <QMainWindow>

#include "linbobackend.h"
#include "linboconfig.h"
#include "linbotheme.h"

class LinboGuiTheme : public QObject
{
    Q_OBJECT
public:
    explicit LinboGuiTheme(LinboBackend* backend, QMainWindow* mainWindow, QObject *parent = nullptr);

    QString iconPath(LinboTheme::LinboThemeIcon icon);
    QColor color(LinboTheme::LinboThemeColorRole colorRole);
    int size(LinboTheme::LinboThemeSizeRole sizeRole);
    int toFontSize(int size);

    QString insertValues(QString string);
    QString insertColorValues(QString string);
    QString insertIconValues(QString string);
    QString insertSizeValues(QString string);

private:
    QMap<LinboTheme::LinboThemeColorRole, QColor> _lightColors = {
        {LinboTheme::TextColor, "#ffffff"},
        {LinboTheme::ToolButtonColor, "#f59c00"},
        {LinboTheme::DisabledToolButtonColor, "#94753e"},
        {LinboTheme::AccentColor, "#f59c00"}
    };

    QMap<LinboTheme::LinboThemeColorRole, QColor> _darkColors = {
        {LinboTheme::TextColor, "#000000"},
        {LinboTheme::ToolButtonColor, "#394f5e"},
        {LinboTheme::DisabledToolButtonColor, "#7b909e"},
        {LinboTheme::AccentColor, "#f59c00"}
    };

    QList<LinboTheme::LinboThemeIcon> _universalIcons = {
        LinboTheme::OverlayStartIcon,
        LinboTheme::OverlaySyncIcon,
        LinboTheme::OverlayReinstallIcon,
        LinboTheme::OverlayImageIcon,
        LinboTheme::StartLegacyIcon,
        LinboTheme::SyncLegacyIcon,
        LinboTheme::ReinstallLegacyIcon,
        LinboTheme::UploadLegacyIcon,
        LinboTheme::DefaultOsIcon,
        LinboTheme::OverlayKeyboardFocusIcon
    };

    LinboTheme* _theme;
    QMainWindow* _mainWindow;

    bool _isBackgroundColorDark();
signals:

};

extern LinboGuiTheme* gTheme;

#endif // LINBOGUITHEME_H
