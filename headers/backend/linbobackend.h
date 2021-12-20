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

#include "linbologger.h"
#include "linboconfig.h"
#include "linboos.h"
#include "linboimage.h"
#include "linbodiskpartition.h"
#include "linboconfigreader.h"

/**
 * @brief The LinboBackend class is used to execute Linbo commands (control linbo_cmd) very comfortable.
 *
 * The LinboBackend class offeres some easy to use functions to manage all Linbo functionality
 */
class LinboBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LinboBackend::LinboState state READ state NOTIFY stateChanged)
    Q_PROPERTY(LinboOs* _currentOs READ currentOs WRITE setCurrentOs NOTIFY currentOsChanged)
    Q_PROPERTY(double autostartTimeoutProgress READ autostartTimeoutProgress NOTIFY autostartTimeoutProgressChanged)
    Q_PROPERTY(int autostartTimeoutRemainingSeconds READ autostartTimeoutRemainingSeconds NOTIFY autostartTimeoutProgressChanged)

public:
    explicit LinboBackend(QObject *parent = nullptr);

    friend class LinboImage;
    friend class LinboConfigReader;

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

    enum LinboPostProcessAction {
        NoAction = 1,
        Shutdown = 2,
        Reboot = 4,
        Logout = 8,
        UploadImage = 16,
        ExecuteAutoInitCache = 32,
        ExecuteAutostart = 64,
        CancelToIdle = 128
    };
    Q_DECLARE_FLAGS(LinboPostProcessActions, LinboPostProcessAction)
    Q_FLAG(LinboPostProcessActions)

    LinboState state();
    LinboLogger* logger();
    LinboConfig* config();
    LinboOs* currentOs();
    void setCurrentOs(LinboOs* os);
    void restartRootTimeout();

    double autostartTimeoutProgress();
    int autostartTimeoutRemainingSeconds();

    double rootTimeoutProgress();
    int rootTimeoutRemainingSeconds();

protected:
    QString _readImageDescription(LinboImage* image);
    bool _writeImageDescription(LinboImage* image, QString newDescription);
    bool _writeImageDescription(QString imageName, QString newDescription);

private:
    LinboState _state;
    LinboLogger* _logger;
    LinboConfigReader* _configReader;
    LinboConfig* _config;
    QStringList _linboCommandCache;

    QTimer* _autostartTimer;
    QTimer* _rootTimeoutTimer;
    QTimer* _timeoutRemainingTimeRefreshTimer;

    LinboOs* _currentOs;
#ifdef TEST_ENV
    QString const _linboCmdCommand = TEST_ENV"/linbo_cmd";
#else
    QString const _linboCmdCommand = "linbo_cmd";
#endif

    QProcess* _asynchronosProcess;
    QProcess* _synchronosProcess;

    QString _rootPassword;
    const LinboImage* _imageToUploadAutomatically;
    LinboPostProcessActions _postProcessActions;

    template<typename ... Strings>
    QString _executeCommand(bool waitForFinished, QString argument, const Strings&... arguments) {
        return this->_executeCommand(waitForFinished, this->_linboCmdCommand, this->_buildCommand(argument, arguments ...));
    }

    QStringList _buildCommand() {
        QStringList tmpArguments = this->_linboCommandCache;
        this->_linboCommandCache.clear();
        return tmpArguments;
    }

    template<typename ... Strings>
    QStringList _buildCommand(QString argument, const Strings&... arguments) {
        // this appends a quoted space in case item is empty and resolves
        // problems with linbo_cmd's weird "shift"-usage
        if (argument.isEmpty())
            this->_linboCommandCache.append("");
        else
            this->_linboCommandCache.append(argument);

        return _buildCommand(arguments...);
    }

    QString _executeCommand(bool wait, QString command, QStringList commandArgs, int* returnCode = nullptr);

    void _setState(LinboState state);

public slots:
    void shutdown();
    void reboot();

    bool startCurrentOs();
    bool syncCurrentOs();
    bool reinstallCurrentOs();

    bool login(QString password);
    void logout();

    bool replaceImageOfCurrentOs(QString description = "", LinboBackend::LinboPostProcessActions postProcessActions = LinboBackend::NoAction);
    bool createImageOfCurrentOS(QString name, QString description = "", LinboBackend::LinboPostProcessActions postProcessActions = LinboBackend::NoAction);

    bool uploadImage(const LinboImage* image, LinboBackend::LinboPostProcessActions postProcessActions = LinboBackend::NoAction);

    void partitionDrive();
    bool updateCache(LinboConfig::DownloadMethod downloadMethod, bool format = false, LinboBackend::LinboPostProcessActions postProcessActions = LinboBackend::NoAction);
    bool updateLinbo();
    bool registerClient(QString room, QString hostname, QString ipAddress, QString hostGroup, LinboConfig::LinboDeviceRole deviceRole);

    bool cancelCurrentAction();
    bool resetMessage();

private slots:
    bool _partitionDrive(bool format, LinboBackend::LinboPostProcessActions postProcessActions = LinboBackend::NoAction);
    bool _uploadImage(const LinboImage* image, LinboBackend::LinboPostProcessActions postProcessAction = LinboBackend::NoAction, bool allowCreatingImageState = false);

    void _executeAutomaticTasks();
    void _executeAutoPartition();
    void _executeAutoInitCache();
    void _executeAutostart();
    void _handleAutostartTimerTimeout();
    void _handleRootTimerTimeout();
    void _readFromStdout();
    void _readFromStderr();
    void _handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void _logout(bool force);

signals:
    void stateChanged(LinboBackend::LinboState state);
    void currentOsChanged(LinboOs* os);
    void autostartTimeoutProgressChanged();
    void rootTimeoutProgressChanged();

};

extern LinboBackend* gBackend;
Q_DECLARE_OPERATORS_FOR_FLAGS(LinboBackend::LinboPostProcessActions)

#endif // LINBOBACKEND_H
