#include "linbotoolbutton.h"

LinboToolButton::LinboToolButton(QString text, QWidget* parent) : LinboToolButton(text, LinboGuiTheme::NoIcon, LinboGuiTheme::ToolButtonColor, parent)
{
}

LinboToolButton::LinboToolButton(LinboGuiTheme::LinboGuiIcon icon, QWidget* parent) : LinboToolButton("", icon, LinboGuiTheme::ToolButtonColor, parent)
{
}

LinboToolButton::LinboToolButton(QString text, LinboGuiTheme::LinboGuiIcon icon, QWidget* parent) : LinboToolButton(text, icon, LinboGuiTheme::ToolButtonColor, parent)
{
}

LinboToolButton::LinboToolButton(QString text, LinboGuiTheme::LinboGuiIcon icon, LinboGuiTheme::LinboGuiColorRole colorRole, QWidget* parent) : LinboPushButton(gTheme->getIconPath(icon), text, parent)
{
    this->colorRole = colorRole;
}

void LinboToolButton::paintEvent(QPaintEvent *e) {
    LinboPushButton::paintEvent(e);
    if(this->colorRole == LinboGuiTheme::ToolButtonColor)
        LinboPushButton::setStyleSheet("QLabel { color: " + gTheme->getColor(this->colorRole).name() + "; font-weight: bold;}");
    else
        LinboPushButton::setStyleSheet("QLabel { color: " + gTheme->getColor(this->colorRole).name() + ";}");
}
