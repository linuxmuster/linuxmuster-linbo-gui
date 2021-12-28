#ifndef LINBOTHEMECONFIG_H
#define LINBOTHEMECONFIG_H

#include <QObject>
#include <QColor>
#include <QMap>
#include <QMetaEnum>

class LinboTheme : public QObject
{
    Q_OBJECT
public:
    explicit LinboTheme(QObject *parent = nullptr);

    friend class LinboConfigReader;

    enum LinboThemeColorRole {
        BackgroundColor,
        ElevatedBackgroundColor,
        LineColor,
        AccentColor,
        TextColor,
        ToolButtonColor,
        DisabledToolButtonColor,
        PrimaryColor,

        LinboThemeColorRoleCount
    };
    Q_ENUM(LinboThemeColorRole)

    enum LinboThemeIcon {
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
        RadioButtonDisabledIcon,

        LinboThemeIconCount
    };
    Q_ENUM(LinboThemeIcon)

    enum LinboThemeSizeRole {
        Margins,
        RowHeight,
        RowLabelHeight,
        RowFontSize,
        RowPaddingSize,
        DialogWidth,
        DialogHeight,
        TopLogoHeight,
        BottomLogoHeight
    };
    Q_ENUM(LinboThemeSizeRole)

    QString iconPath(LinboThemeIcon icon);
    QColor color(LinboThemeColorRole colorRole);

    QMap<LinboThemeColorRole, QString> colorRolesAndNames();
    QMap<LinboThemeIcon, QString> iconsAndNames();
    QMap<LinboThemeSizeRole, QString> sizeRolesAndNames();

    QString iconName(LinboThemeIcon icon);

private:
    QMap<LinboThemeColorRole, QColor> _colors;
    QMap<LinboThemeIcon, QString> _icons;
    QMetaEnum _iconMetaEnum;
    QMetaEnum _colorRoleMetaEnum;
    QMetaEnum _sizeRoleMetaEnum;
signals:

};

#endif // LINBOTHEMECONFIG_H
