#include "linboradiobutton.h"

LinboRadioButton::LinboRadioButton(QString label, QWidget* parent) : QRadioButton(label, parent)
{

}

void LinboRadioButton::paintEvent(QPaintEvent *e) {
    QString indicatorSize = QString::number(this->font().pixelSize());
    this->setStyleSheet("QRadioButton {"
                        "color: " + gTheme->getColor(LinboGuiTheme::TextColor).name() + ";"
                        "}"
                        "QRadioButton::indicator {"
                        "width: " + indicatorSize + "px;"
                        "height: " + indicatorSize + "px;"
                        "}"
                        "QRadioButton::indicator:unchecked {"
                        "image: url(" + gTheme->getIconPath(LinboGuiTheme::RadioButtonUncheckedIcon) + ");"
                        "}"
                        "QRadioButton::indicator:checked {"
                        "image: url(" + gTheme->getIconPath(LinboGuiTheme::RadioButtonCheckedIcon) + ");"
                        "}"
                        "");

    QRadioButton::paintEvent(e);
}
