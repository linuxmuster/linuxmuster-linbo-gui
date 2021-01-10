/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020  Dorian Zedler <dorian@itsblue.de>
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

#include "linbobackend.h"
#include "linboosselectbutton.h"
#include "qmodernprogressbar.h"
#include "linboosselectionrow.h"
#include "linbomainactions.h"
#include "linboclientinfo.h"
#include "qmodernsvgwidget.h"

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
    QModernPushButton* rootActionButton;
    QModernPushButton* logoutActionButton;
    QList<QModernPushButton*> powerActionButtons;

    LinboLoginDialog* loginDialog;
    LinboTerminalDialog* terminalDialog;
    LinboConfirmationDialog* confirmationDialog;
    LinboRegisterDialog* registerDialog;
    LinboUpdateCacheDialog* updateCacheDialog;
    LinboImageCreationDialog* imageCreationDialog;
    LinboImageUploadDialog* imageUploadDialog;

    QPropertyAnimation* startActionWidgetAnimation;
    QPropertyAnimation* clientInfoAnimation;

    bool inited;
    bool showClientInfo;

private slots:
    void handleLinboStateChanged(LinboBackend::LinboState newState);

    void keyPressEvent(QKeyEvent *ev) override;
signals:

};

#endif // LINBOMAINPAGE_H
