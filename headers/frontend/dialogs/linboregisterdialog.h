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

#ifndef LINBOREGISTERDIALOG_H
#define LINBOREGISTERDIALOG_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

#include "linbodialog.h"
#include "linbolineedit.h"
#include "linbopushbutton.h"
#include "linbocombobox.h"

#include "linbobackend.h"

class LinboRegisterDialog : public LinboDialog
{
    Q_OBJECT
public:
    LinboRegisterDialog(LinboBackend* backend, QWidget* parent);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    LinboBackend* _backend;
    QVBoxLayout* _mainLayout;
    LinboLineEdit* _roomEdit;
    LinboLineEdit* _hostnameEdit;
    LinboLineEdit* _ipAddressEdit;
    LinboLineEdit* _hostGroupEdit;
    QComboBox* _roleSelectBox;

private slots:
    void _handleRoomChanged(const QString& newText);
    void _registerClient();
};

#endif // LINBOREGISTERDIALOG_H
