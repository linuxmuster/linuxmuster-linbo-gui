#include "qmoderntextbrowser.h"

QModernTextBrowser::QModernTextBrowser(QWidget* parent) : QTextBrowser(parent)
{
    this->setStyleSheet("QTextBrowser {background-color: \"#ffffff\";border: 1px solid black;}"
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
