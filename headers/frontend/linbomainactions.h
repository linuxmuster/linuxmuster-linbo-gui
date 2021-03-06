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

#ifndef LINBOMAINACTIONS_H
#define LINBOMAINACTIONS_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QList>
#include <QLabel>

#include "linbostackedwidget.h"
#include "linbopushbutton.h"
#include "linboprogressbar.h"

#include "linbobackend.h"
#include "linbologger.h"
#include "linboosselectionrow.h"
#include "linboterminaldialog.h"
#include "linboconfirmationdialog.h"
#include "linboregisterdialog.h"
#include "linboupdatecachedialog.h"
#include "linboimagecreationdialog.h"
#include "linbotextbrowser.h"

class LinboMainActions : public QWidget
{
    Q_OBJECT
public:
    explicit LinboMainActions(LinboBackend* backend, QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    LinboBackend* backend;

    LinboStackedWidget* stackView;

    QWidget* buttonWidget;
    LinboPushButton* startOsButton;
    LinboPushButton* syncOsButton;
    LinboPushButton* reinstallOsButton;
    QList<LinboPushButton*> actionButtons;
    QLabel* noBaseImageLabel;

    QWidget* progressBarWidget;
    LinboProgressBar* progressBar;
    QLabel* logLabel;
    QLabel* passedTimeLabel;
    QTimer* passedTimeTimer;
    double processStartedAt;
    LinboPushButton* cancelButton;

    QWidget* messageWidget;
    QVBoxLayout* messageLayout;
    QLabel* messageLabel;
    LinboTextBrowser* messageDetailsTextBrowser;
    LinboPushButton* resetMessageButton;

    QWidget* rootWidget;
    QVBoxLayout* rootLayout;
    QList<LinboPushButton*> rootActionButtons;

    QWidget* emptyWidget;

    bool inited;

private slots:
    void resizeAndPositionAllItems();
    void handleCurrentOsChanged(LinboOs* newOs);
    void handleLinboStateChanged(LinboBackend::LinboState newState);
    void handleLatestLogChanged(const LinboLogger::LinboLog& latestLog);
    void handleTimeoutProgressChanged();

signals:
    void imageCreationRequested();
    void imageUploadRequested();
    void terminalRequested();
    void cacheUpdateRequested();
    void drivePartitioningRequested();
    void registrationRequested();

};

#endif // LINBOMAINACTIONS_H
