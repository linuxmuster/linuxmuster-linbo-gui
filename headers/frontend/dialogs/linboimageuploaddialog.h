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

#ifndef LINBOIMAGEUPLOADDIALOG_H
#define LINBOIMAGEUPLOADDIALOG_H

#include <QObject>
#include <QButtonGroup>
#include <QComboBox>

#include "linbodialog.h"
#include "linbobackend.h"
#include "linboradiobutton.h"
#include "linbotoolbutton.h"
#include "linbocombobox.h"

class LinboImageUploadDialog : public LinboDialog
{
    Q_OBJECT
public:
    LinboImageUploadDialog(LinboBackend* backend, QWidget* parent);

public slots:
    void open(LinboOs* os);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void refreshImageList();

private:
    LinboBackend* _backend;

    QVBoxLayout* _mainLayout;

    QComboBox* _imageSelectBox;

    QButtonGroup* _postProcessActionButtonGroup;
    QHBoxLayout* _postProcessActionLayout;

    LinboToolButton* _uploadButton;

    LinboOs* _targetOs;
};

#endif // LINBOIMAGEUPLOADDIALOG_H
