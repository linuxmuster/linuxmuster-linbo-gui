#ifndef QMODERNCHECKBOX_H
#define QMODERNCHECKBOX_H

#include <QCheckBox>
#include <QObject>
#include <QWidget>

class QModernCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    QModernCheckBox(QString label = "", QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;
};

#endif // QMODERNCHECKBOX_H
