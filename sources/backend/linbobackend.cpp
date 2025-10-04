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

LinboBackend::LinboBackend(QObject *parent) : QObject(parent)
{
    this->_logger = nullptr;
    this->_rootPassword = "";
    this->_imageToUploadAutomatically = nullptr;
    this->_state = Initializing;
    this->_postProcessActions = LinboPostProcessActions::NoAction;
    this->_osOfCurrentAction = nullptr;

    this->_logger = new LinboLogger("/tmp/linbo.log", this);

    this->_linboCmd = new LinboCmd(this->_logger, this);
    connect(this->_linboCmd, &LinboCmd::commandFinished, this, &LinboBackend::_handleCommandFinished);

    this->_configReader = new LinboConfigReader(this);
    this->_config = this->_configReader->readConfig();

    this->_initTimers();

    if(this->_config->guiDisabled()) {
        this->_setState(Disabled);
        this->_logger->_log("Linbo GUI is disabled", LinboLogger::LinboGuiInfo);
        return;
    }

    this->_executeAutomaticTasks();

    // Prevent going back to idle when we are partitioning
    if(this->state() == Initializing)
        this->_setState(Idle);
}

// --------------------
// - Public functions -
// --------------------

LinboOs* LinboBackend::osOfCurrentAction() {
    QList<LinboState> osBaseStates = {Autostarting, Starting, Syncing, Reinstalling, CreatingImage, UploadingImage, StartActionError, RootActionError, RootActionSuccess};
    if(!osBaseStates.contains(this->state()))
        return nullptr;
    return this->_osOfCurrentAction;
}

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

    this->_osOfCurrentAction = os;
    this->_setState(Starting);

    return this->_linboCmd->startOs(os, this->_config->cachePath());
}

bool LinboBackend::syncOs(LinboOs* os) {
    if(os == nullptr || (this->_state != Idle && this->_state != Autostarting) || !os->actionEnabled(LinboOs::SyncOs))
        return false;

    this->_logger->_log("Syncing " + os->name(), LinboLogger::LinboLogChapterBeginning);

    this->_osOfCurrentAction = os;
    this->_setState(Syncing);

    return this->_linboCmd->syncOs(os, this->_config->serverIpAddress(), this->_config->cachePath());
}

bool LinboBackend::reinstallOs(LinboOs* os) {
    if(os == nullptr || (this->_state != Idle && this->_state != Autostarting) || !os->actionEnabled(LinboOs::ReinstallOs))
        return false;

    this->_logger->_log("Reinstalling " + os->name(), LinboLogger::LinboLogChapterBeginning);

    this->_osOfCurrentAction = os;
    this->_setState(Reinstalling);

    return this->_linboCmd->reinstallOs(os, this->_config->serverIpAddress(), this->_config->cachePath());
}

bool LinboBackend::login(QString password) {
    if(this->_state != Idle)
        return false;

    this->_logger->_log("Authenticating with password.", LinboLogger::LinboLogChapterBeginning);

    if(!this->_linboCmd->authenticate(password, this->_config->serverIpAddress())) {
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
        this->_timeoutTimer->start((this->_config->rootTimeout() * 1000) / 2);
        this->_timeoutRemainingTimeRefreshTimer->start();
    }
}

bool LinboBackend::createBaseImageOfOs(LinboOs* os, QString description, LinboPostProcessActions::Flags postProcessAction) {
    return this->_createImageOfOs(os, os->baseImage()->name(), description, postProcessAction);
}

bool LinboBackend::createDiffImageOfOs(LinboOs* os, QString description, LinboPostProcessActions::Flags postProcessAction) {
    return this->_createImageOfOs(os, os->baseImage()->name().replace(this->qcwoEndingRegex, ".qdiff"), description, postProcessAction);
}

bool LinboBackend::_createImageOfOs(LinboOs* os, QString name, QString description, LinboPostProcessActions::Flags postProcessActions) {
    if(this->_state != Root)
        return false;

    this->_postProcessActions = postProcessActions;

    this->_logger->_log("Creating image", LinboLogger::LinboLogChapterBeginning);
    this->_osOfCurrentAction = os;
    this->_setState(CreatingImage);

    this->_logger->_log("Writing image description", LinboLogger::LinboGuiInfo);
    if(!this->_linboCmd->writeImageDescription(os->baseImage()->name(), description, this->_config->cachePath()))
        this->_logger->_log("Error writing image description, continuing anyway...", LinboLogger::LinboGuiError);

    if(this->_postProcessActions.testFlag(LinboPostProcessActions::UploadImage) && os->baseImage() != nullptr && name == os->baseImage()->name()) {
        this->_imageToUploadAutomatically = os->baseImage();
    }
    else if(this->_postProcessActions.testFlag(LinboPostProcessActions::UploadImage)) {
        this->_imageToUploadAutomatically = new LinboImage(name);
    }


    this->_logger->_log("Beginning image creation...", LinboLogger::LinboGuiInfo);
    return this->_linboCmd->createImageOfOs(os, name, this->_config->cachePath());
}

QString LinboBackend::readImageDescription(LinboImage* image) {
    return this->_linboCmd->readImageDescription(image, this->_config->cachePath());
}

bool LinboBackend::writeImageDescription(LinboImage* image, QString newDescription) {
    return this->_linboCmd->writeImageDescription(image, newDescription, this->_config->cachePath());
}

bool LinboBackend::writeImageDescription(QString imageName, QString newDescription) {
    return this->_linboCmd->writeImageDescription(imageName, newDescription, this->_config->cachePath());
}

bool LinboBackend::uploadImage(LinboImage* image, LinboPostProcessActions::Flags postProcessActions) {
    if(this->_state != Root)
        return false;
    return this->_uploadImage(image, postProcessActions);
}

bool LinboBackend::_uploadImage(LinboImage* image, LinboPostProcessActions::Flags postProcessActions) {
    if(this->_state != Root && this->_state != CreatingImage)
        return false;

    if(image == nullptr)
        return false;

    this->_postProcessActions = postProcessActions;
    this->_logger->_log("Uploading image", LinboLogger::LinboLogChapterBeginning);
    this->_osOfCurrentAction = image->_os;
    this->_setState(UploadingImage);

    return this->_linboCmd->uploadImage(image, this->_rootPassword, this->_config->serverIpAddress(), this->_config->cachePath());
}

bool LinboBackend::partitionDrive() {
    return this->_partitionDrive(true, LinboPostProcessActions::NoAction);
}

bool LinboBackend::_partitionDrive(bool format, LinboPostProcessActions::Flags postProcessActions) {
    if(this->_state != Root && this->_state != Initializing)
        return false;

    this->_osOfCurrentAction = nullptr;
    this->_postProcessActions = postProcessActions;
    this->_logger->_log("Partitioning drive", LinboLogger::LinboLogChapterBeginning);
    this->_setState(Partitioning);

    return this->_linboCmd->partitionDrive(this->_config->diskPartitions(), format);
}

bool LinboBackend::updateCache(LinboConfig::DownloadMethod downloadMethod, bool format, LinboPostProcessActions::Flags postProcessActions) {
    if(this->_state != Root && this->_state != Initializing && this->_state != UpdatingCache)
        return false;

    this->_osOfCurrentAction = nullptr;
    this->_postProcessActions = postProcessActions;
    this->_logger->_log("Updating cache", LinboLogger::LinboLogChapterBeginning);
    this->_setState(UpdatingCache);

    if(downloadMethod < LinboConfig::Rsync || downloadMethod > LinboConfig::Torrent)
        downloadMethod = this->_config->downloadMethod();

    return this->_linboCmd->updateCache(
               downloadMethod,
               format,
               this->_config->operatingSystems(),
               this->_config->serverIpAddress(),
               this->_config->cachePath()
           );
}

bool LinboBackend::updateLinbo() {
    if(this->_state != Root)
        return false;

    return this->_linboCmd->updateLinbo(this->_config->serverIpAddress(), this->_config->cachePath());
}

bool LinboBackend::registerClient(QString room, QString hostname, QString ipAddress, QString hostGroup, LinboConfig::LinboDeviceRole deviceRole) {
    if(this->_state != Root)
        return false;

    this->_logger->_log("Registering client", LinboLogger::LinboLogChapterBeginning);

    this->_setState(Registering);

    return this->_linboCmd->registerClient(
               room,
               hostname,
               ipAddress,
               hostGroup,
               deviceRole,
               this->_config->serverIpAddress(),
               this->_rootPassword
           );
}

bool LinboBackend::cancelCurrentAction() {
    switch (this->_state) {
    case Autostarting:
        this->_logger->_log("Cancelling autostart", LinboLogger::LinboGuiInfo);
        this->_timeoutTimer->stop();
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
        if(!this->_postProcessActions.testFlag(LinboPostProcessActions::Logout))
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

QString LinboBackend::loadEnvironmentValue(QString key) {
    return this->_linboCmd->getOutput(key).replace("\n", "");
}

QString LinboBackend::getCacheSize() {
    return this->_linboCmd->getOutput("size_cache").replace("\n", "");

}
QString LinboBackend::getDiskSize(){
    return this->_linboCmd->getOutput("size_disk").replace("\n", "");
}

// -----------
// - Helpers -
// -----------

void LinboBackend::_executeAutomaticTasks() {
    if(this->_executeAutoPartition())
        return;
    if(this->_executeAutoInitCache())
        return;

    this->_executeAutostart();
}

bool LinboBackend::_executeAutoPartition() {
    if(this->_config->autoPartition()) {
        return this->_partitionDrive(true, LinboPostProcessActions::ExecuteAutoInitCache | LinboPostProcessActions::ExecuteAutostart | LinboPostProcessActions::Logout);
    }
    return false;
}

bool LinboBackend::_executeAutoInitCache() {
    if(this->_config->autoInitCache()) {
        this->_setState(UpdatingCache);
        return this->updateCache(this->_config->downloadMethod(), false, LinboPostProcessActions::ExecuteAutostart | LinboPostProcessActions::Logout);
    }
    return false;
}

bool LinboBackend::_executeAutostart() {
    LinboOs* osForAutostart = this->_getOsForAutostart();
    if(osForAutostart == nullptr) {
        return false;
    }

    this->_osOfCurrentAction = osForAutostart;
    this->_setState(Autostarting);
    this->_logger->_log("Beginning autostart timeout for " + osForAutostart->name(), LinboLogger::LinboGuiInfo);
    this->_timeoutTimer->setInterval(osForAutostart->autostartTimeout() * 1000);
    this->_timeoutTimer->start();
    this->_timeoutRemainingTimeRefreshTimer->start();
    return true;
}

LinboOs* LinboBackend::_getOsForAutostart() {
    LinboOs* osForAutostart = nullptr;
    for(LinboOs* os : this->_config->operatingSystems()) {
        if(os->autostartEnabled()) {
            osForAutostart = os;
            break;
        }
    }

    if(osForAutostart == nullptr || !osForAutostart->actionEnabled(osForAutostart->defaultAction())) {
        return nullptr;
    }

    return osForAutostart;
}

void LinboBackend::_logout(bool force) {
    if((this->_state != Root && this->_state != RootTimeout) && !(this->_state >= Root && force))
        return;

    this->_rootPassword.clear();
    this->_linboCmd->setStringToMaskInOutput("");
    this->_setState(Idle);
}

void LinboBackend::_initTimers() {
    this->_timeoutRemainingTimeRefreshTimer = new QTimer(this);
    this->_timeoutRemainingTimeRefreshTimer->setSingleShot(false);
    this->_timeoutRemainingTimeRefreshTimer->setInterval(10);
    connect(this->_timeoutRemainingTimeRefreshTimer, &QTimer::timeout, this, &LinboBackend::_handleTimeoutRemaningTimeRefreshTimerTimeout);

    this->_timeoutTimer = new QTimer(this);
    this->_timeoutTimer->setSingleShot(true);
    connect(this->_timeoutTimer, &QTimer::timeout, this->_timeoutRemainingTimeRefreshTimer, &QTimer::stop);
    connect(this->_timeoutTimer, &QTimer::timeout, this, &LinboBackend::_handleTimeoutTimerTimeout);
}

void LinboBackend::_handleTimeoutRemaningTimeRefreshTimerTimeout() {
    emit this->timeoutProgressChanged(
        this->_timeoutProgress(),
        this->_timeoutRemainingMilliseconds()
    );
}

double LinboBackend::_timeoutProgress() {
    if(!this->_timeoutTimer->isActive())
        return 1.0;

    double remaningTime = this->_timeoutTimer->remainingTime();
    double totalTime = this->_timeoutTimer->interval();
    return 1 - remaningTime / totalTime;
}

int LinboBackend::_timeoutRemainingMilliseconds() {
    if(!this->_timeoutTimer->isActive())
        return 0;

    return this->_timeoutTimer->remainingTime();
}

void LinboBackend::_handleTimeoutTimerTimeout() {
    if(this->state() == Autostarting) {
        this->_handleAutostartTimerTimeout();
    }
    else if(this->state() >= Root) {
        this->_handleRootTimerTimeout();
    }
    else {
        this->_logger->error("Got timer timeout in wrong state: " + QString::number(this->state()));
    }
}

void LinboBackend::_handleAutostartTimerTimeout() {
    LinboOs* os = this->_osOfCurrentAction;
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
        // non-matching state -> some task is running (eg. create image)
        // -> restart to make sure, the timer still works after the task finished
        this->restartRootTimeout();
    }
}

// -- Command helpers --

void LinboBackend::_handleCommandFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitStatus)
    if(exitCode == 0) {
        this->_logger->chapterEnd("Command finished successfully.");
        this->_handleCommandFinishedSuccess();
    }
    else {
        this->_logger->chapterEnd("Command finished with an error.");
        this->_handleCommandFinishedError();
    }
}

void LinboBackend::_handleCommandFinishedSuccess() {
    if(this->_noMorePostProcessActionsToExecute() && this->state() > Root) {
        this->_setState(RootActionSuccess);
    }
    else if(this->_noMorePostProcessActionsToExecute() && this->state() < Root && this->state() > Idle) {
        this->_setState(Idle);
    }
    else if(!this->_noMorePostProcessActionsToExecute()) {
        this->_executeNextPostProcessAction();
    }
}

void LinboBackend::_handleCommandFinishedError() {
    if(this->_state > Root && !this->_postProcessActions.testAnyFlags(LinboPostProcessActions::Logout))
        this->_setState(RootActionError);
    else if(this->_state != Root && this->_state != Idle) // prevent showing an error when the process was cancelled
        this->_setState(StartActionError);

    this->_postProcessActions = LinboPostProcessActions::NoAction;
}

void LinboBackend::_executeNextPostProcessAction() {
    LinboPostProcessActions::Flags actions = this->_postProcessActions;
    if(!this->_validatePostProcessActions(actions)) {
        this->logger()->error("Got invalid post process actions!");
        this->_postProcessActions = LinboPostProcessActions::NoAction;
        return;
    }

    LinboPostProcessActions::Flag actionToExecute = this->_nextAction(actions);
    if(actionToExecute == LinboPostProcessActions::NoAction) {
        this->_postProcessActions = LinboPostProcessActions::NoAction;
    }
    else {
        this->_postProcessActions = actions.setFlag(actionToExecute, false);
        this->_executePostProcessAction(actionToExecute);
    }
}

void LinboBackend::_executePostProcessAction(LinboPostProcessActions::Flag action) {
    bool actionSkipped = false;
    switch(action) {
    case LinboPostProcessActions::UploadImage:
        this->_uploadImage(this->_imageToUploadAutomatically, this->_postProcessActions);
        this->_imageToUploadAutomatically = nullptr;
        break;
    case LinboPostProcessActions::ExecuteAutoInitCache:
        actionSkipped = !this->_executeAutoInitCache();
        break;
    case LinboPostProcessActions::ExecuteAutostart:
        actionSkipped = !this->_executeAutostart();
        break;
    case LinboPostProcessActions::Shutdown:
        this->shutdown();
        break;
    case LinboPostProcessActions::Reboot:
        this->reboot();
        break;
    case LinboPostProcessActions::Logout:
        this->_logout(true);
        break;
    default:
        this->logger()->error("Tried to execute a non-existing post process action!");
    };
    if(actionSkipped)
        this->_handleCommandFinishedSuccess();
}

bool LinboBackend::_noMorePostProcessActionsToExecute() {
    return this->_nextAction(this->_postProcessActions) == LinboPostProcessActions::NoAction;
}

bool LinboBackend::_validatePostProcessActions(LinboPostProcessActions::Flags flags) {
    // See comment in linbopostprocessactions.h
    if(flags.testFlag(LinboPostProcessActions::UploadImage)) {
        return !flags.testAnyFlags(LinboPostProcessActions::ExecuteAutoInitCache | LinboPostProcessActions::ExecuteAutostart);
    }
    else if(flags.testAnyFlags(LinboPostProcessActions::ExecuteAutoInitCache | LinboPostProcessActions::ExecuteAutostart)) {
        return !flags.testAnyFlags(
                   LinboPostProcessActions::UploadImage |
                   LinboPostProcessActions::Shutdown |
                   LinboPostProcessActions::Reboot
               ) &&
               flags.testAnyFlag(LinboPostProcessActions::Logout);
    }
    return true;
}

LinboPostProcessActions::Flag LinboBackend::_nextAction(LinboPostProcessActions::Flags postProcessActions) {
    // The order is important! See comment in linbopostprocessactions.h
    QList<LinboPostProcessActions::Flag> flagsToTest = {
        LinboPostProcessActions::UploadImage,
        LinboPostProcessActions::ExecuteAutoInitCache,
        LinboPostProcessActions::ExecuteAutostart,
        LinboPostProcessActions::Shutdown,
        LinboPostProcessActions::Reboot,
        LinboPostProcessActions::Logout
    };

    for(LinboPostProcessActions::Flag flag : flagsToTest) {
        if(postProcessActions.testFlag(flag)) {
            return flag;
        }
    }

    return LinboPostProcessActions::NoAction;
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
