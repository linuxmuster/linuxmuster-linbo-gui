#ifndef LINBOTOOLBUTTON_H
#define LINBOTOOLBUTTON_H

#include <linbopushbutton.h>
#include <QObject>
#include <QWidget>

#include "linboguitheme.h"

class LinboToolButton : public LinboPushButton
{
    Q_OBJECT
public:
    LinboToolButton(QString text, QWidget* parent = nullptr);
    LinboToolButton(LinboGuiTheme::LinboGuiIcon icon, QWidget* parent = nullptr);
    LinboToolButton(QString text, LinboGuiTheme::LinboGuiIcon icon, QWidget* parent = nullptr);
    LinboToolButton(QString text, LinboGuiTheme::LinboGuiIcon icon, LinboGuiTheme::LinboGuiColorRole colorRole, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    LinboGuiTheme::LinboGuiColorRole colorRole;

};

#endif // LINBOTOOLBUTTON_H
