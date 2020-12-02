#include "../../../headers/frontend/components/qmodernlineedit.h"

QModernLineEdit::QModernLineEdit(QWidget* parent) : QLineEdit(parent)
{
    this->setStyleSheet(
                "QLineEdit {"
                "border: 1px solid black;"
                "}"
                );
}
