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

#ifndef LINBOIMAGECREATIONDIALOG_H
#define LINBOIMAGECREATIONDIALOG_H

#include <QObject>
#include <QWidget>
#include <QButtonGroup>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>

#include "linbodialog.h"
#include "linbotoolbutton.h"
#include "linbolineedit.h"
#include "linbotextbrowser.h"
#include "linboradiobutton.h"

#include "linbobackend.h"

class LinboImageCreationDialog : public LinboDialog
{
    Q_OBJECT
public:
    LinboImageCreationDialog(LinboBackend* backend, QWidget* parent);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    LinboBackend* backend;
    QVBoxLayout* mainLayout;
    QButtonGroup* actionButtonGroup;
    LinboLineEdit* imageNameLineEdit;
    LinboTextBrowser* imageDescriptionTextBrowser;

    QButtonGroup* postProcessActionButtonGroup;
    QHBoxLayout* postProcessActionLayout;

private slots:
    void refreshPathAndDescription(bool isOpening = false);
    void createImage(LinboBackend::LinboPostProcessActions postProcessActions);
};

#endif // LINBOIMAGECREATIONDIALOG_H
