#include "linbotheme.h"

LinboTheme::LinboTheme(QObject *parent) : QObject(parent)
{
    this->_colors = {
        {TextColor, ""},
        {PrimaryColor, "#2a4457"},
        {AccentColor, ""}
    };

    this->_icons = {
        {BrandTopIcon, ""},
        {BrandBottomIcon, ""}
    };

    this->_iconMetaEnum = QMetaEnum::fromType<LinboThemeIcon>();
    this->_colorRoleMetaEnum = QMetaEnum::fromType<LinboThemeColorRole>();
    this->_sizeRoleMetaEnum = QMetaEnum::fromType<LinboThemeSizeRole>();
}


QString LinboTheme::iconPath(LinboThemeIcon icon) {
    if(this->_icons.contains(icon))
        return this->_icons.value(icon);
    else
        return "";
}

QColor LinboTheme::color(LinboThemeColorRole colorRole) {
    if(this->_colors.contains(colorRole))
        return this->_colors.value(colorRole);
    else
        return "";
}

QMap<LinboTheme::LinboThemeColorRole, QString> LinboTheme::colorRolesAndNames() {
    QMap<LinboTheme::LinboThemeColorRole, QString> colorRolesAndNames;
    for(int i = 0; i < LinboThemeColorRoleCount; i++) {
        colorRolesAndNames.insert(LinboThemeColorRole(i), this->_colorRoleMetaEnum.valueToKey(i));
    }
    return colorRolesAndNames;
}

QMap<LinboTheme::LinboThemeIcon, QString> LinboTheme::iconsAndNames() {
    QMap<LinboTheme::LinboThemeIcon, QString> iconsAndNames;
    for(int i = 0; i < LinboThemeIconCount; i++) {
        iconsAndNames.insert(LinboThemeIcon(i), this->_iconMetaEnum.valueToKey(i));
    }
    return iconsAndNames;
}

QMap<LinboTheme::LinboThemeSizeRole, QString> LinboTheme::sizeRolesAndNames() {
    QMap<LinboTheme::LinboThemeSizeRole, QString> sizeRolesAndNames;
    for(int i = 0; i < LinboThemeIconCount; i++) {
        sizeRolesAndNames.insert(LinboThemeSizeRole(i), this->_sizeRoleMetaEnum.valueToKey(i));
    }
    return sizeRolesAndNames;
}

QString LinboTheme::iconName(LinboThemeIcon icon) {
    return QString(this->_iconMetaEnum.valueToKey(icon)).replace("Icon", "");
}
