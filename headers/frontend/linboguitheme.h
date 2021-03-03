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

class LinboGuiTheme : public QObject
{
    Q_OBJECT
public:
    explicit LinboGuiTheme(LinboBackend* backend, QMainWindow* mainWindow, QObject *parent = nullptr);

    enum LinboGuiIcon {
        NoIcon,

        BrandTopIcon,
        BrandBottomIcon,

        BackIcon,
        CancelIcon,
        LogoutIcon,
        RebootIcon,
        SettingsIcon,
        ShutdownIcon,

        OverlayCheckedIcon,
        OverlayPressedIcon,
        OverlayHoveredIcon,
        OverlayKeyboardFocusIcon,
        OverlayStartIcon,
        OverlaySyncIcon,
        OverlayReinstallIcon,
        OverlayImageIcon,

        DefaultOsIcon,

        DesktopIcon,
        NetworkIcon,

        StartIcon,
        StartLegacyIcon,
        SyncIcon,
        SyncLegacyIcon,
        ReinstallIcon,
        ReinstallLegacyIcon,

        ImageIcon,
        UploadIcon,
        UploadLegacyIcon,
        PartitionIcon,
        RegisterIcon,
        TerminalIcon,

        CheckBoxCheckedIcon,
        CheckBoxUncheckedIcon,
        RadioButtonCheckedIcon,
        RadioButtonUncheckedIcon,
        RadioButtonDisabledIcon
    };
    Q_ENUM(LinboGuiIcon)

    enum LinboGuiColorRole {
        BackgroundColor,
        ElevatedBackgroundColor,
        LineColor,
        AccentColor,
        TextColor,
        ToolButtonColor,
        DisabledToolButtonColor
    };
    Q_ENUM(LinboGuiColorRole)

    enum LinboGuiSizeRole {
        Margins,
        RowHeight,
        RowLabelHeight,
        RowFontSize,
        DialogWidth,
        DialogHeight
    };
    Q_ENUM(LinboGuiSizeRole)

    const QList<LinboGuiIcon> universalIcons = {
        OverlayStartIcon,
        OverlaySyncIcon,
        OverlayReinstallIcon,
        OverlayImageIcon,
        StartLegacyIcon,
        SyncLegacyIcon,
        ReinstallLegacyIcon,
        UploadLegacyIcon,
        DefaultOsIcon
    };

    const QMap<LinboGuiColorRole, QColor> lightColors = {
        {TextColor, "#ffffff"},
        {ToolButtonColor, "#f59c00"},
        {DisabledToolButtonColor, "#94753e"},
        {AccentColor, "#f59c00"}
    };

    const QMap<LinboGuiColorRole, QColor> darkColors = {
        {TextColor, "#000000"},
        {ToolButtonColor, "#394f5e"},
        {DisabledToolButtonColor, "#7b909e"},
        {AccentColor, "#f59c00"}
    };

    QString getIconPath(LinboGuiIcon icon);
    QColor getColor(LinboGuiColorRole colorRole);
    int getSize(LinboGuiSizeRole sizeRole);
    int toFontSize(int size);

private:
    LinboBackend* backend;
    QMainWindow* mainWindow;

    QMetaEnum iconMetaEnum;
    QMetaEnum colorRoleMetaEnum;

signals:

};

extern LinboGuiTheme* gTheme;

#endif // LINBOGUITHEME_H
