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

    enum ColorRole {
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
    Q_ENUM(ColorRole)

    enum Icon {
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
    Q_ENUM(Icon)

    enum SizeRole {
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
    Q_ENUM(SizeRole)

    QString iconPath(Icon icon);
    QColor color(ColorRole colorRole);

    QMap<ColorRole, QString> colorRolesAndNames();
    QMap<Icon, QString> iconsAndNames();
    QMap<SizeRole, QString> sizeRolesAndNames();

    QString iconName(Icon icon);

private:
    QMap<ColorRole, QColor> _colors;
    QMap<Icon, QString> _icons;
    QMetaEnum _iconMetaEnum;
    QMetaEnum _colorRoleMetaEnum;
    QMetaEnum _sizeRoleMetaEnum;
signals:

};

#endif // LINBOTHEMECONFIG_H
