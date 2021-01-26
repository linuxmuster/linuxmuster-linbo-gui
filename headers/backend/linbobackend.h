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

/**
 * @brief The LinboBackend class is used to execute Linbo commands (control linbo_cmd) very comfortable.
 *
 * The LinboBackend class offeres some easy to use functions to manage all Linbo functionality
 */
class LinboBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LinboBackend::LinboState state READ getState NOTIFY stateChanged)
    Q_PROPERTY(LinboOs* currentOs READ getCurrentOs WRITE setCurrentOs NOTIFY currentOsChanged)
    Q_PROPERTY(double autostartTimeoutProgress READ getAutostartTimeoutProgress NOTIFY autostartTimeoutProgressChanged)
    Q_PROPERTY(int autostartTimeoutRemainingSeconds READ getAutostartTimeoutRemainingSeconds NOTIFY autostartTimeoutProgressChanged)

public:
    explicit LinboBackend(QObject *parent = nullptr);

    friend class LinboImage;

    /**
     * @brief The LinboState enum contains all possible states of Linbo
     */
    enum LinboState {
        Initializing,
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

    enum LinboDeviceRole {
        ClassroomStudentComputerRole,
        ClassroomTeacherComputerRole,
        FacultyTeacherComputerRole,
        StaffComputerRole,
        DeviceRoleCount
    };

    enum LinboPostProcessAction {
        NoAction = 1,
        Shutdown = 2,
        Reboot = 4,
        Logout = 8,
        UploadImage = 16
    };
    Q_DECLARE_FLAGS(LinboPostProcessActions, LinboPostProcessAction)
    Q_FLAG(LinboPostProcessActions)

    LinboState getState();
    LinboLogger* getLogger();
    LinboConfig* getConfig();
    QList<LinboImage*> getImages();
    QList<LinboImage*> getImagesOfOs(LinboOs* os, bool includeImagesWithoutOs = true, bool includeNonExistantImages = true);
    LinboImage* getImageByName(QString name);
    QList<LinboOs*> getOperatingSystems();
    LinboOs* getCurrentOs();
    void setCurrentOs(LinboOs* os);
    void restartRootTimeout();

    double getAutostartTimeoutProgress();
    int getAutostartTimeoutRemainingSeconds();

    double getRootTimeoutProgress();
    int getRootTimeoutRemainingSeconds();

protected:
    void loadStartConfiguration(QString startConfFilePath);
    void loadEnvironmentValues();

    void writeToLinboConfig(QMap<QString, QString> config, LinboConfig* linboConfig);
    void writeToPartitionConfig(QMap<QString, QString> config, LinboDiskPartition* partition);
    void writeToOsConfig(QMap<QString, QString> config, LinboOs* os);

    bool stringToBool(const QString& value);
    LinboConfig::DownloadMethod stringToDownloadMethod(const QString& value);
    QString downloadMethodToString(const LinboConfig::DownloadMethod& value);
    QString deviceRoleToString(const LinboDeviceRole& deviceRole);

    QString readImageDescription(LinboImage* image);
    bool writeImageDescription(LinboImage* image, QString newDescription);
    bool writeImageDescription(QString imageName, QString newDescription);

private:
    LinboState state;
    LinboLogger* logger;
    LinboConfig* config;
    QStringList linboCommandCache;
    QMap<QString, LinboImage*> images;
    QList<LinboOs*> operatingSystems;
    QList<LinboDiskPartition*> diskPartitions;

    QTimer* autostartTimer;
    QTimer* rootTimeoutTimer;
    QTimer* timeoutRemainingTimeRefreshTimer;

    LinboOs* currentOs;
#ifdef TEST_ENV
    QString const linboCmdCommand = TEST_ENV"/linbo_cmd";
#else
    QString const linboCmdCommand = "linbo_cmd";
#endif

    QProcess* asynchronosProcess;
    QProcess* synchronosProcess;

    QString rootPassword;
    const LinboImage* imageToUploadAutomatically;
    LinboPostProcessActions postProcessActions;

    template<typename ... Strings>
    QString executeCommand(bool waitForFinished, QString argument, const Strings&... arguments) {
        return this->executeCommand(waitForFinished, this->linboCmdCommand, this->buildCommand(argument, arguments ...));
    }

    QStringList buildCommand() {
        QStringList tmpArguments = this->linboCommandCache;
        this->linboCommandCache.clear();
        return tmpArguments;
    }

    template<typename ... Strings>
    QStringList buildCommand(QString argument, const Strings&... arguments) {
        // this appends a quoted space in case item is empty and resolves
        // problems with linbo_cmd's weird "shift"-usage
        if (argument.isEmpty())
            this->linboCommandCache.append("");
        else
            this->linboCommandCache.append(argument);

        return buildCommand(arguments...);
    }

    QString executeCommand(bool wait, QString command, QStringList commandArgs, int* returnCode = nullptr);

    void setState(LinboState state);

public slots:
    void shutdown();
    void reboot();

    bool startCurrentOs();
    bool syncCurrentOs();
    bool reinstallCurrentOs();

    bool login(QString password);
    void logout();

    bool replaceImageOfCurrentOs(QString description = "", LinboPostProcessActions postProcessAction = NoAction);
    bool createImageOfCurrentOS(QString name, QString description = "", LinboPostProcessActions postProcessAction = NoAction);

    bool uploadImage(const LinboImage* image, LinboPostProcessActions postProcessAction = NoAction);

    bool partitionDrive(bool format = true);
    bool updateCache(LinboConfig::DownloadMethod downloadMethod, bool format = false);
    bool updateLinbo();
    bool registerClient(QString room, QString hostname, QString ipAddress, QString hostGroup, LinboDeviceRole deviceRole);

    bool cancelCurrentAction();
    bool resetMessage();

private slots:
    bool uploadImagePrivate(const LinboImage* image, LinboPostProcessActions postProcessAction = NoAction, bool allowCreatingImageState = false);

    void executeAutostart();
    void handleAutostartTimerTimeout();
    void handleRootTimerTimeout();
    void readFromStdout();
    void readFromStderr();
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

signals:
    void stateChanged(LinboBackend::LinboState state);
    void currentOsChanged(LinboOs* os);
    void autostartTimeoutProgressChanged();
    void rootTimeoutProgressChanged();

};

extern LinboBackend* gBackend;
Q_DECLARE_OPERATORS_FOR_FLAGS(LinboBackend::LinboPostProcessActions)

#endif // LINBOBACKEND_H
