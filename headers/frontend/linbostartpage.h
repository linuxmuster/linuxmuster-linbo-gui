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

#ifndef LINBOGUISTARTPAGE_H
#define LINBOGUISTARTPAGE_H

#include <QObject>
#include <QWidget>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QProgressBar>
#include <QInputDialog>
#include <QPropertyAnimation>

#include "linbobackend.h"
#include "linboosselectbutton.h"
#include "qmodernprogressbar.h"
#include "linboosselectionrow.h"
#include "linbostartactions.h"
#include "linboclientinfo.h"

#include "linbologindialog.h"

class LinboStartPage : public QWidget
{
    Q_OBJECT
public:
    explicit LinboStartPage(LinboBackend* backend, QWidget *parent = nullptr);

private:
    LinboBackend* backend;
    LinboOsSelectionRow* osSelectionRow;
    LinboStartActions* startActionsWidget;
    LinboClientInfo* clientInfo;
    QModernPushButton* rootActionButton;
    QModernPushButton* logoutActionButton;
    QList<QModernPushButton*> powerActionButtons;

    LinboLoginDialog* loginDialog;

    QPropertyAnimation* startActionWidgetAnimation;
    QPropertyAnimation* clientInfoAnimation;

    bool inited;
    bool showClientInfo;

private slots:
    void handleLinboStateChanged(LinboBackend::LinboState newState);

    void keyPressEvent(QKeyEvent *ev) override;
signals:

};

#endif // LINBOGUISTARTPAGE_H
