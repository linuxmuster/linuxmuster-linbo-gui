#include "qmoderncheckbox.h"

QModernCheckBox::QModernCheckBox(QString label, bool darkMode, QWidget* parent) : QCheckBox(label, parent)
{
    this->darkMode = darkMode;
}

void QModernCheckBox::paintEvent(QPaintEvent *e) {
    QString indicatorSize = QString::number(this->font().pixelSize() * 0.8);
    QString iconPathPrefix = ":/icons/" + QString(this->darkMode ? "light":"dark") + "/";
    this->setStyleSheet("QCheckBox::indicator {"
                        "width: " + indicatorSize + "px;"
                        "height: " + indicatorSize + "px;"
                        "}"
                        "QCheckBox::indicator:unchecked {"
                        "image: url(" + iconPathPrefix + "/checkboxUnchecked.svg);"
                        "}"
                        "QCheckBox::indicator:checked {"
                        "image: url(" + iconPathPrefix + "/checkboxChecked.svg);"
                        "}"
                        "");

    QCheckBox::paintEvent(e);
}
