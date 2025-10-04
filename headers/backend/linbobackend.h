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

#ifndef LINBOBACKEND_H
#define LINBOBACKEND_H

#include <QObject>
#include <QStringList>
#include <QtDebug>
#include <QProcess>
#include <QTimer>
#include <QFile>
#include <QSettings>

#include "linbopostprocessactions.h"
#include "linbologger.h"
#include "linboconfig.h"
#include "linboos.h"
#include "linboimage.h"
#include "linbodiskpartition.h"
#include "linboconfigreader.h"
#include "linbocmd.h"

/**
 * @brief The LinboBackend class is used to execute Linbo commands (control linbo_cmd) very comfortable.
 *
 * The LinboBackend class offeres some easy to use functions to manage all Linbo functionality
 */
class LinboBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LinboBackend::LinboState state READ state NOTIFY stateChanged)
    Q_PROPERTY(double _timeoutProgress READ _timeoutProgress NOTIFY timeoutProgressChanged)
    Q_PROPERTY(int _timeoutRemainingMilliseconds READ _timeoutRemainingMilliseconds NOTIFY timeoutProgressChanged)

public:
    explicit LinboBackend(QObject *parent = nullptr);

    friend class LinboImage;
    friend class LinboConfigReader;
    friend class LinboOs;

    /**
     * @brief The LinboState enum contains all possible states of Linbo
     */
    enum LinboState {
        Initializing,
        Disabled,
        Autostarting,
        Idle,
        Starting,
        Syncing,
        Reinstalling,
        StartActionError,   /*!< The last start action failed, the resetMessage() function will reset to Idle */
        Root,
        CreatingImage,
        UploadingImage,
        Registering,
        Partitioning,
        UpdatingCache,
        Updating,
        RootTimeout,
        RootActionError,    /*!< The last root action failed, the resetMessage() function will reset to Root */
        RootActionSuccess   /*!< The last root action was successfull, the resetMessage() function will reset to Root */
    };

    LinboState state();
    LinboLogger* logger();
    LinboConfig* config();
    LinboOs* osOfCurrentAction();

    void restartRootTimeout();

public slots:
    void shutdown();
    void reboot();

    bool login(QString password);
    void logout();

    bool partitionDrive();
    bool updateCache(LinboConfig::DownloadMethod downloadMethod, bool format = false, LinboPostProcessActions::Flags postProcessActions = LinboPostProcessActions::NoAction);
    bool updateLinbo();
    bool registerClient(QString room, QString hostname, QString ipAddress, QString hostGroup, LinboConfig::LinboDeviceRole deviceRole);

    bool cancelCurrentAction();
    bool resetMessage();

protected:

protected slots:
    bool startOs(LinboOs* os);
    bool syncOs(LinboOs* os);
    bool reinstallOs(LinboOs* os);

    bool createBaseImageOfOs(LinboOs* os, QString description = "", LinboPostProcessActions::Flags postProcessActions = LinboPostProcessActions::NoAction);
    bool createDiffImageOfOs(LinboOs* os, QString description = "", LinboPostProcessActions::Flags postProcessActions = LinboPostProcessActions::NoAction);

    QString readImageDescription(LinboImage* image);
    bool writeImageDescription(LinboImage* image, QString newDescription);
    bool writeImageDescription(QString imageName, QString newDescription);

    bool uploadImage(LinboImage* image, LinboPostProcessActions::Flags postProcessActions = LinboPostProcessActions::NoAction);

    QString loadEnvironmentValue(QString key);
    QString getCacheSize();
    QString getDiskSize();

private:
    LinboState _state;
    LinboLogger* _logger;
    LinboConfigReader* _configReader;
    LinboConfig* _config;
    LinboCmd* _linboCmd;
    LinboOs* _osOfCurrentAction;

    QTimer* _timeoutTimer;
    QTimer* _timeoutRemainingTimeRefreshTimer;

    QString _rootPassword;
    LinboImage* _imageToUploadAutomatically;
    LinboPostProcessActions::Flags _postProcessActions;

    QRegularExpression qcwoEndingRegex = QRegularExpression(".qcow2$");

    void _setState(LinboState state);
    bool _createImageOfOs(LinboOs* os, QString name, QString description = "", LinboPostProcessActions::Flags postProcessActions = LinboPostProcessActions::NoAction);

private slots:
    void _logout(bool force);

    bool _partitionDrive(bool format, LinboPostProcessActions::Flags postProcessActions);
    bool _uploadImage(LinboImage* image, LinboPostProcessActions::Flags postProcessActions);

    void _executeAutomaticTasks();
    bool _executeAutoPartition();
    bool _executeAutoInitCache();
    LinboOs* _getOsForAutostart();
    bool _executeAutostart();

    void _handleCommandFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void _handleCommandFinishedSuccess();
    void _handleCommandFinishedError();
    void _executeNextPostProcessAction();
    void _executePostProcessAction(LinboPostProcessActions::Flag action);
    bool _noMorePostProcessActionsToExecute();
    bool _validatePostProcessActions(LinboPostProcessActions::Flags postProcessActions);
    LinboPostProcessActions::Flag _nextAction(LinboPostProcessActions::Flags postProcessActions);

    void _initTimers();
    void _handleTimeoutTimerTimeout();
    void _handleAutostartTimerTimeout();
    void _handleRootTimerTimeout();
    void _handleTimeoutRemaningTimeRefreshTimerTimeout();
    double _timeoutProgress();
    int _timeoutRemainingMilliseconds();

signals:
    void stateChanged(LinboBackend::LinboState state);
    void timeoutProgressChanged(double progress, int remaningMilliseconds);

};

#endif // LINBOBACKEND_H
