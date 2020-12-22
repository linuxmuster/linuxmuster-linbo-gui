#ifndef LINBOCLIENTINFO_H
#define LINBOCLIENTINFO_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QSvgWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtDebug>

#include "linboconfig.h"

class LinboClientInfo : public QWidget
{
    Q_OBJECT
public:
    explicit LinboClientInfo(LinboConfig* config, QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QVBoxLayout* mainLayout;

    QSvgWidget* networkIconWidget;
    QSvgWidget* desktopIconWidget;

    QHBoxLayout* networkInfoLayout;
    QHBoxLayout* clientInfoLayout;

signals:

};

#endif // LINBOCLIENTINFO_H
