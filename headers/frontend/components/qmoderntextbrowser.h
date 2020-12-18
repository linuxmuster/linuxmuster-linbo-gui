#ifndef QMODERNTEXTBROWSER_H
#define QMODERNTEXTBROWSER_H

#include <QTextBrowser>
#include <QObject>
#include <QWidget>

class QModernTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    QModernTextBrowser(QWidget* parent = nullptr);
};

#endif // QMODERNTEXTBROWSER_H
