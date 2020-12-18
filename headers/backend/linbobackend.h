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

#ifndef LINBOBACKEND_H
#define LINBOBACKEND_H

#include <QObject>
#include <QStringList>
#include <QtDebug>
#include <QProcess>
#include <QTimer>
#include <QElapsedTimer>
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

    enum ImageCreationAction {
        ReplaceImage,
        CreateNewImage
    };

    LinboState getState();
    LinboLogger* getLogger();
    LinboConfig* getConfig();
    QList<LinboOs*> getOperatingSystems();
    LinboOs* getCurrentOs();
    void setCurrentOs(LinboOs* os);

    double getAutostartTimeoutProgress();
    int getAutostartTimeoutRemainingSeconds();

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

private:
    LinboState state;
    LinboLogger* logger;
    LinboConfig* config;
    QStringList linboCommandCache;
    QList<LinboOs*> operatingSystems;
    QList<LinboDiskPartition*> diskPartitions;

    QElapsedTimer* autostartElapsedTimer;
    QTimer* autostartTimer;
    QTimer* autostartRemainingTimeRefreshTimer;

    LinboOs* currentOs;

    QString const linboCmdCommand = "linbo_cmd";

    QProcess* asynchronosProcess;
    QProcess* synchronosProcess;

    QString rootPassword;

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

    QString executeCommand(bool wait, QString command, QStringList commandArgs);

    void setState(LinboState state);

public slots:
    void shutdown();
    void reboot();

    bool startCurrentOs();
    bool syncCurrentOs();
    bool reinstallCurrentOs();

    bool login(QString password);
    void logout();

    bool createImageOfCurrentOs(LinboImage::ImageType type, ImageCreationAction action);
    bool uploadImageOfCurrentOs(LinboImage::ImageType type);

    bool partitionDrive(bool format = true);
    bool updateCache(LinboConfig::DownloadMethod downloadMethod, bool format = false);
    bool updateLinbo();
    bool registerClient(QString room, QString hostname, QString ipAddress, QString hostGroup, LinboDeviceRole deviceRole);

    bool cancelCurrentAction();
    bool resetMessage();

private slots:
    void executeAutostart();
    void handleAutostartTimerTimeout();
    void readFromStdout();
    void readFromStderr();
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

signals:
    void stateChanged(LinboBackend::LinboState state);
    void currentOsChanged(LinboOs* os);
    void autostartTimeoutProgressChanged();

};

#endif // LINBOBACKEND_H
