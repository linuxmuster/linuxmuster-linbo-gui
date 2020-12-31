#include "qmoderntextbrowser.h"

QModernTextBrowser::QModernTextBrowser(QWidget* parent) : QTextBrowser(parent)
{
    this->setStyleSheet("QTextBrowser {"
                        "border: 0 0 0 0;"
                        "border-bottom: 1px solid #ddd;"
                        "background: #f8f8f8;"
                        "padding-left: 5px;"
                        "}"
                        "QTextBrowser:focus {"
                        "border-bottom: 1px solid #f59c00;"
                        "}"
                        "QScrollBar:vertical {"
                        "    background:lightgrey;"
                        "    width:10px;    "
                        "    margin: 0px 0px 0px 0px;"
                        "}"
                        "QScrollBar::handle:vertical {"
                        "    background: grey;"
                        "    min-height: 0px;"
                        "}"
                        "QScrollBar::add-line:vertical {"
                        "    background: grey;"
                        "    height: 0px;"
                        "    subcontrol-position: bottom;"
                        "    subcontrol-origin: margin;"
                        "}"
                        "QScrollBar::sub-line:vertical {"
                        "    background: grey;"
                        "    height: 0 px;"
                        "    subcontrol-position: top;"
                        "    subcontrol-origin: margin;"
                        "}");
}
