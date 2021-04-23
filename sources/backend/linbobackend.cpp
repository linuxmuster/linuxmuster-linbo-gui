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
    this->currentOs = nullptr;
    this->logger = nullptr;
    this->rootPassword = "";
    this->imageToUploadAutomatically = nullptr;
    this->state = Initializing;
    this->postProcessActions = NoAction;

    this->logger = new LinboLogger("/tmp/linbo.log", this);

    // Processes
    this->asynchronosProcess = new QProcess(this);
    // ascynchorons commands are logged to logger
    connect( asynchronosProcess, SIGNAL(readyReadStandardOutput()),
             this, SLOT(readFromStdout()) );
    connect( asynchronosProcess, SIGNAL(readyReadStandardError()),
             this, SLOT(readFromStderr()) );
    connect(this->asynchronosProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(handleProcessFinished(int, QProcess::ExitStatus)));

    // synchronos commands are not logged
    this->synchronosProcess = new QProcess(this);

    this->configReader = new LinboConfigReader(this);
    this->config = this->configReader->readConfig();

    // autostart timers
    this->autostartTimer = new QTimer(this);
    this->autostartTimer->setSingleShot(true);
    connect(this->autostartTimer, SIGNAL(timeout()),
            this, SLOT(handleAutostartTimerTimeout()));

    // root timeout timer
    this->rootTimeoutTimer = new QTimer(this);
    this->rootTimeoutTimer->setSingleShot(true);
    connect(this->rootTimeoutTimer, SIGNAL(timeout()),
            this, SLOT(handleRootTimerTimeout()));

    // timeout progress refresh timer
    this->timeoutRemainingTimeRefreshTimer = new QTimer(this);
    this->timeoutRemainingTimeRefreshTimer->setSingleShot(false);
    this->timeoutRemainingTimeRefreshTimer->setInterval(10);
    connect(this->timeoutRemainingTimeRefreshTimer, &QTimer::timeout,
    [=] {
        if(this->state == Autostarting)
            emit this->autostartTimeoutProgressChanged();
        else if(this->state == RootTimeout)
            emit this->rootTimeoutProgressChanged();
    });

    if(this->config->getGuiDisabled()) {
        this->setState(Disabled);
        this->logger->log("Linbo GUI is disabled", LinboLogger::LinboGuiInfo);
    }

    // triger autostart if necessary
    for(LinboOs* os : this->config->operatingSystems())
        if(os->getAutostartEnabled() && this->currentOs == nullptr)
            // If the autostart of this OS is enabled and there is
            // no other OS active yet -> set this OS as current OS
            this->currentOs = os;

    // default select first OS if no other OS has been selected yet
    if(config->operatingSystems().length() > 0 && this->currentOs == nullptr)
        this->currentOs = this->config->operatingSystems()[0];

    if(this->currentOs != nullptr && this->currentOs->getAutostartEnabled())
        this->executeAutostart();
    else
        this->setState(Idle);
}

// --------------------
// - Public functions -
// --------------------

void LinboBackend::executeAutostart() {
    if(!this->currentOs->getActionEnabled(this->currentOs->getDefaultAction())) {
        this->setState(Idle);
        return;
    }

    this->setState(Autostarting);
    this->logger->log("Beginning autostart timeout for " + this->currentOs->getName(), LinboLogger::LinboGuiInfo);
    this->autostartTimer->setInterval(this->currentOs->getAutostartTimeout() * 1000);
    this->autostartTimer->start();
    this->timeoutRemainingTimeRefreshTimer->start();
}

void LinboBackend::handleAutostartTimerTimeout() {
    this->timeoutRemainingTimeRefreshTimer->stop();
    this->logger->log("Executing autostart for " + this->currentOs->getName(), LinboLogger::LinboGuiInfo);

    LinboOs::LinboOsStartAction defaultAction = this->currentOs->getDefaultAction();
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
        this->logger->log("Executed autostart successfully!", LinboLogger::LinboGuiInfo);
    else
        this->logger->log("An error occured when executing autostart for " + this->currentOs->getName(), LinboLogger::LinboGuiError);
}

void LinboBackend::handleRootTimerTimeout() {
    this->timeoutRemainingTimeRefreshTimer->stop();
    qDebug() << "timeout stop";
    if(this->state == Root) {
        // timeout for the first time -> switch to timeout state
        this->restartRootTimeout();
        this->setState(RootTimeout);
    }
    else if(this->state == RootTimeout) {
        // timeout for the second time -> logout
        this->logout();
    }
    else if(this->state > Root) {
        // non-matching state
        this->restartRootTimeout();
    }

}

void LinboBackend::shutdown() {
    this->executeCommand(false, "busybox", QStringList("poweroff"));
}

void LinboBackend::reboot() {
    this->executeCommand(false, "busybox", QStringList("reboot"));
}

bool LinboBackend::startCurrentOs() {
    LinboOs* os = this->currentOs;

    if(os == nullptr || (this->state != Idle && this->state != Autostarting) || !os->getActionEnabled(LinboOs::StartOs))
        return false;

    this->logger->log("Starting " + this->currentOs->getName(), LinboLogger::LinboLogChapterBeginning);

    this->setState(Starting);

    this->executeCommand(
        false,
        "start",
        os->getBootPartition(),
        os->getRootPartition(),
        os->getKernel(),
        os->getInitrd(),
        os->getKernelOptions(),
        this->config->getCachePath()
    );

    return true;
}

bool LinboBackend::syncCurrentOs() {
    LinboOs* os = this->currentOs;

    if(os == nullptr || (this->state != Idle && this->state != Autostarting) || !os->getActionEnabled(LinboOs::SyncOs))
        return false;

    this->logger->log("Syncing " + this->currentOs->getName(), LinboLogger::LinboLogChapterBeginning);

    this->setState(Syncing);

    this->executeCommand(
        false,
        "syncstart",
        this->config->getServerIpAddress(),
        this->config->getCachePath(),
        os->getBaseImage()->getName(),
        "",
        os->getBootPartition(),
        os->getRootPartition(),
        os->getKernel(),
        os->getInitrd(),
        os->getKernelOptions()
    );

    return true;
}

bool LinboBackend::reinstallCurrentOs() {
    LinboOs* os = this->currentOs;

    if(os == nullptr || (this->state != Idle && this->state != Autostarting) || !os->getActionEnabled(LinboOs::ReinstallOs))
        return false;

    this->logger->log("Reinstalling " + this->currentOs->getName(), LinboLogger::LinboLogChapterBeginning);

    this->setState(Reinstalling);

    this->executeCommand(
        false,
        "syncr",
        this->config->getServerIpAddress(),
        this->config->getCachePath(),
        os->getBaseImage()->getName(),
        "",
        os->getBootPartition(),
        os->getRootPartition(),
        os->getKernel(),
        os->getInitrd(),
        os->getKernelOptions(),
        QString("force")
    );

    return true;
}

bool LinboBackend::login(QString password) {
    if(this->state != Idle)
        return false;

    this->logger->log("Authenticating with password.", LinboLogger::LinboLogChapterBeginning);

    this->executeCommand(true, "authenticate", this->config->getServerIpAddress(), "linbo", password, "linbo");
    bool successfull = this->synchronosProcess->exitCode() == 0;

    if(successfull) {
        this->rootPassword = password;
        this->setState(Root);
        this->restartRootTimeout();
    }

    this->logger->log("Authentication " + QString(successfull ? "OK":"FAILED"), LinboLogger::LinboLogChapterEnd);

    return successfull;
}

void LinboBackend::logout() {
    if(this->state != Root && this->state != RootTimeout)
        return;

    this->rootPassword.clear();
    this->setState(Idle);
}

void LinboBackend::restartRootTimeout() {
    if(this->state == Root && this->config->getRootTimeout() > 0) {
        this->rootTimeoutTimer->start((this->config->getRootTimeout() * 1000) / 2);
        this->timeoutRemainingTimeRefreshTimer->start();
    }
}

bool LinboBackend::replaceImageOfCurrentOs(QString description, LinboPostProcessActions postProcessAction) {
    return this->createImageOfCurrentOS(this->currentOs->getBaseImage()->getName(), description, postProcessAction);
}

bool LinboBackend::createImageOfCurrentOS(QString name, QString description, LinboPostProcessActions postProcessActions) {
    if(this->state != Root)
        return false;

    this->postProcessActions = postProcessActions;

    this->logger->log("Creating image", LinboLogger::LinboLogChapterBeginning);
    this->setState(CreatingImage);

    this->logger->log("Writing image description", LinboLogger::LinboGuiInfo);
    if(!this->writeImageDescription(name, description))
        this->logger->log("Error writing image description, continuing anyway...", LinboLogger::LinboGuiError);

    if(this->postProcessActions.testFlag(UploadImage) && name == this->currentOs->getBaseImage()->getName()) {
        this->imageToUploadAutomatically = this->currentOs->getBaseImage();
    }
    else if(this->postProcessActions.testFlag(UploadImage)) {
        this->imageToUploadAutomatically = new LinboImage(name);
    }


    this->logger->log("Beginning image creation...", LinboLogger::LinboGuiInfo);
    this->executeCommand(
        false,
        "create",
        this->config->getCachePath(),
        name,
        name,
        this->currentOs->getBootPartition(),
        this->currentOs->getRootPartition(),
        this->currentOs->getKernel(),
        this->currentOs->getInitrd()
    );

    return true;
}

QString LinboBackend::readImageDescription(LinboImage* image) {
    QProcess readProcess;
    int exitCode = -1;
    QString description = this->executeCommand(
                              true,
                              this->linboCmdCommand,
                              this->buildCommand("readfile", this->config->getCachePath(), image->getName() + ".desc"),
                              &exitCode);

    if(exitCode == 0)
        return description;
    else
        return "";
}

bool LinboBackend::writeImageDescription(LinboImage* image, QString newDescription) {
    return this->writeImageDescription(image->getName(), newDescription);
}

bool LinboBackend::writeImageDescription(QString imageName, QString newDescription) {

    QProcess process;
    process.start(
        this->linboCmdCommand,
        this->buildCommand("writefile", this->config->getCachePath(), imageName + ".desc"));

    if(!process.waitForStarted()) {
        this->logger->log("Description writer didn't start: " + QString::number(process.exitCode()), LinboLogger::LinboGuiError);
        return false;
    }

    process.write(newDescription.toUtf8());

    if(!process.waitForBytesWritten()) {
        this->logger->log("Description writer didn't write: " + QString::number(process.exitCode()), LinboLogger::LinboGuiError);
        return false;
    }

    process.closeWriteChannel();

    if(!process.waitForFinished()) {
        this->logger->log("Description writer didn't finish: " + QString::number(process.exitCode()), LinboLogger::LinboGuiError);
        return false;
    }

    return true;
}

bool LinboBackend::uploadImage(const LinboImage* image, LinboPostProcessActions postProcessActions) {
    return this->uploadImagePrivate(image, postProcessActions, false);
}

bool LinboBackend::uploadImagePrivate(const LinboImage* image, LinboPostProcessActions postProcessActions, bool allowCreatingImageState) {
    if(this->state != Root && !(this->state == CreatingImage && allowCreatingImageState))
        return false;

    if(image == nullptr)
        return false;

    this->postProcessActions = postProcessActions;

    this->logger->log("Uploading image", LinboLogger::LinboLogChapterBeginning);

    this->setState(UploadingImage);

    this->executeCommand(
        false,
        "upload",
        this->config->getServerIpAddress(),
        "linbo",
        this->rootPassword,
        this->config->getCachePath(),
        image->getName()
    );

    return true;
}

bool LinboBackend::partitionDrive(bool format) {
    if(this->state != Root)
        return false;

    this->logger->log("Partitioning drive", LinboLogger::LinboLogChapterBeginning);

    this->setState(Partitioning);

    QStringList commandArgs = QStringList(format ? "partition":"partition_noformat");
    for( int i=0; i < this->config->diskPartitions().length(); i++) {
        LinboDiskPartition* p = this->config->diskPartitions()[i];
        commandArgs.append(
            this->buildCommand(
                p->getPath(),
                QString::number(p->getSize()),
                p->getId(),
                QString((p->getBootable())?"bootable":"\" \""),
                p->getFstype()
            )
        );
    }

    this->executeCommand(false, this->linboCmdCommand, commandArgs);

    return true;
}

bool LinboBackend::updateCache(LinboConfig::DownloadMethod downloadMethod, bool format) {
    if(this->state != Root)
        return false;

    this->logger->log("Updating cache", LinboLogger::LinboLogChapterBeginning);
    this->setState(UpdatingCache);

    QStringList commandArgs = this->buildCommand(format ? "initcache_format":"initcache", config->getServerIpAddress(), config->getCachePath());

    if( downloadMethod < LinboConfig::Rsync || downloadMethod > LinboConfig::Torrent )
        commandArgs.append(LinboConfig::downloadMethodToString(this->config->getDownloadMethod()));
    else
        commandArgs.append(LinboConfig::downloadMethodToString(downloadMethod));

    for(int i = 0; i < this->config->operatingSystems().length(); i++) {
        LinboOs* os = this->config->operatingSystems()[i];
        commandArgs.append(this->buildCommand(os->getBaseImage()->getName()));
        commandArgs.append(this->buildCommand(""));
    }

    this->executeCommand(false, this->linboCmdCommand, commandArgs);

    return true;
}

bool LinboBackend::updateLinbo() {
    if(this->state != Root)
        return false;

    this->executeCommand(false, "update", this->config->getServerIpAddress(), this->config->getCachePath());

    return true;
}

bool LinboBackend::registerClient(QString room, QString hostname, QString ipAddress, QString hostGroup, LinboConfig::LinboDeviceRole deviceRole) {
    if(this->state != Root)
        return false;

    this->logger->log("Registering client", LinboLogger::LinboLogChapterBeginning);

    this->setState(Registering);

    this->executeCommand(false, "register", this->config->getServerIpAddress(), "linbo", this->rootPassword, room, hostname, ipAddress, hostGroup, LinboConfig::deviceRoleToString(deviceRole));

    return true;
}

bool LinboBackend::cancelCurrentAction() {
    switch (this->state) {
    case Autostarting:
        this->logger->log("Cancelling autostart", LinboLogger::LinboGuiInfo);
        this->autostartTimer->stop();
        this->timeoutRemainingTimeRefreshTimer->stop();
        this->setState(Idle);
        return true;

    case Starting:
    case Syncing:
    case Reinstalling:
        this->logger->log("Cancelling current start action: " + QString::number(this->state), LinboLogger::LinboGuiInfo);
        this->asynchronosProcess->kill();
        this->setState(Idle);
        return true;

    case RootTimeout:
        this->logger->log("Cancelling root timeout", LinboLogger::LinboGuiInfo);
        this->setState(Root);
        this->restartRootTimeout();
        return true;

    case Partitioning:
    case UpdatingCache:
    case CreatingImage:
    case UploadingImage:
        this->logger->log("Cancelling current action: " + QString::number(this->state), LinboLogger::LinboGuiInfo);
        this->asynchronosProcess->kill();
        this->setState(Root);
        return true;

    default:
        this->logger->log("Cannot cancel current action: " + QString::number(this->state), LinboLogger::LinboGuiError);
        return false;
    }
}


bool LinboBackend::resetMessage() {
    switch (this->state) {
    case StartActionError:
        this->setState(Idle);
        break;
    case RootActionError:
        this->setState(Root);
        break;
    case RootActionSuccess:
        this->setState(Root);
        break;
    default:
        return false;
    }

    return true;
}

LinboLogger* LinboBackend::getLogger() {
    return this->logger;
}

LinboConfig* LinboBackend::getConfig() {
    return this->config;
}

LinboOs* LinboBackend::getCurrentOs() {
    return this->currentOs;
}

void LinboBackend::setCurrentOs(LinboOs* os) {
    if((this->state != Idle && this->state != Root) || !this->config->operatingSystems().contains(os) || this->currentOs == os)
        return;

    this->currentOs = os;
    emit this->currentOsChanged(os);
}

double LinboBackend::getAutostartTimeoutProgress() {
    return this->autostartTimer->isActive() ? 1 - double(this->autostartTimer->remainingTime()) / double(this->autostartTimer->interval()) : 1;
}

int LinboBackend::getAutostartTimeoutRemainingSeconds() {
    return this->autostartTimer->isActive() ? this->autostartTimer->remainingTime() / 1000 : 0;
}

double LinboBackend::getRootTimeoutProgress() {
    return this->rootTimeoutTimer->isActive() ? 1 - double(this->rootTimeoutTimer->remainingTime()) / double(this->rootTimeoutTimer->interval()) : 1;
}

int LinboBackend::getRootTimeoutRemainingSeconds() {
    return this->rootTimeoutTimer->isActive() ? this->rootTimeoutTimer->remainingTime() / 1000 : 0;
}


// -----------
// - Helpers -
// -----------

QString LinboBackend::executeCommand(bool waitForFinished, QString command, QStringList commandArgs, int* returnCode) {
    if(this->state >= Root)
        // args are not logged for security
        this->logger->log("Executing " + QString(waitForFinished ? "synchronos":"asynchronos") + ": " + command, LinboLogger::LinboGuiInfo);
    else
        this->logger->log("Executing " + QString(waitForFinished ? "synchronos":"asynchronos") + ": " + command + " " + commandArgs.join(" "), LinboLogger::LinboGuiInfo);

    if(waitForFinished) {
        // clear old output
        if(this->synchronosProcess->bytesAvailable())
            this->synchronosProcess->readAll();

        this->synchronosProcess->start(command, commandArgs);
        this->synchronosProcess->waitForStarted();

        this->synchronosProcess->waitForFinished(10000);

        if(returnCode != nullptr)
            *returnCode = this->synchronosProcess->exitCode();
        else if(this->synchronosProcess->exitCode() != 0)
            return "";

        return this->synchronosProcess->readAllStandardOutput();
    }
    else {
        asynchronosProcess->start(command, commandArgs);
        asynchronosProcess->waitForStarted();

        return "";
    }
}

void LinboBackend::readFromStdout() {
    QString stdOut = this->asynchronosProcess->readAllStandardOutput();
    QStringList lines = stdOut.split("\n");
    for(QString line : lines) {
        this->logger->log(line.simplified(), LinboLogger::StdOut);
    }
}

void LinboBackend::readFromStderr() {
    QString stdOut = this->asynchronosProcess->readAllStandardError();
    QStringList lines = stdOut.split("\n");
    for(QString line : lines) {
        this->logger->log(line.simplified(), LinboLogger::StdErr);
    }
}

void LinboBackend::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitStatus)
    if(exitCode == 0) {
        this->logger->log("Process exited normally.", LinboLogger::LinboGuiInfo);

        if(this->postProcessActions.testFlag(NoAction)) {
            // do nothing
        }
        else if(this->postProcessActions.testFlag(UploadImage)) {
            this->logger->log("Process exited.", LinboLogger::LinboLogChapterEnd);
            this->postProcessActions = this->postProcessActions.setFlag(UploadImage, false);
            this->uploadImagePrivate(this->imageToUploadAutomatically, this->postProcessActions, true);
            this->imageToUploadAutomatically = nullptr;
            return;
        }
        else if(this->postProcessActions.testFlag(Shutdown)) {
            this->shutdown();
        }
        else if(this->postProcessActions.testFlag(Reboot)) {
            this->reboot();
        }
        else if (this->postProcessActions.testFlag(Logout) && this->state >= Root) {
            this->postProcessActions = NoAction;
            this->setState(Root);
            this->logger->log("Process exited.", LinboLogger::LinboLogChapterEnd);
            this->logout();
            return;
        }

        this->postProcessActions = NoAction;

        if(this->state > Root)
            this->setState(RootActionSuccess);
    }
    else {
        this->logger->log("Process exited with an error.", LinboLogger::LinboGuiError);

        this->postProcessActions = NoAction;

        if(this->state > Root)
            this->setState(RootActionError);
        else if(this->state != Root && this->state != Idle) // prevent showing an error when the process was cancelled
            this->setState(StartActionError);
    }

    this->logger->log("Process exited.", LinboLogger::LinboLogChapterEnd);
}

LinboBackend::LinboState LinboBackend::getState() {
    return this->state;
}

void LinboBackend::setState(LinboState state) {
    if(this->state == state)
        return;

    this->state = state;
    emit this->stateChanged(this->state);

    if(this->logger != nullptr)
        this->logger->log("Linbo state changed to: " + QString::number(state), LinboLogger::LinboGuiInfo);
}
