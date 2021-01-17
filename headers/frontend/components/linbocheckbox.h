#ifndef QMODERNCHECKBOX_H
#define QMODERNCHECKBOX_H

#include <QCheckBox>
#include <QObject>
#include <QWidget>

#include "linboguitheme.h"

class LinboCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    LinboCheckBox(QString label = "", QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
};

#endif // QMODERNCHECKBOX_H
