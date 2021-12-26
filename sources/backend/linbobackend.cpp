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
    this->_logger = nullptr;
    this->_rootPassword = "";
    this->_imageToUploadAutomatically = nullptr;
    this->_state = Initializing;
    this->_postProcessActions = LinboPostProcessActions::NoAction;

    this->_logger = new LinboLogger("/tmp/linbo.log", this);

    this->_linboCmd = new LinboCmd(this);
    connect(this->_linboCmd, &LinboCmd::commandFinished, this, &LinboBackend::_handleCommandFinished);

    this->_configReader = new LinboConfigReader(this);
    this->_config = this->_configReader->readConfig();

    this->_initTimers();

    if(this->_config->guiDisabled()) {
        this->_setState(Disabled);
        this->_logger->_log("Linbo GUI is disabled", LinboLogger::LinboGuiInfo);
    }

    this->_setDefaultOs();
    this->_executeAutomaticTasks();

    if(this->state() == Initializing)
        this->_setState(Idle);
}

// --------------------
// - Public functions -
// --------------------

void LinboBackend::shutdown() {
    QProcess::execute("busybox", {"poweroff"});
}

void LinboBackend::reboot() {
    QProcess::execute("busybox", {"reboot"});
}

bool LinboBackend::startOs(LinboOs* os) {
    if(os == nullptr || (this->_state != Idle && this->_state != Autostarting) || !os->actionEnabled(LinboOs::StartOs))
        return false;

    this->_logger->_log("Starting " + os->name(), LinboLogger::LinboLogChapterBeginning);

    this->_setState(Starting);

    return this->_linboCmd->executeAsync(
               "start",
               os->bootPartition(),
               os->rootPartition(),
               os->kernel(),
               os->initrd(),
               os->kernelOptions(),
               this->_config->cachePath()
           );
}

bool LinboBackend::syncOs(LinboOs* os) {
    if(os == nullptr || (this->_state != Idle && this->_state != Autostarting) || !os->actionEnabled(LinboOs::SyncOs))
        return false;

    this->_logger->_log("Syncing " + os->name(), LinboLogger::LinboLogChapterBeginning);

    this->_setState(Syncing);

    return this->_linboCmd->executeAsync(
               "syncstart",
               this->_config->serverIpAddress(),
               this->_config->cachePath(),
               os->baseImage()->name(),
               "",
               os->bootPartition(),
               os->rootPartition(),
               os->kernel(),
               os->initrd(),
               os->kernelOptions()
           );
}

bool LinboBackend::reinstallOs(LinboOs* os) {
    if(os == nullptr || (this->_state != Idle && this->_state != Autostarting) || !os->actionEnabled(LinboOs::ReinstallOs))
        return false;

    this->_logger->_log("Reinstalling " + os->name(), LinboLogger::LinboLogChapterBeginning);

    this->_setState(Reinstalling);

    return this->_linboCmd->executeAsync(
               "syncr",
               this->_config->serverIpAddress(),
               this->_config->cachePath(),
               os->baseImage()->name(),
               "",
               os->bootPartition(),
               os->rootPartition(),
               os->kernel(),
               os->initrd(),
               os->kernelOptions(),
               QString("force")
           );
}

bool LinboBackend::login(QString password) {
    if(this->_state != Idle)
        return false;

    this->_logger->_log("Authenticating with password.", LinboLogger::LinboLogChapterBeginning);

    int exitCode = this->_linboCmd->executeSync("authenticate", this->_config->serverIpAddress(), "linbo", password, "linbo");

    if(exitCode != 0) {
        this->_logger->chapterEnd("Authentication FAILED");
        return false;
    }

    this->_rootPassword = password;
    this->_linboCmd->setStringToMaskInOutput(password);
    this->_setState(Root);
    this->restartRootTimeout();

    this->_logger->chapterEnd("Authentication SUCCESSFULL");
    return true;
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

bool LinboBackend::replaceImageOfOs(LinboOs* os, QString description, LinboPostProcessActions::Flags postProcessAction) {
    return this->createImageOfOs(os, os->baseImage()->name(), description, postProcessAction);
}

bool LinboBackend::createImageOfOs(LinboOs* os, QString name, QString description, LinboPostProcessActions::Flags postProcessActions) {
    if(this->_state != Root)
        return false;

    this->_postProcessActions = postProcessActions;

    this->_logger->_log("Creating image", LinboLogger::LinboLogChapterBeginning);
    this->_setState(CreatingImage);

    this->_logger->_log("Writing image description", LinboLogger::LinboGuiInfo);
    if(!this->_linboCmd->writeImageDescription(name, description))
        this->_logger->_log("Error writing image description, continuing anyway...", LinboLogger::LinboGuiError);

    if(this->_postProcessActions.testFlag(LinboPostProcessActions::UploadImage) && os->baseImage() != nullptr && name == os->baseImage()->name()) {
        this->_imageToUploadAutomatically = os->baseImage();
    }
    else if(this->_postProcessActions.testFlag(LinboPostProcessActions::UploadImage)) {
        this->_imageToUploadAutomatically = new LinboImage(name);
    }


    this->_logger->_log("Beginning image creation...", LinboLogger::LinboGuiInfo);
    return this->_linboCmd->executeAsync(
               "create",
               this->_config->cachePath(),
               name,
               name,
               os->bootPartition(),
               os->rootPartition(),
               os->kernel(),
               os->initrd()
           );
}

bool LinboBackend::uploadImage(const LinboImage* image, LinboPostProcessActions::Flags postProcessActions) {
    return this->_uploadImage(image, postProcessActions, false);
}

void LinboBackend::partitionDrive() {
    this->_partitionDrive(true, LinboPostProcessActions::NoAction);
}

bool LinboBackend::_partitionDrive(bool format, LinboPostProcessActions::Flags postProcessActions) {
    if(this->_state != Root && this->_state != Initializing)
        return false;

    this->_postProcessActions = postProcessActions;
    this->_logger->_log("Partitioning drive", LinboLogger::LinboLogChapterBeginning);
    this->_setState(Partitioning);

    QStringList commandArgs = QStringList(format ? "partition":"partition_noformat");
    for( int i=0; i < this->_config->diskPartitions().length(); i++) {
        LinboDiskPartition* p = this->_config->diskPartitions().at(i);
        commandArgs
                << p->path()
                << QString::number(p->size())
                << p->id()
                << QString((p->bootable())?"bootable":"\" \"")
                << p->fstype();
    }

    return this->_linboCmd->executeAsync(commandArgs);
}

bool LinboBackend::updateCache(LinboConfig::DownloadMethod downloadMethod, bool format, LinboPostProcessActions::Flags postProcessActions) {
    if(this->_state != Root && this->_state != Initializing)
        return false;

    this->_postProcessActions = postProcessActions;
    this->_logger->_log("Updating cache", LinboLogger::LinboLogChapterBeginning);
    this->_setState(UpdatingCache);

    QStringList commandArgs;
    commandArgs
            << (format ? "initcache_format":"initcache")
            << _config->serverIpAddress()
            << _config->cachePath();

    if( downloadMethod < LinboConfig::Rsync || downloadMethod > LinboConfig::Torrent )
        commandArgs.append(LinboConfig::downloadMethodToString(this->_config->downloadMethod()));
    else
        commandArgs.append(LinboConfig::downloadMethodToString(downloadMethod));

    for(int i = 0; i < this->_config->operatingSystems().length(); i++) {
        LinboOs* os = this->_config->operatingSystems().at(i);
        commandArgs
                << os->baseImage()->name()
                << "";
    }

    return this->_linboCmd->executeAsync(commandArgs);
}

bool LinboBackend::updateLinbo() {
    if(this->_state != Root)
        return false;

    return this->_linboCmd->executeAsync("update", this->_config->serverIpAddress(), this->_config->cachePath());
}

bool LinboBackend::registerClient(QString room, QString hostname, QString ipAddress, QString hostGroup, LinboConfig::LinboDeviceRole deviceRole) {
    if(this->_state != Root)
        return false;

    this->_logger->_log("Registering client", LinboLogger::LinboLogChapterBeginning);

    this->_setState(Registering);

    return this->_linboCmd->executeAsync(
               "register",
               this->_config->serverIpAddress(),
               "linbo",
               this->_rootPassword,
               room,
               hostname,
               ipAddress,
               hostGroup,
               LinboConfig::deviceRoleToString(deviceRole)
           );
}

bool LinboBackend::cancelCurrentAction() {
    switch (this->_state) {
    case Autostarting:
        this->_logger->_log("Cancelling autostart", LinboLogger::LinboGuiInfo);
        this->_autostartTimer->stop();
        this->_timeoutRemainingTimeRefreshTimer->stop();
        this->_setState(Idle);
        return true;

    case Starting:
    case Syncing:
    case Reinstalling:
        this->_logger->_log("Cancelling current start action: " + QString::number(this->_state), LinboLogger::LinboGuiInfo);
        this->_linboCmd->killAsyncProcess();
        this->_setState(Idle);
        return true;

    case RootTimeout:
        this->_logger->_log("Cancelling root timeout", LinboLogger::LinboGuiInfo);
        this->_setState(Root);
        this->restartRootTimeout();
        return true;

    case Partitioning:
    case UpdatingCache:
    case CreatingImage:
    case UploadingImage:
        this->_logger->_log("Cancelling current action: " + QString::number(this->_state), LinboLogger::LinboGuiInfo);
        this->_linboCmd->killAsyncProcess();
        if(!this->_postProcessActions.testFlag(LinboPostProcessActions::CancelToIdle))
            this->_setState(Root);
        else
            this->_setState(Idle);
        this->_postProcessActions = LinboPostProcessActions::NoAction;
        return true;

    default:
        this->_logger->_log("Cannot cancel current action: " + QString::number(this->_state), LinboLogger::LinboGuiError);
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

LinboLogger* LinboBackend::logger() {
    return this->_logger;
}

LinboConfig* LinboBackend::config() {
    return this->_config;
}

double LinboBackend::autostartTimeoutProgress() {
    return this->_autostartTimer->isActive() ? 1 - double(this->_autostartTimer->remainingTime()) / double(this->_autostartTimer->interval()) : 1;
}

int LinboBackend::autostartTimeoutRemainingSeconds() {
    return this->_autostartTimer->isActive() ? this->_autostartTimer->remainingTime() / 1000 : 0;
}

double LinboBackend::rootTimeoutProgress() {
    return this->_rootTimeoutTimer->isActive() ? 1 - double(this->_rootTimeoutTimer->remainingTime()) / double(this->_rootTimeoutTimer->interval()) : 1;
}

int LinboBackend::rootTimeoutRemainingSeconds() {
    return this->_rootTimeoutTimer->isActive() ? this->_rootTimeoutTimer->remainingTime() / 1000 : 0;
}


// -----------
// - Helpers -
// -----------

void LinboBackend::_initTimers() {
    // autostart timers
    this->_autostartTimer = new QTimer(this);
    this->_autostartTimer->setSingleShot(true);
    connect(this->_autostartTimer, &QTimer::timeout, this->_timeoutRemainingTimeRefreshTimer, &QTimer::stop);
    connect(this->_autostartTimer, &QTimer::timeout, this, &LinboBackend::_handleAutostartTimerTimeout);

    // root timeout timer
    this->_rootTimeoutTimer = new QTimer(this);
    this->_rootTimeoutTimer->setSingleShot(true);
    connect(this->_rootTimeoutTimer, &QTimer::timeout, this->_timeoutRemainingTimeRefreshTimer, &QTimer::stop);
    connect(this->_rootTimeoutTimer, &QTimer::timeout, this, &LinboBackend::_handleRootTimerTimeout);

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
}

void LinboBackend::_executeAutomaticTasks() {
    if(this->_executeAutoPartition())
        return;
    if(this->_executeAutoInitCache())
        return;

    this->_executeAutostart();
}

bool LinboBackend::_executeAutoPartition() {
    if(this->_config->autoPartition()) {
        return this->_partitionDrive(true, LinboPostProcessActions::ExecuteAutoInitCache | LinboPostProcessActions::CancelToIdle);
    }
    return false;
}

bool LinboBackend::_executeAutoInitCache() {
    if(this->_config->autoInitCache()) {
        return this->updateCache(this->_config->downloadMethod(), false, LinboPostProcessActions::ExecuteAutostart | LinboPostProcessActions::CancelToIdle);
    }
    return false;
}

LinboOs* LinboBackend::_getOsForAutostart() {
    LinboOs* osForAutostart = nullptr;
    for(LinboOs* os : this->_config->operatingSystems())
        if(os->autostartEnabled()) {
            osForAutostart = os;
            break;
        }

    if(osForAutostart == nullptr || !osForAutostart->actionEnabled(osForAutostart->defaultAction())) {
        return nullptr;
    }

    return osForAutostart;
}

bool LinboBackend::_executeAutostart() {
    LinboOs* osForAutostart = this->_getOsForAutostart();
    if(osForAutostart == nullptr) {
        return false;
    }

    this->_setState(Autostarting);
    this->_logger->_log("Beginning autostart timeout for " + osForAutostart->name(), LinboLogger::LinboGuiInfo);
    this->_autostartTimer->setInterval(osForAutostart->autostartTimeout() * 1000);
    this->_autostartTimer->start();
    this->_timeoutRemainingTimeRefreshTimer->start();
    return true;
}

bool LinboBackend::_uploadImage(const LinboImage* image, LinboPostProcessActions::Flags postProcessActions, bool allowCreatingImageState) {
    if(this->_state != Root && !(this->_state == CreatingImage && allowCreatingImageState))
        return false;

    if(image == nullptr)
        return false;

    this->_postProcessActions = postProcessActions;
    this->_logger->_log("Uploading image", LinboLogger::LinboLogChapterBeginning);
    this->_setState(UploadingImage);

    return this->_linboCmd->executeAsync(
               "upload",
               this->_config->serverIpAddress(),
               "linbo",
               this->_rootPassword,
               this->_config->cachePath(),
               image->name()
           );
}

void LinboBackend::_logout(bool force) {
    if((this->_state != Root && this->_state != RootTimeout) && !(this->_state >= Root && force))
        return;

    this->_rootPassword.clear();
    this->_linboCmd->setStringToMaskInOutput("");
    this->_setState(Idle);
}

void LinboBackend::_handleAutostartTimerTimeout() {
    LinboOs* os = this->_getOsForAutostart();
    if(os == nullptr)
        return;

    this->_logger->_log("Executing autostart for " + os->name(), LinboLogger::LinboGuiInfo);

    if(os->executeDefaultAction())
        this->_logger->_log("Executed autostart successfully!", LinboLogger::LinboGuiInfo);
    else
        this->_logger->_log("An error occured when executing autostart for " + os->name(), LinboLogger::LinboGuiError);
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

void LinboBackend::_handleCommandFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitStatus)
    if(exitCode == 0) {
        this->_logger->_log("Process exited normally.", LinboLogger::LinboGuiInfo);

        if(this->_postProcessActions.testFlag(LinboPostProcessActions::UploadImage)) {
            this->_logger->_log("Process exited.", LinboLogger::LinboLogChapterEnd);
            this->_postProcessActions = this->_postProcessActions.setFlag(LinboPostProcessActions::UploadImage, false);
            this->_uploadImage(this->_imageToUploadAutomatically, this->_postProcessActions, true);
            this->_imageToUploadAutomatically = nullptr;
            return;
        }
        else if(this->_postProcessActions.testFlag(LinboPostProcessActions::Shutdown)) {
            this->shutdown();
        }
        else if(this->_postProcessActions.testFlag(LinboPostProcessActions::Reboot)) {
            this->reboot();
        }
        else if (this->_postProcessActions.testFlag(LinboPostProcessActions::Logout) && this->_state >= Root) {
            this->_logout(true);
        }
        else if(this->_postProcessActions.testFlag(LinboPostProcessActions::ExecuteAutoInitCache)) {
            this->_setState(Initializing);
            this->_postProcessActions = LinboPostProcessActions::NoAction;
            this->_logger->_log("Process exited.", LinboLogger::LinboLogChapterEnd);
            this->_executeAutoInitCache();
            return;
        }
        else if(this->_postProcessActions.testFlag(LinboPostProcessActions::ExecuteAutostart)) {
            this->_setState(Initializing);
            this->_executeAutostart();
        }
        else if(this->_state > Root)
            this->_setState(RootActionSuccess);

        this->_postProcessActions = LinboPostProcessActions::NoAction;
    }
    else {
        this->_logger->_log("Process exited with an error.", LinboLogger::LinboGuiError);

        if(this->_state > Root && !this->_postProcessActions.testFlag(LinboPostProcessActions::CancelToIdle))
            this->_setState(RootActionError);
        else if(this->_state != Root && this->_state != Idle) // prevent showing an error when the process was cancelled
            this->_setState(StartActionError);

        this->_postProcessActions = LinboPostProcessActions::NoAction;
    }

    this->_logger->_log("Process exited.", LinboLogger::LinboLogChapterEnd);
}

LinboBackend::LinboState LinboBackend::state() {
    return this->_state;
}

void LinboBackend::_setState(LinboState state) {
    if(this->_state == state)
        return;

    this->_state = state;
    emit this->stateChanged(this->_state);

    if(this->_logger != nullptr)
        this->_logger->_log("Linbo state changed to: " + QString::number(state), LinboLogger::LinboGuiInfo);
}
