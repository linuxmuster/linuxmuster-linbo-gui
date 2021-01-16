#ifndef QMODERNCHECKBOX_H
#define QMODERNCHECKBOX_H

#include <QCheckBox>
#include <QObject>
#include <QWidget>

class QModernCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    QModernCheckBox(QString label = "", bool darkMode = false, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    bool darkMode = false;
};

#endif // QMODERNCHECKBOX_H
