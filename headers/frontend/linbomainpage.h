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

#ifndef LINBOMAINPAGE_H
#define LINBOMAINPAGE_H

#include <QObject>
#include <QWidget>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QProgressBar>
#include <QInputDialog>
#include <QPropertyAnimation>
#include <QSvgRenderer>
#include <algorithm>

#include "linbobackend.h"
#include "linboosselectbutton.h"
#include "linboprogressbar.h"
#include "linboosselectionrow.h"
#include "linbomainactions.h"
#include "linboclientinfo.h"
#include "linbosvgwidget.h"

#include "linbologindialog.h"
#include "linboimageuploaddialog.h"

class LinboMainPage : public QWidget
{
    Q_OBJECT
public:
    explicit LinboMainPage(LinboBackend* backend, QWidget *parent = nullptr);

private:
    LinboBackend* backend;
    LinboOsSelectionRow* osSelectionRow;
    LinboMainActions* mainActions;
    LinboClientInfo* clientInfo;
    LinboPushButton* rootActionButton;
    LinboPushButton* logoutActionButton;
    QList<LinboPushButton*> powerActionButtons;

    LinboLoginDialog* loginDialog;
    LinboTerminalDialog* terminalDialog;
    LinboConfirmationDialog* confirmationDialog;
    LinboRegisterDialog* registerDialog;
    LinboUpdateCacheDialog* updateCacheDialog;
    LinboImageCreationDialog* imageCreationDialog;
    LinboImageUploadDialog* imageUploadDialog;
    QList<LinboDialog*> allDialogs;

    QPropertyAnimation* startActionWidgetAnimation;
    QPropertyAnimation* clientInfoAnimation;

    bool inited;
    bool showClientInfo;
    bool f1Pressed;

private slots:
    void handleLinboStateChanged(LinboBackend::LinboState newState);

    bool eventFilter(QObject *obj, QEvent *event) override;
signals:

};

#endif // LINBOMAINPAGE_H
