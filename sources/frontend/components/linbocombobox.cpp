#include "linbocombobox.h"

LinboComboBox::LinboComboBox()
{
    this->setStyleSheet(
                "QComboBox {"
                    "border: 0 0 0 0;"
                    "border-bottom: 1px solid " + gTheme->getColor(LinboGuiTheme::LineColor).name() + ";"
                    "background-color: " + gTheme->getColor(LinboGuiTheme::ElevatedBackgroundColor).name() + ";"
                    "selection-color: #ffffff;"
                    "selection-background-color: " + gTheme->getColor(LinboGuiTheme::AccentColor).name() + ";"
                    "color: " + gTheme->getColor(LinboGuiTheme::TextColor).name() + ";"
                "}"
                "QComboBox QAbstractItemView {"
                    "background-color: " + gTheme->getColor(LinboGuiTheme::ElevatedBackgroundColor).name() + ";"
                "}"
                "QComboBox:focus {"
                    "border-bottom: 1px solid " + gTheme->getColor(LinboGuiTheme::AccentColor).name() + ";"
                "}"
                );

}
