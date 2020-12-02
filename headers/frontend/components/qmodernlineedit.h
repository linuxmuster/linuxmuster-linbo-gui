#ifndef QMODERNLINEEDIT_H
#define QMODERNLINEEDIT_H

#include <QLineEdit>
#include <QObject>
#include <QWidget>

class QModernLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    QModernLineEdit(QWidget* parent = nullptr);
};

#endif // QMODERNLINEEDIT_H
