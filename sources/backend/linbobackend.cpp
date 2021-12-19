/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020-2021  Dorian Zedler <dorian@itsblue.de>
 **
 ** Portions of the code in this file are based on code by:
 ** Copyright (C) 2007 Martin Oehler <oehler@knopper.net>
 ** Copyright (C) 2007 Klaus Knopper <knopper@knopper.net>
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

#include "linbobackend.h"

LinboBackend* gBackend = nullptr;

LinboBackend::LinboBackend(QObject *parent) : QObject(parent)
{
    gBackend = this;
    this->_currentOs = nullptr;
    this->_logger = nullptr;
    this->_rootPassword = "";
    this->_imageToUploadAutomatically = nullptr;
    this->_state = Initializing;
    this->_postProcessActions = NoAction;

    this->_logger = new LinboLogger("/tmp/linbo.log", this);

    // Processes
    this->_asynchronosProcess = new QProcess(this);
    // ascynchorons commands are logged to logger
    connect(this->_asynchronosProcess, &QProcess::readyReadStandardOutput, this, &LinboBackend::_readFromStdout);
    connect(this->_asynchronosProcess, &QProcess::readyReadStandardError, this, &LinboBackend::_readFromStderr);
    connect(this->_asynchronosProcess, &QProcess::finished, this, &LinboBackend::_handleProcessFinished);

    // synchronos commands are not logged
    this->_synchronosProcess = new QProcess(this);

    this->_configReader = new LinboConfigReader(this);
    this->_config = this->_configReader->readConfig();

    // autostart timers
    this->_autostartTimer = new QTimer(this);
    this->_autostartTimer->setSingleShot(true);
    connect(this->_autostartTimer, SIGNAL(timeout()),
            this, SLOT(_handleAutostartTimerTimeout()));

    // root timeout timer
    this->_rootTimeoutTimer = new QTimer(this);
    this->_rootTimeoutTimer->setSingleShot(true);
    connect(this->_rootTimeoutTimer, SIGNAL(timeout()),
            this, SLOT(_handleRootTimerTimeout()));

    // timeout progress refresh timer
    this->_timeoutRemainingTimeRefreshTimer = new QTimer(this);
    this->_timeoutRemainingTimeRefreshTimer->setSingleShot(false);
    this->_timeoutRemainingTimeRefreshTimer->setInterval(10);
    connect(this->_timeoutRemainingTimeRefreshTimer, &QTimer::timeout, this,
    [=] {
        if(this->_state == Autostarting)
            emit this->autostartTimeoutProgressChanged();
        else if(this->_state == RootTimeout)
            emit this->rootTimeoutProgressChanged();
    });

    if(this->_config->guiDisabled()) {
        this->_setState(Disabled);
        this->_logger->log("Linbo GUI is disabled", LinboLogger::LinboGuiInfo);
    }


    // default select first OS if no other OS has been selected yet
    if(_config->operatingSystems().length() > 0 && this->_currentOs == nullptr)
        this->_currentOs = this->_config->operatingSystems().at(0);

    this->_executeAutomaticTasks();
}

// --------------------
// - Public functions -
// --------------------

void LinboBackend::shutdown() {
    this->_executeCommand(false, "busybox", QStringList("poweroff"));
}

void LinboBackend::reboot() {
    this->_executeCommand(false, "busybox", QStringList("reboot"));
}

bool LinboBackend::startCurrentOs() {
    LinboOs* os = this->_currentOs;

    if(os == nullptr || (this->_state != Idle && this->_state != Autostarting) || !os->actionEnabled(LinboOs::StartOs))
        return false;

    this->_logger->log("Starting " + this->_currentOs->name(), LinboLogger::LinboLogChapterBeginning);

    this->_setState(Starting);

    this->_executeCommand(
        false,
        "start",
        os->bootPartition(),
        os->rootPartition(),
        os->kernel(),
        os->initrd(),
        os->kernelOptions(),
        this->_config->cachePath()
    );

    return true;
}

bool LinboBackend::syncCurrentOs() {
    LinboOs* os = this->_currentOs;

    if(os == nullptr || (this->_state != Idle && this->_state != Autostarting) || !os->actionEnabled(LinboOs::SyncOs))
        return false;

    this->_logger->log("Syncing " + this->_currentOs->name(), LinboLogger::LinboLogChapterBeginning);

    this->_setState(Syncing);

    this->_executeCommand(
        false,
        "syncstart",
        this->_config->serverIpAddress(),
        this->_config->cachePath(),
        os->baseImage()->getName(),
        "",
        os->bootPartition(),
        os->rootPartition(),
        os->kernel(),
        os->initrd(),
        os->kernelOptions()
    );

    return true;
}

bool LinboBackend::reinstallCurrentOs() {
    LinboOs* os = this->_currentOs;

    if(os == nullptr || (this->_state != Idle && this->_state != Autostarting) || !os->actionEnabled(LinboOs::ReinstallOs))
        return false;

    this->_logger->log("Reinstalling " + this->_currentOs->name(), LinboLogger::LinboLogChapterBeginning);

    this->_setState(Reinstalling);

    this->_executeCommand(
        false,
        "syncr",
        this->_config->serverIpAddress(),
        this->_config->cachePath(),
        os->baseImage()->getName(),
        "",
        os->bootPartition(),
        os->rootPartition(),
        os->kernel(),
        os->initrd(),
        os->kernelOptions(),
        QString("force")
    );

    return true;
}

bool LinboBackend::login(QString password) {
    if(this->_state != Idle)
        return false;

    this->_logger->log("Authenticating with password.", LinboLogger::LinboLogChapterBeginning);

    this->_executeCommand(true, "authenticate", this->_config->serverIpAddress(), "linbo", password, "linbo");
    bool successfull = this->_synchronosProcess->exitCode() == 0;

    if(successfull) {
        this->_rootPassword = password;
        this->_setState(Root);
        this->restartRootTimeout();
    }

    this->_logger->log("Authentication " + QString(successfull ? "OK":"FAILED"), LinboLogger::LinboLogChapterEnd);

    return successfull;
}

void LinboBackend::logout() {
    this->_logout(false);
}

void LinboBackend::restartRootTimeout() {
    if(this->_state == Root && this->_config->rootTimeout() > 0) {
        this->_rootTimeoutTimer->start((this->_config->rootTimeout() * 1000) / 2);
        this->_timeoutRemainingTimeRefreshTimer->start();
    }
}

bool LinboBackend::replaceImageOfCurrentOs(QString description, LinboPostProcessActions postProcessAction) {
    return this->createImageOfCurrentOS(this->_currentOs->baseImage()->getName(), description, postProcessAction);
}

bool LinboBackend::createImageOfCurrentOS(QString name, QString description, LinboPostProcessActions postProcessActions) {
    if(this->_state != Root)
        return false;

    this->_postProcessActions = postProcessActions;

    this->_logger->log("Creating image", LinboLogger::LinboLogChapterBeginning);
    this->_setState(CreatingImage);

    this->_logger->log("Writing image description", LinboLogger::LinboGuiInfo);
    if(!this->_writeImageDescription(name, description))
        this->_logger->log("Error writing image description, continuing anyway...", LinboLogger::LinboGuiError);

    if(this->_postProcessActions.testFlag(UploadImage) && this->_currentOs->baseImage() != nullptr && name == this->_currentOs->baseImage()->getName()) {
        this->_imageToUploadAutomatically = this->_currentOs->baseImage();
    }
    else if(this->_postProcessActions.testFlag(UploadImage)) {
        this->_imageToUploadAutomatically = new LinboImage(name);
    }


    this->_logger->log("Beginning image creation...", LinboLogger::LinboGuiInfo);
    this->_executeCommand(
        false,
        "create",
        this->_config->cachePath(),
        name,
        name,
        this->_currentOs->bootPartition(),
        this->_currentOs->rootPartition(),
        this->_currentOs->kernel(),
        this->_currentOs->initrd()
    );

    return true;
}

bool LinboBackend::uploadImage(const LinboImage* image, LinboPostProcessActions postProcessActions) {
    return this->_uploadImage(image, postProcessActions, false);
}

bool LinboBackend::partitionDrive(bool format, LinboPostProcessActions postProcessActions) {
    if(this->_state != Root && this->_state != Initializing)
        return false;

    this->_postProcessActions = postProcessActions;
    this->_logger->log("Partitioning drive", LinboLogger::LinboLogChapterBeginning);
    this->_setState(Partitioning);

    QStringList commandArgs = QStringList(format ? "partition":"partition_noformat");
    for( int i=0; i < this->_config->diskPartitions().length(); i++) {
        LinboDiskPartition* p = this->_config->diskPartitions().at(i);
        commandArgs.append(
            this->_buildCommand(
                p->path(),
                QString::number(p->size()),
                p->id(),
                QString((p->bootable())?"bootable":"\" \""),
                p->fstype()
            )
        );
    }

    this->_executeCommand(false, this->_linboCmdCommand, commandArgs);

    return true;
}

bool LinboBackend::updateCache(LinboConfig::DownloadMethod downloadMethod, bool format, LinboPostProcessActions postProcessActions) {
    if(this->_state != Root && this->_state != Initializing)
        return false;

    this->_postProcessActions = postProcessActions;
    this->_logger->log("Updating cache", LinboLogger::LinboLogChapterBeginning);
    this->_setState(UpdatingCache);

    QStringList commandArgs = this->_buildCommand(format ? "initcache_format":"initcache", _config->serverIpAddress(), _config->cachePath());

    if( downloadMethod < LinboConfig::Rsync || downloadMethod > LinboConfig::Torrent )
        commandArgs.append(LinboConfig::downloadMethodToString(this->_config->downloadMethod()));
    else
        commandArgs.append(LinboConfig::downloadMethodToString(downloadMethod));

    for(int i = 0; i < this->_config->operatingSystems().length(); i++) {
        LinboOs* os = this->_config->operatingSystems().at(i);
        commandArgs.append(this->_buildCommand(os->baseImage()->getName()));
        commandArgs.append(this->_buildCommand(""));
    }

    this->_executeCommand(false, this->_linboCmdCommand, commandArgs);

    return true;
}

bool LinboBackend::updateLinbo() {
    if(this->_state != Root)
        return false;

    this->_executeCommand(false, "update", this->_config->serverIpAddress(), this->_config->cachePath());

    return true;
}

bool LinboBackend::registerClient(QString room, QString hostname, QString ipAddress, QString hostGroup, LinboConfig::LinboDeviceRole deviceRole) {
    if(this->_state != Root)
        return false;

    this->_logger->log("Registering client", LinboLogger::LinboLogChapterBeginning);

    this->_setState(Registering);

    this->_executeCommand(false, "register", this->_config->serverIpAddress(), "linbo", this->_rootPassword, room, hostname, ipAddress, hostGroup, LinboConfig::deviceRoleToString(deviceRole));

    return true;
}

bool LinboBackend::cancelCurrentAction() {
    switch (this->_state) {
    case Autostarting:
        this->_logger->log("Cancelling autostart", LinboLogger::LinboGuiInfo);
        this->_autostartTimer->stop();
        this->_timeoutRemainingTimeRefreshTimer->stop();
        this->_setState(Idle);
        return true;

    case Starting:
    case Syncing:
    case Reinstalling:
        this->_logger->log("Cancelling current start action: " + QString::number(this->_state), LinboLogger::LinboGuiInfo);
        this->_asynchronosProcess->kill();
        this->_setState(Idle);
        return true;

    case RootTimeout:
        this->_logger->log("Cancelling root timeout", LinboLogger::LinboGuiInfo);
        this->_setState(Root);
        this->restartRootTimeout();
        return true;

    case Partitioning:
    case UpdatingCache:
    case CreatingImage:
    case UploadingImage:
        this->_logger->log("Cancelling current action: " + QString::number(this->_state), LinboLogger::LinboGuiInfo);
        this->_asynchronosProcess->kill();
        if(!this->_postProcessActions.testFlag(LinboPostProcessAction::CancelToIdle))
            this->_setState(Root);
        else
            this->_setState(Idle);
        this->_postProcessActions = LinboPostProcessAction::NoAction;
        return true;

    default:
        this->_logger->log("Cannot cancel current action: " + QString::number(this->_state), LinboLogger::LinboGuiError);
        return false;
    }
}


bool LinboBackend::resetMessage() {
    switch (this->_state) {
    case StartActionError:
        this->_setState(Idle);
        break;
    case RootActionError:
        this->_setState(Root);
        break;
    case RootActionSuccess:
        this->_setState(Root);
        break;
    default:
        return false;
    }

    return true;
}

LinboLogger* LinboBackend::getLogger() {
    return this->_logger;
}

LinboConfig* LinboBackend::getConfig() {
    return this->_config;
}

LinboOs* LinboBackend::getCurrentOs() {
    return this->_currentOs;
}

void LinboBackend::setCurrentOs(LinboOs* os) {
    if((this->_state != Idle && this->_state != Root) || !this->_config->operatingSystems().contains(os) || this->_currentOs == os)
        return;

    this->_currentOs = os;
    emit this->currentOsChanged(os);
}

double LinboBackend::getAutostartTimeoutProgress() {
    return this->_autostartTimer->isActive() ? 1 - double(this->_autostartTimer->remainingTime()) / double(this->_autostartTimer->interval()) : 1;
}

int LinboBackend::getAutostartTimeoutRemainingSeconds() {
    return this->_autostartTimer->isActive() ? this->_autostartTimer->remainingTime() / 1000 : 0;
}

double LinboBackend::getRootTimeoutProgress() {
    return this->_rootTimeoutTimer->isActive() ? 1 - double(this->_rootTimeoutTimer->remainingTime()) / double(this->_rootTimeoutTimer->interval()) : 1;
}

int LinboBackend::getRootTimeoutRemainingSeconds() {
    return this->_rootTimeoutTimer->isActive() ? this->_rootTimeoutTimer->remainingTime() / 1000 : 0;
}


// -----------
// - Helpers -
// -----------

void LinboBackend::_executeAutomaticTasks() {
    this->_executeAutoPartition();
}

void LinboBackend::_executeAutoPartition() {
    if(!this->_config->autoPartition())
        return this->_executeAutoInitCache();

    this->partitionDrive(true, LinboPostProcessAction::ExecuteAutoInitCache | LinboPostProcessAction::CancelToIdle);
}

void LinboBackend::_executeAutoInitCache() {
    if(!this->_config->autoInitCache())
        return this->_executeAutostart();

    this->updateCache(this->_config->downloadMethod(), false, LinboPostProcessAction::ExecuteAutostart | LinboPostProcessAction::CancelToIdle);
}

void LinboBackend::_executeAutostart() {
    for(LinboOs* os : this->_config->operatingSystems())
        if(os->autostartEnabled()) {
            this->_currentOs = os;
            break;
        }

    if(this->_currentOs == nullptr || !this->_currentOs->autostartEnabled() || !this->_currentOs->actionEnabled(this->_currentOs->defaultAction())) {
        this->_setState(Idle);
        return;
    }

    this->_setState(Autostarting);
    this->_logger->log("Beginning autostart timeout for " + this->_currentOs->name(), LinboLogger::LinboGuiInfo);
    this->_autostartTimer->setInterval(this->_currentOs->autostartTimeout() * 1000);
    this->_autostartTimer->start();
    this->_timeoutRemainingTimeRefreshTimer->start();
}

bool LinboBackend::_uploadImage(const LinboImage* image, LinboPostProcessActions postProcessActions, bool allowCreatingImageState) {
    if(this->_state != Root && !(this->_state == CreatingImage && allowCreatingImageState))
        return false;

    if(image == nullptr)
        return false;

    this->_postProcessActions = postProcessActions;
    this->_logger->log("Uploading image", LinboLogger::LinboLogChapterBeginning);
    this->_setState(UploadingImage);

    this->_executeCommand(
        false,
        "upload",
        this->_config->serverIpAddress(),
        "linbo",
        this->_rootPassword,
        this->_config->cachePath(),
        image->getName()
    );

    return true;
}

QString LinboBackend::_readImageDescription(LinboImage* image) {
    QProcess readProcess;
    int exitCode = -1;
    QString description = this->_executeCommand(
                              true,
                              this->_linboCmdCommand,
                              this->_buildCommand("readfile", this->_config->cachePath(), image->getName() + ".desc"),
                              &exitCode);

    if(exitCode == 0)
        return description;
    else
        return "";
}

bool LinboBackend::_writeImageDescription(LinboImage* image, QString newDescription) {
    return this->_writeImageDescription(image->getName(), newDescription);
}

bool LinboBackend::_writeImageDescription(QString imageName, QString newDescription) {

    QProcess process;
    process.start(
        this->_linboCmdCommand,
        this->_buildCommand("writefile", this->_config->cachePath(), imageName + ".desc"));

    if(!process.waitForStarted()) {
        this->_logger->log("Description writer didn't start: " + QString::number(process.exitCode()), LinboLogger::LinboGuiError);
        return false;
    }

    process.write(newDescription.toUtf8());

    if(!process.waitForBytesWritten()) {
        this->_logger->log("Description writer didn't write: " + QString::number(process.exitCode()), LinboLogger::LinboGuiError);
        return false;
    }

    process.closeWriteChannel();

    if(!process.waitForFinished()) {
        this->_logger->log("Description writer didn't finish: " + QString::number(process.exitCode()), LinboLogger::LinboGuiError);
        return false;
    }

    return true;
}

void LinboBackend::_logout(bool force) {
    if((this->_state != Root && this->_state != RootTimeout) && !(this->_state >= Root && force))
        return;

    this->_rootPassword.clear();
    this->_setState(Idle);
}

void LinboBackend::_handleAutostartTimerTimeout() {
    this->_timeoutRemainingTimeRefreshTimer->stop();
    this->_logger->log("Executing autostart for " + this->_currentOs->name(), LinboLogger::LinboGuiInfo);

    LinboOs::LinboOsStartAction defaultAction = this->_currentOs->defaultAction();
    bool wasSuccessfull = false;
    switch (defaultAction) {
    case LinboOs::StartOs:
        wasSuccessfull = this->startCurrentOs();
        break;
    case LinboOs::SyncOs:
        wasSuccessfull = this->syncCurrentOs();
        break;
    case LinboOs::ReinstallOs:
        wasSuccessfull = this->reinstallCurrentOs();
        break;
    default:
        break;
    }

    if(wasSuccessfull)
        this->_logger->log("Executed autostart successfully!", LinboLogger::LinboGuiInfo);
    else
        this->_logger->log("An error occured when executing autostart for " + this->_currentOs->name(), LinboLogger::LinboGuiError);
}

void LinboBackend::_handleRootTimerTimeout() {
    this->_timeoutRemainingTimeRefreshTimer->stop();
    qDebug() << "timeout stop";
    if(this->_state == Root) {
        // timeout for the first time -> switch to timeout state
        this->restartRootTimeout();
        this->_setState(RootTimeout);
    }
    else if(this->_state == RootTimeout) {
        // timeout for the second time -> logout
        this->logout();
    }
    else if(this->_state > Root) {
        // non-matching state
        this->restartRootTimeout();
    }
}

QString LinboBackend::_executeCommand(bool waitForFinished, QString command, QStringList commandArgs, int* returnCode) {
    if(this->_state >= Root) {
        // args are not logged for security
        QString args = commandArgs.join(" ").replace(this->_rootPassword, "***");
        this->_logger->log("Executing " + QString(waitForFinished ? "synchronos":"asynchronos") + ": " + command + " " + args, LinboLogger::LinboGuiInfo);
    }
    else
        this->_logger->log("Executing " + QString(waitForFinished ? "synchronos":"asynchronos") + ": " + command + " " + commandArgs.join(" "), LinboLogger::LinboGuiInfo);

    if(waitForFinished) {
        // clear old output
        if(this->_synchronosProcess->bytesAvailable())
            this->_synchronosProcess->readAll();

        this->_synchronosProcess->start(command, commandArgs);
        this->_synchronosProcess->waitForStarted();

        this->_synchronosProcess->waitForFinished(10000);

        if(returnCode != nullptr)
            *returnCode = this->_synchronosProcess->exitCode();
        else if(this->_synchronosProcess->exitCode() != 0)
            return "";

        return this->_synchronosProcess->readAllStandardOutput();
    }
    else {
        _asynchronosProcess->start(command, commandArgs);
        _asynchronosProcess->waitForStarted();

        return "";
    }
}

void LinboBackend::_readFromStdout() {
    QString stdOut = this->_asynchronosProcess->readAllStandardOutput();
    QStringList lines = stdOut.split("\n");
    for(const QString &line : lines) {
        this->_logger->log(line.simplified(), LinboLogger::StdOut);
    }
}

void LinboBackend::_readFromStderr() {
    QString stdOut = this->_asynchronosProcess->readAllStandardError();
    QStringList lines = stdOut.split("\n");
    for(const QString &line : lines) {
        this->_logger->log(line.simplified(), LinboLogger::StdErr);
    }
}

void LinboBackend::_handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitStatus)
    if(exitCode == 0) {
        this->_logger->log("Process exited normally.", LinboLogger::LinboGuiInfo);

        if(this->_postProcessActions.testFlag(UploadImage)) {
            this->_logger->log("Process exited.", LinboLogger::LinboLogChapterEnd);
            this->_postProcessActions = this->_postProcessActions.setFlag(UploadImage, false);
            this->_uploadImage(this->_imageToUploadAutomatically, this->_postProcessActions, true);
            this->_imageToUploadAutomatically = nullptr;
            return;
        }
        else if(this->_postProcessActions.testFlag(Shutdown)) {
            this->shutdown();
        }
        else if(this->_postProcessActions.testFlag(Reboot)) {
            this->reboot();
        }
        else if (this->_postProcessActions.testFlag(Logout) && this->_state >= Root) {
            this->_logout(true);
        }
        else if(this->_postProcessActions.testFlag(ExecuteAutoInitCache)) {
            this->_setState(Initializing);
            this->_postProcessActions = NoAction;
            this->_logger->log("Process exited.", LinboLogger::LinboLogChapterEnd);
            this->_executeAutoInitCache();
            return;
        }
        else if(this->_postProcessActions.testFlag(ExecuteAutostart)) {
            this->_setState(Initializing);
            this->_executeAutostart();
        }
        else if(this->_state > Root)
            this->_setState(RootActionSuccess);

        this->_postProcessActions = NoAction;
    }
    else {
        this->_logger->log("Process exited with an error.", LinboLogger::LinboGuiError);

        if(this->_state > Root && !this->_postProcessActions.testFlag(LinboPostProcessAction::CancelToIdle))
            this->_setState(RootActionError);
        else if(this->_state != Root && this->_state != Idle) // prevent showing an error when the process was cancelled
            this->_setState(StartActionError);

        this->_postProcessActions = NoAction;
    }

    this->_logger->log("Process exited.", LinboLogger::LinboLogChapterEnd);
}

LinboBackend::LinboState LinboBackend::getState() {
    return this->_state;
}

void LinboBackend::_setState(LinboState state) {
    if(this->_state == state)
        return;

    this->_state = state;
    emit this->stateChanged(this->_state);

    if(this->_logger != nullptr)
        this->_logger->log("Linbo state changed to: " + QString::number(state), LinboLogger::LinboGuiInfo);
}
