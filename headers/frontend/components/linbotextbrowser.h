#ifndef QMODERNTEXTBROWSER_H
#define QMODERNTEXTBROWSER_H

#include <QTextBrowser>
#include <QObject>
#include <QWidget>

#include "linboguitheme.h"

class LinboTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    LinboTextBrowser(QWidget* parent = nullptr);
};

#endif // QMODERNTEXTBROWSER_H
