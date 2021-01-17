#ifndef QMODERNLINEEDIT_H
#define QMODERNLINEEDIT_H

#include <QLineEdit>
#include <QObject>
#include <QWidget>

#include "linboguitheme.h"

class LinboLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    LinboLineEdit(QWidget* parent = nullptr);

protected:

    void resizeEvent(QResizeEvent *event) override;
};

#endif // QMODERNLINEEDIT_H
