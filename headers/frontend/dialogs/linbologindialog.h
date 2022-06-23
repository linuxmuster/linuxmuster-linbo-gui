/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020-2021  Dorian Zedler <dorian@itsblue.de>
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Affero General Public License as published
 ** by the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Affero General Public License for more details.
 **
 ** You should have received a copy of the GNU Affero General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#ifndef LINBOLOGINDIALOG_H
#define LINBOLOGINDIALOG_H

#include <QDialog>
#include <QObject>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolTip>
#include <QtDebug>

#include "linbodialog.h"
#include "linbopushbutton.h"
#include "linbolineedit.h"
#include "linbobackend.h"

class LinboLoginDialog : public LinboDialog
{
    Q_OBJECT
public:
    LinboLoginDialog(LinboBackend* backend, QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    virtual void setVisibleAnimated(bool visible) override;

private:
    LinboBackend* _backend;
    QLabel* _headerLabel;
    QLineEdit* _passwordInput;
    QWidget* _buttonLayoutWidget;
    QVBoxLayout* _mainLayout;

private slots:
    void inputFinished();
};

#endif // LINBOLOGINDIALOG_H
