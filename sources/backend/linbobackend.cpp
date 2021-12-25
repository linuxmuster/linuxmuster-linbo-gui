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

    this->_linboCmd = new LinboCmd(this->_logger, this);
    connect(this->_linboCmd, &LinboCmd::commandFinished, this, &LinboBackend::_handleCommandFinished);

    this->_configReader = new LinboConfigReader(this);
    this->_config = this->_configReader->readConfig();

    // autostart timers
    this->_autostartTimer = new QTimer(this);
    this->_autostartTimer->setSingleShot(true);
    connect(this->_autostartTimer, &QTimer::timeout, this, &LinboBackend::_handleAutostartTimerTimeout);

    // root timeout timer
    this->_rootTimeoutTimer = new QTimer(this);
    this->_rootTimeoutTimer->setSingleShot(true);
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

    if(this->_config->guiDisabled()) {
        this->_setState(Disabled);
        this->_logger->_log("Linbo GUI is disabled", LinboLogger::LinboGuiInfo);
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
    QProcess::execute("busybox", {"poweroff"});
}

void LinboBackend::reboot() {
    QProcess::execute("busybox", {"reboot"});
}

bool LinboBackend::startCurrentOs() {
    LinboOs* os = this->_currentOs;

    if(os == nullptr || (this->_state != Idle && this->_state != Autostarting) || !os->actionEnabled(LinboOs::StartOs))
        return false;

    this->_logger->_log("Starting " + this->_currentOs->name(), LinboLogger::LinboLogChapterBeginning);

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

bool LinboBackend::syncCurrentOs() {
    LinboOs* os = this->_currentOs;

    if(os == nullptr || (this->_state != Idle && this->_state != Autostarting) || !os->actionEnabled(LinboOs::SyncOs))
        return false;

    this->_logger->_log("Syncing " + this->_currentOs->name(), LinboLogger::LinboLogChapterBeginning);

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

bool LinboBackend::reinstallCurrentOs() {
    LinboOs* os = this->_currentOs;

    if(os == nullptr || (this->_state != Idle && this->_state != Autostarting) || !os->actionEnabled(LinboOs::ReinstallOs))
        return false;

    this->_logger->_log("Reinstalling " + this->_currentOs->name(), LinboLogger::LinboLogChapterBeginning);

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

bool LinboBackend::replaceImageOfCurrentOs(QString description, LinboPostProcessActions postProcessAction) {
    return this->createImageOfCurrentOS(this->_currentOs->baseImage()->name(), description, postProcessAction);
}

bool LinboBackend::createImageOfCurrentOS(QString name, QString description, LinboPostProcessActions postProcessActions) {
    if(this->_state != Root)
        return false;

    this->_postProcessActions = postProcessActions;

    this->_logger->_log("Creating image", LinboLogger::LinboLogChapterBeginning);
    this->_setState(CreatingImage);

    this->_logger->_log("Writing image description", LinboLogger::LinboGuiInfo);
    if(!this->_linboCmd->writeImageDescription(name, description))
        this->_logger->_log("Error writing image description, continuing anyway...", LinboLogger::LinboGuiError);

    if(this->_postProcessActions.testFlag(UploadImage) && this->_currentOs->baseImage() != nullptr && name == this->_currentOs->baseImage()->name()) {
        this->_imageToUploadAutomatically = this->_currentOs->baseImage();
    }
    else if(this->_postProcessActions.testFlag(UploadImage)) {
        this->_imageToUploadAutomatically = new LinboImage(name);
    }


    this->_logger->_log("Beginning image creation...", LinboLogger::LinboGuiInfo);
    return this->_linboCmd->executeAsync(
               "create",
               this->_config->cachePath(),
               name,
               name,
               this->_currentOs->bootPartition(),
               this->_currentOs->rootPartition(),
               this->_currentOs->kernel(),
               this->_currentOs->initrd()
           );
}

bool LinboBackend::uploadImage(const LinboImage* image, LinboPostProcessActions postProcessActions) {
    return this->_uploadImage(image, postProcessActions, false);
}

void LinboBackend::partitionDrive() {
    this->_partitionDrive(true, NoAction);
}

bool LinboBackend::_partitionDrive(bool format, LinboPostProcessActions postProcessActions) {
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

bool LinboBackend::updateCache(LinboConfig::DownloadMethod downloadMethod, bool format, LinboPostProcessActions postProcessActions) {
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
        if(!this->_postProcessActions.testFlag(LinboPostProcessAction::CancelToIdle))
            this->_setState(Root);
        else
            this->_setState(Idle);
        this->_postProcessActions = LinboPostProcessAction::NoAction;
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

LinboOs* LinboBackend::currentOs() {
    return this->_currentOs;
}

void LinboBackend::setCurrentOs(LinboOs* os) {
    if((this->_state != Idle && this->_state != Root) || !this->_config->operatingSystems().contains(os) || this->_currentOs == os)
        return;

    this->_currentOs = os;
    emit this->currentOsChanged(os);
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

void LinboBackend::_executeAutomaticTasks() {
    this->_executeAutoPartition();
}

void LinboBackend::_executeAutoPartition() {
    if(!this->_config->autoPartition())
        return this->_executeAutoInitCache();

    this->_partitionDrive(true, LinboPostProcessAction::ExecuteAutoInitCache | LinboPostProcessAction::CancelToIdle);
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
    this->_logger->_log("Beginning autostart timeout for " + this->_currentOs->name(), LinboLogger::LinboGuiInfo);
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
    this->_timeoutRemainingTimeRefreshTimer->stop();
    this->_logger->_log("Executing autostart for " + this->_currentOs->name(), LinboLogger::LinboGuiInfo);

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
        this->_logger->_log("Executed autostart successfully!", LinboLogger::LinboGuiInfo);
    else
        this->_logger->_log("An error occured when executing autostart for " + this->_currentOs->name(), LinboLogger::LinboGuiError);
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

        if(this->_postProcessActions.testFlag(UploadImage)) {
            this->_logger->_log("Process exited.", LinboLogger::LinboLogChapterEnd);
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
            this->_logger->_log("Process exited.", LinboLogger::LinboLogChapterEnd);
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
        this->_logger->_log("Process exited with an error.", LinboLogger::LinboGuiError);

        if(this->_state > Root && !this->_postProcessActions.testFlag(LinboPostProcessAction::CancelToIdle))
            this->_setState(RootActionError);
        else if(this->_state != Root && this->_state != Idle) // prevent showing an error when the process was cancelled
            this->_setState(StartActionError);

        this->_postProcessActions = NoAction;
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
