#include "qmoderncheckbox.h"

QModernCheckBox::QModernCheckBox(QString label, QWidget* parent) : QCheckBox(label, parent)
{

}

void QModernCheckBox::paintEvent(QPaintEvent *e) {
    QString indicatorSize = QString::number(this->font().pixelSize() * 0.8);
    this->setStyleSheet("QCheckBox::indicator {"
                        "width: " + indicatorSize + "px;"
                        "height: " + indicatorSize + "px;"
                        "}"
                        "QCheckBox::indicator:unchecked {"
                        "image: url(:/icons/universal/components/checkboxUnchecked.svg);"
                        "}"
                        "QCheckBox::indicator:checked {"
                        "image: url(:/icons/universal/components/checkboxChecked.svg);"
                        "}"
                        "");

    QCheckBox::paintEvent(e);
}
