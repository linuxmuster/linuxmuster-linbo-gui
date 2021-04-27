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
}


QString LinboTheme::getIconPath(LinboThemeIcon icon) {
    if(this->_icons.contains(icon))
        return this->_icons.value(icon);
    else
        return "";
}

QColor LinboTheme::getColor(LinboThemeColorRole colorRole) {
    if(this->_colors.contains(colorRole))
        return this->_colors.value(colorRole);
    else
        return "";
}

QMap<LinboTheme::LinboThemeColorRole, QString> LinboTheme::getColorRolesAndNames() {
    QMap<LinboTheme::LinboThemeColorRole, QString> colorRolesAndNames;
    for(int i = 0; i < LinboThemeColorRoleCount; i++) {
        colorRolesAndNames.insert(LinboThemeColorRole(i), this->_colorRoleMetaEnum.valueToKey(i));
    }
    return colorRolesAndNames;
}

QMap<LinboTheme::LinboThemeIcon, QString> LinboTheme::getIconsAndNames() {
    QMap<LinboTheme::LinboThemeIcon, QString> iconsAndNames;
    for(int i = 0; i < LinboThemeIconCount; i++) {
        iconsAndNames.insert(LinboThemeIcon(i), this->_iconMetaEnum.valueToKey(i));
    }
    return iconsAndNames;
}

QString LinboTheme::getIconName(LinboThemeIcon icon) {
    return QString(this->_iconMetaEnum.valueToKey(icon)).replace("Icon", "");
}
