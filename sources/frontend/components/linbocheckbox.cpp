#include "linbocheckbox.h"

LinboCheckBox::LinboCheckBox(QString label, QWidget* parent) : QCheckBox(label, parent)
{
}

void LinboCheckBox::paintEvent(QPaintEvent *e) {
    QString indicatorSize = QString::number(this->font().pixelSize());
    this->setStyleSheet("QCheckBox {"
                        "color: " + gTheme->getColor(LinboGuiTheme::TextColor).name() + ";"
                        "}"
                        "QCheckBox::indicator {"
                        "width: " + indicatorSize + "px;"
                        "height: " + indicatorSize + "px;"
                        "}"
                        "QCheckBox::indicator:unchecked {"
                        "image: url(" + gTheme->getIconPath(LinboGuiTheme::CheckBoxUncheckedIcon) + ");"
                        "}"
                        "QCheckBox::indicator:checked {"
                        "image: url(" + gTheme->getIconPath(LinboGuiTheme::CheckBoxCheckedIcon) + ");"
                        "}"
                        "");

    QCheckBox::paintEvent(e);
}
