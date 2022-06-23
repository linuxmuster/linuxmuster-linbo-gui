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

    this->_iconMetaEnum = QMetaEnum::fromType<Icon>();
    this->_colorRoleMetaEnum = QMetaEnum::fromType<ColorRole>();
    this->_sizeRoleMetaEnum = QMetaEnum::fromType<SizeRole>();
}


QString LinboTheme::iconPath(Icon icon) {
    if(this->_icons.contains(icon))
        return this->_icons.value(icon);
    else
        return "";
}

QColor LinboTheme::color(ColorRole colorRole) {
    if(this->_colors.contains(colorRole))
        return this->_colors.value(colorRole);
    else
        return "";
}

QMap<LinboTheme::ColorRole, QString> LinboTheme::colorRolesAndNames() {
    QMap<LinboTheme::ColorRole, QString> colorRolesAndNames;
    for(int i = 0; i < LinboThemeColorRoleCount; i++) {
        colorRolesAndNames.insert(ColorRole(i), this->_colorRoleMetaEnum.valueToKey(i));
    }
    return colorRolesAndNames;
}

QMap<LinboTheme::Icon, QString> LinboTheme::iconsAndNames() {
    QMap<LinboTheme::Icon, QString> iconsAndNames;
    for(int i = 0; i < LinboThemeIconCount; i++) {
        iconsAndNames.insert(Icon(i), this->_iconMetaEnum.valueToKey(i));
    }
    return iconsAndNames;
}

QMap<LinboTheme::SizeRole, QString> LinboTheme::sizeRolesAndNames() {
    QMap<LinboTheme::SizeRole, QString> sizeRolesAndNames;
    for(int i = 0; i < LinboThemeIconCount; i++) {
        sizeRolesAndNames.insert(SizeRole(i), this->_sizeRoleMetaEnum.valueToKey(i));
    }
    return sizeRolesAndNames;
}

QString LinboTheme::iconName(Icon icon) {
    return QString(this->_iconMetaEnum.valueToKey(icon)).replace("Icon", "");
}
