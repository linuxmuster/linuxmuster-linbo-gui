/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020  Dorian Zedler <dorian@itsblue.de>
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
    this->currentOs = nullptr;
    this->logger = nullptr;
    this->rootPassword = "";
    this->state = Initializing;

    // Init some objects
    this->logger = new LinboLogger("/tmp/linbo.log", this);
    this->config = new LinboConfig(this);

    // autostart timers
    this->autostartTimer = new QTimer(this);
    this->autostartTimer->setSingleShot(true);
    connect(this->autostartTimer, SIGNAL(timeout()),
            this, SLOT(handleAutostartTimerTimeout()));

    this->autostartRemainingTimeRefreshTimer = new QTimer(this);
    this->autostartRemainingTimeRefreshTimer->setSingleShot(false);
    this->autostartRemainingTimeRefreshTimer->setInterval(10);
    connect(this->autostartRemainingTimeRefreshTimer, SIGNAL(timeout()),
            this, SIGNAL(autostartTimeoutProgressChanged()));

    this->autostartElapsedTimer = new QElapsedTimer();

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

    // load all configuration
#ifdef TEST_ENV
    this->loadStartConfiguration(TEST_ENV"/start.conf");
#else
    this->loadStartConfiguration("start.conf");
#endif

    this->loadEnvironmentValues();

    // triger autostart if necessary
    if(this->currentOs != nullptr && this->currentOs->getAutostartEnabled())
        this->executeAutostart();
    else
        this->setState(Idle);
}

// --------------------
// - Public functions -
// --------------------

void LinboBackend::executeAutostart() {
    if(this->currentOs->getDefaultAction() == LinboOs::UnknownAction) {
        this->setState(Idle);
        return;
    }

    this->setState(Autostarting);
    this->logger->log("Beginning autostart timeout for " + this->currentOs->getName(), LinboLogType::LinboGuiInfo);
    this->autostartTimer->setInterval(this->currentOs->getAutostartTimeout() * 1000);
    this->autostartTimer->start();
    this->autostartElapsedTimer->restart();
    this->autostartRemainingTimeRefreshTimer->start();
}

void LinboBackend::handleAutostartTimerTimeout() {
    this->autostartElapsedTimer->invalidate();
    this->autostartRemainingTimeRefreshTimer->stop();
    this->logger->log("Executing autostart for " + this->currentOs->getName(), LinboLogType::LinboGuiInfo);

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
    default: break;
    }

    if(wasSuccessfull)
        this->logger->log("Executed autostart successfully!", LinboLogType::LinboGuiInfo);
    else
        this->logger->log("An error occured when executing autostart for " + this->currentOs->getName(), LinboLogType::LinboGuiError);
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

    this->logger->log("Starting " + this->currentOs->name, LinboLogType::LinboLogChapterBeginning);

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

    this->logger->log("Syncing " + this->currentOs->name, LinboLogType::LinboLogChapterBeginning);

    this->setState(Syncing);

    qDebug() << "TEST2";

    QString differentialImage;
    if(os->getDifferentialImage() == nullptr)
        differentialImage = "";
    else
        differentialImage = os->getDifferentialImage()->getName();

    this->executeCommand(
                false,
                "syncstart",
                this->config->getServerIpAddress(),
                this->config->getCachePath(),
                os->getBaseImage()->getName(),
                differentialImage,
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

    this->logger->log("Reinstalling " + this->currentOs->name, LinboLogType::LinboLogChapterBeginning);

    this->setState(Reinstalling);

    QString differentialImage;
    if(os->getDifferentialImage() == nullptr)
        differentialImage = "";
    else
        differentialImage = os->getDifferentialImage()->getName();

    this->executeCommand(
                false,
                "syncr",
                this->config->getServerIpAddress(),
                this->config->getCachePath(),
                os->getBaseImage()->getName(),
                differentialImage,
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

    this->logger->log("Authenticating with password: " + password, LinboLogType::LinboLogChapterBeginning);

    this->executeCommand(true, "authenticate", this->config->getServerIpAddress(), "linbo", password, "linbo");
    bool successfull = this->synchronosProcess->exitCode() == 0;

    if(successfull) {
        this->rootPassword = password;
        this->setState(Root);
    }

    this->logger->log("Authentication " + QString(successfull ? "OK":"FAILED"), LinboLogType::LinboLogChapterEnd);

    return successfull;
}

void LinboBackend::logout() {
    if(this->state != Root)
        return;

    this->rootPassword.clear();
    this->setState(Idle);
}

bool LinboBackend::partitionDrive(bool format) {
    if(this->state != Root)
        return false;

    this->logger->log("Partitioning drive", LinboLogType::LinboLogChapterBeginning);

    this->setState(Partitioning);

    QStringList commandArgs = QStringList(format ? "partition":"partition_noformat");
    for( int i=0; i < this->diskPartitions.length(); i++) {
        LinboDiskPartition* p = this->diskPartitions[i];
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

    this->logger->log("Updating cache", LinboLogType::LinboLogChapterBeginning);
    this->setState(UpdatingCache);

    QStringList commandArgs = this->buildCommand(format ? "initcache_format":"initcache", config->getServerIpAddress(), config->getCachePath());

    if( downloadMethod < LinboConfig::Rsync || downloadMethod > LinboConfig::Torrent )
        commandArgs.append(this->downloadMethodToString(this->config->getDownloadMethod()));
    else
        commandArgs.append(this->downloadMethodToString(downloadMethod));

    for(int i = 0; i < this->operatingSystems.length(); i++) {
        LinboOs* os = this->operatingSystems[i];
        commandArgs.append(this->buildCommand(os->getBaseImage()->getName()));
        if(os->getDifferentialImage() != nullptr)
            commandArgs.append(this->buildCommand(os->getDifferentialImage()->getName()));
        else
            commandArgs.append(this->buildCommand(""));
        /* TODO ?? for(unsigned int j = 0; j < os[i].image_history.size(); j++) {
          saveappend( command, os[i].image_history[j].get_image() );
        }*/
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

bool LinboBackend::registerClient(QString room, QString hostname, QString ipAddress, QString hostGroup, LinboDeviceRole deviceRole) {
    if(this->state != Root)
        return false;

    this->logger->log("Registering client", LinboLogType::LinboLogChapterBeginning);

    this->setState(Registering);

    this->executeCommand(false, "register", this->config->getServerIpAddress(), "linbo", this->rootPassword, room, hostname, ipAddress, hostGroup, this->deviceRoleToString(deviceRole));

    return true;
}

bool LinboBackend::cancelCurrentAction() {
    switch (this->state) {
    case Autostarting:
        this->logger->log("Cancelling autostart", LinboLogType::LinboGuiInfo);
        this->autostartTimer->stop();
        this->autostartRemainingTimeRefreshTimer->stop();
        this->autostartElapsedTimer->invalidate();
        this->setState(Idle);
        return true;

    case Starting:
    case Syncing:
    case Reinstalling:
        this->logger->log("Cancelling current start action: " + QString::number(this->state), LinboLogType::LinboGuiInfo);
        this->asynchronosProcess->kill();
        this->setState(Idle);
        return true;

    case Partitioning:
    case UpdatingCache:
        this->logger->log("Cancelling current action: " + QString::number(this->state), LinboLogType::LinboGuiInfo);
        this->asynchronosProcess->kill();
        this->setState(Root);
        return true;

    default:
        this->logger->log("Cannot cancel current action: " + QString::number(this->state), LinboLogType::LinboGuiError);
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

QList<LinboOs*> LinboBackend::getOperatingSystems() {
    return this->operatingSystems;
}

LinboOs* LinboBackend::getCurrentOs() {
    return this->currentOs;
}

void LinboBackend::setCurrentOs(LinboOs* os) {
    if((this->state != Idle && this->state != Root) || !this->operatingSystems.contains(os) || this->currentOs == os)
        return;

    this->currentOs = os;
    emit this->currentOsChanged(os);
}


double LinboBackend::getAutostartTimeoutProgress() {
    return this->autostartTimer->isActive() ? double(this->autostartElapsedTimer->elapsed()) / double(this->autostartTimer->interval()) : 1;
}

int LinboBackend::getAutostartTimeoutRemainingSeconds() {
    return this->autostartTimer->isActive() ? (this->autostartTimer->interval() - this->autostartElapsedTimer->elapsed()) / 1000 : 0;
}

// -----------
// - Helpers -
// -----------

QString LinboBackend::executeCommand(bool waitForFinished, QString command, QStringList commandArgs) {

#ifdef TEST_ENV
    command = TEST_ENV"/" + command;
#else
#endif

    this->logger->log("Executing " + QString(waitForFinished ? "synchronos":"asynchronos") + ": " + command + " " + commandArgs.join(" "), LinboLogType::LinboGuiInfo);

    if(waitForFinished) {
        // clear old output
        if(this->synchronosProcess->bytesAvailable())
            this->synchronosProcess->readAll();

        this->synchronosProcess->start(command, commandArgs);
        this->synchronosProcess->waitForStarted();

        this->synchronosProcess->waitForFinished(10000);
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
        this->logger->log(line.simplified(), LinboLogType::StdOut);
    }
}

void LinboBackend::readFromStderr() {
    QString stdOut = this->asynchronosProcess->readAllStandardError();
    QStringList lines = stdOut.split("\n");
    for(QString line : lines) {
        this->logger->log(line.simplified(), LinboLogType::StdErr);
    }
}

void LinboBackend::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitStatus)
    if(exitCode == 0) {
        this->logger->log("Process exited normally.", LinboLogType::LinboGuiInfo);
        if(this->state > Root)
            this->setState(RootActionSuccess);
    }
    else {
        this->logger->log("Process exited with an error.", LinboLogType::LinboGuiError);

        if(this->state > Root)
            this->setState(RootActionError);
        else if(this->state != Root && this->state != Idle) // prevent showing an error when the process was cancelled
            this->setState(StartActionError);
    }

    this->logger->log("Process exited.", LinboLogType::LinboLogChapterEnd);
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
        this->logger->log("Linbo state changed to: " + QString::number(state), LinboLogType::LinboGuiInfo);
}

void LinboBackend::loadStartConfiguration(QString startConfFilePath) {
    // read start.conf
    this->logger->log("Starting to parse start.conf", LinboLogType::LinboGuiInfo);

    QFile inputFile(startConfFilePath);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream input(&inputFile);
        QString currentSection;
        QMap<QString, QString> linboConfig;
        QList<QMap<QString, QString>> partitionConfigs;
        QList<QMap<QString, QString>> osConfigs;
        bool firstLineOfSection = false;

        while(!input.atEnd()) {
            QString thisLine = input.readLine();

            // Remove comments
            thisLine = thisLine.split("#")[0];
            // ignore capitalization
            thisLine = thisLine.toLower();
            // remove empty characters
            thisLine = thisLine.simplified();
            // ignore empty lines
            if(thisLine.length() == 0)
                continue;

            //qDebug() << "reading line: " << thisLine;

            if(thisLine.startsWith("[")) {
                // we found a new section!
                currentSection = thisLine;
                firstLineOfSection = true;
                continue;
            }

            // parse key value pair
            // ignore invalid lines
            if(!thisLine.contains("=") || thisLine.startsWith("="))
                continue;

            // split string at
            QStringList keyValueList = thisLine.split("=");

            if(keyValueList.length() < 2)
                continue;

            QString key = keyValueList[0].simplified();
            QString value = keyValueList[1].simplified();

            // ignore empty keys and values
            if(key.isEmpty() || value.isEmpty())
                continue;

            // insert the values into our internal objects
            if(currentSection == "[linbo]") {
                // we are parsing the linbo config
                linboConfig.insert(key, value);
            }
            else if(currentSection == "[partition]") {
                // we are parsing a partition block
                if(firstLineOfSection)
                    partitionConfigs.append(QMap<QString, QString>());

                partitionConfigs.last().insert(key, value);
            }
            else if(currentSection == "[os]") {
                // we are parsing an os block
                if(firstLineOfSection)
                    osConfigs.append(QMap<QString, QString>());

                osConfigs.last().insert(key, value);
            }
            // ignore everything else

            firstLineOfSection = false;
        }

        inputFile.close();

        // write the config our internal objects
        this->writeToLinboConfig(linboConfig, this->config);

        for(QMap<QString, QString> partitionConfig : partitionConfigs) {
            LinboDiskPartition* tmpPartition = new LinboDiskPartition(this);
            this->writeToPartitionConfig(partitionConfig, tmpPartition);
            if(tmpPartition->getPath() != "")
                this->diskPartitions.append(tmpPartition);
            else
                tmpPartition->deleteLater();
        }

        for(QMap<QString, QString> osConfig : osConfigs) {
            LinboOs* tmpOs = new LinboOs(this);
            this->writeToOsConfig(osConfig, tmpOs);
            if(tmpOs->getName() != "") {
                this->operatingSystems.append(tmpOs);
                if(tmpOs->getAutostartEnabled() && this->currentOs == nullptr)
                    // If the autostart of this OS is enabled and there is
                    // no other OS active yet -> set this OS as current OS
                    this->currentOs = tmpOs;

                // check if this is an additional/incremental image for an existing OS
                /* TODO unsigned int i; // Being checked later.
                for(i = 0; i < elements.size(); i++ ) {
                if(tmp_os.get_name().lower().compare(elements[i].get_name().lower()) == 0) {
                elements[i].image_history.push_back(tmp_image); break;
                }
                }
                if(i==elements.size()) { // Not included yet -> new image
                tmp_os.image_history.push_back(tmp_image);
                elements.push_back(tmp_os);
                }*/

            }
            else
                tmpOs->deleteLater();
        }

        // default select first OS if no other OS has been selected yet
        if(this->operatingSystems.length() > 0 && this->currentOs == nullptr)
            this->currentOs = this->operatingSystems[0];
    }
    else
        this->logger->log("Error opening the start configuration file: " + startConfFilePath, LinboLogType::LinboGuiError);

    this->logger->log("Finished parsing start.conf", LinboLogType::LinboGuiInfo);
}

void LinboBackend::loadEnvironmentValues() {
    this->logger->log("Loading environment values", LinboLogType::LinboGuiInfo);
    //  client ip
    this->config->setIpAddress(this->executeCommand(true, "ip").replace("\n", ""));

    // subnet mask
    this->config->setSubnetMask(this->executeCommand(true, "netmask").replace("\n", ""));

    // mac address
    this->config->setMacAddress(this->executeCommand(true, "mac").replace("\n", ""));

    // Version
    this->config->setLinboVersion(this->executeCommand(true, "version").simplified().replace("\n", ""));

    // hostname
    this->config->setHostname(this->executeCommand(true, "hostname").replace("\n", ""));

    // CPU
    this->config->setCpuModel(this->executeCommand(true, "cpu").replace("\n", ""));

    // Memory
    this->config->setRamSize(this->executeCommand(true, "memory").replace("\n", ""));

    // Cache Size
    this->config->setCacheSize(this->executeCommand(true, "size", this->config->getCachePath()).replace("\n", ""));

    // Harddisk Size
    QRegExp *removePartition = new QRegExp("[0-9]{1,2}");
    QString hd = this->config->getCachePath();
    // e.g. turn /dev/sda1 into /dev/sda
    hd.remove( *removePartition );
    this->config->setHddSize(this->executeCommand(true, "size", hd).replace("\n", ""));

    this->logger->log("Finished loading environment values", LinboLogType::LinboGuiInfo);
}

void LinboBackend::writeToLinboConfig(QMap<QString, QString> config, LinboConfig* linboConfig) {
    for(QString key : config.keys()) {
        QString value = config[key];

        if(key == "server")  linboConfig->setServerIpAddress(value);
        else if(key == "cache")   linboConfig->setCachePath(value);
        else if(key == "roottimeout")   linboConfig->setRootTimeout((unsigned int)value.toInt());
        else if(key == "group")   linboConfig->setHostGroup(value);
        else if(key == "autopartition")  linboConfig->setAutoPartition(stringToBool(value));
        else if(key == "autoinitcache")  linboConfig->setAutoInitCache(stringToBool(value));
        else if(key == "autoformat")  linboConfig->setAutoFormat(stringToBool(value));
        else if(key == "backgroundfontcolor")  linboConfig->setBackgroundFontcolor(value);
        else if(key == "consolefontcolorstdout")  linboConfig->setConsoleFontcolorStdout(value);
        else if(key == "consolefontcolorstderr")  linboConfig->setConsoleFontcolorStderr(value);
        else if(key == "usemulticast") {
            if(value.toInt() == 0)
                linboConfig->setDownloadMethod(LinboConfig::Rsync);
            else
                linboConfig->setDownloadMethod(LinboConfig::Multicast);
        }
        else if(key == "downloadtype")  linboConfig->setDownloadMethod(this->stringToDownloadMethod(value));
    }
}

void LinboBackend::writeToPartitionConfig(QMap<QString, QString> config, LinboDiskPartition* partition) {
    for(QString key : config.keys()) {
        QString value = config[key];
        if(key == "dev") partition->setPath(value);
        else if(key == "size")  partition->setSize(value.toInt());
        else if(key == "id")  partition->setId(value);
        else if(key == "fstype")  partition->setFstype(value);
        else if(key.startsWith("bootable"))  partition->setBootable(stringToBool(value));
    }
}

void LinboBackend::writeToOsConfig(QMap<QString, QString> config, LinboOs* os) {
    for(QString key : config.keys()) {
        QString value = config[key];
        if(key == "name")               os->setName(value);
        else if(key == "description")   os->setDescription(value);
        else if(key == "version")       os->setVersion(value);
        else if(key == "iconname")      os->setIconName(value);
        else if(key == "image")         os->setDifferentialImage(new LinboImage(value, os));
        else if(key == "baseimage")     os->setBaseImage(new LinboImage(value, os));
        else if(key == "boot")          os->setBootPartition(value);
        else if(key == "root")          os->setRootPartition(value);
        else if(key == "kernel")        os->setKernel(value);
        else if(key == "initrd")        os->setInitrd(value);
        else if(key == "append")        os->setKernelOptions(value);
        else if(key == "syncenabled")   os->setSyncButtonEnabled(stringToBool(value));
        else if(key == "startenabled")  os->setStartButtonEnabled(stringToBool(value));
        else if((key == "remotesyncenabled") || (key == "newenabled"))   os->setReinstallButtonEnabled(stringToBool(value));
        else if(key == "defaultaction") os->setDefaultAction(os->startActionFromString(value));
        else if(key == "autostart")     os->setAutostartEnabled(stringToBool(value));
        else if(key == "autostarttimeout")   os->setAutostartTimeout(value.toInt());
        else if(key == "hidden")        os->setHidden(stringToBool(value));
    }
}

bool LinboBackend::stringToBool(const QString& value) {
    QStringList trueWords("yes");
    trueWords.append("true");
    trueWords.append("enable");
    return trueWords.contains(value.simplified());
}


LinboConfig::DownloadMethod LinboBackend::stringToDownloadMethod(const QString& value) {
    if(value == "rsync")
        return LinboConfig::Rsync;
    else if(value == "multicast")
        return LinboConfig::Multicast;
    else if(value == "torrent")
        return LinboConfig::Torrent;
    else
        return LinboConfig::Rsync;
}

QString LinboBackend::downloadMethodToString(const LinboConfig::DownloadMethod& value) {
    switch (value) {
        case LinboConfig::Rsync:
        return "rsync";
    case LinboConfig::Multicast:
        return "multicast";
    case LinboConfig::Torrent:
        return "torrent";
    default:
        return "rsync";
    }
}


QString LinboBackend::deviceRoleToString(const LinboDeviceRole& deviceRole) {
    switch (deviceRole) {
    case ClassroomStudentComputerRole:
        return "classroom-studentcomputer";
    case ClassroomTeacherComputerRole:
        return "classroom-teachercomputer";
    case FacultyTeacherComputerRole:
        return "faculty-teachercomputer";
    case StaffComputerRole:
        return "staffcomputer";
    default:
        return "";
    }
}
