#ifndef QMODERNRADIOBUTTON_H
#define QMODERNRADIOBUTTON_H

#include <QRadioButton>
#include <QObject>
#include <QWidget>

#include "linboguitheme.h"

class LinboRadioButton : public QRadioButton
{
    Q_OBJECT
public:
    LinboRadioButton(QString label = "", QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;
};

#endif // QMODERNRADIOBUTTON_H
