#include "../../../headers/frontend/components/qmodernlineedit.h"

QModernLineEdit::QModernLineEdit(QWidget* parent) : QLineEdit(parent)
{
    this->setStyleSheet(
                "QLineEdit {"
                "border: 0 0 0 0;"
                "border-bottom: 1px solid #ddd;"
                "background: #f8f8f8;"
                "padding-left: 10px;"
                "}"
                "QLineEdit:focus {"
                "border-bottom: 1px solid #f59c00;"
                "background: #ffffff;"
                "}"
                );
}


void QModernLineEdit::resizeEvent(QResizeEvent *event) {
    QLineEdit::resizeEvent(event);
}
