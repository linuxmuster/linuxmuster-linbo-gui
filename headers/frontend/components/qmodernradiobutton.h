#ifndef QMODERNRADIOBUTTON_H
#define QMODERNRADIOBUTTON_H

#include <QRadioButton>
#include <QObject>
#include <QWidget>

class QModernRadioButton : public QRadioButton
{
    Q_OBJECT
public:
    QModernRadioButton(QString label = "", QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;
};

#endif // QMODERNRADIOBUTTON_H
