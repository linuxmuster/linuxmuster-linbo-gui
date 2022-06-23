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

#ifndef LINBOUPDATECACHEDIALOG_H
#define LINBOUPDATECACHEDIALOG_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QButtonGroup>

#include "linbodialog.h"
#include "linbopushbutton.h"
#include "linboradiobutton.h"
#include "linbocheckbox.h"

#include "linbobackend.h"

class LinboUpdateCacheDialog : public LinboDialog
{
    Q_OBJECT
public:
    LinboUpdateCacheDialog(LinboBackend* backend, QWidget* parent);

    void resizeEvent(QResizeEvent *event) override;

private:
    LinboBackend* _backend;
    QButtonGroup* _updateTypeButtonGroup;
    QVBoxLayout* _mainLayout;
    LinboCheckBox* _formatCheckBox;

private slots:
    void _updateCache();

};

#endif // LINBOUPDATECACHEDIALOG_H
