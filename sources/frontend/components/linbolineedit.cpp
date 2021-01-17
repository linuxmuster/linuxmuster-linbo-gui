#include "../../../headers/frontend/components/linbolineedit.h"

LinboLineEdit::LinboLineEdit(QWidget* parent) : QLineEdit(parent)
{
    this->setStyleSheet(
                "QLineEdit {"
                "border: 0 0 0 0;"
                "border-bottom: 1px solid " + gTheme->getColor(LinboGuiTheme::LineColor).name() + ";"
                "background: " + gTheme->getColor(LinboGuiTheme::ElevatedBackgroundColor).name() + ";"
                "padding-left: 10px;"
                "color: " + gTheme->getColor(LinboGuiTheme::TextColor).name() + ";"
                "}"
                "QLineEdit:focus {"
                "border-bottom: 1px solid " + gTheme->getColor(LinboGuiTheme::AccentColor).name() + ";"
                "}"
                );
}

void LinboLineEdit::resizeEvent(QResizeEvent *event) {
    QLineEdit::resizeEvent(event);
}
