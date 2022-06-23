#include "../../headers/backend/linbocmd.h"
#include "linbobackend.h"

LinboCmd::LinboCmd(LinboLogger* logger, QObject *parent)
    : QObject(parent)
{
    this->_logger = logger;
    this->_stringToMaskInOutput = "";
    // Processes
    this->_asynchronosProcess = new QProcess(this);
    // ascynchorons commands are logged to logger
    connect(this->_asynchronosProcess, &QProcess::readyReadStandardOutput, this, &LinboCmd::_readFromStdout);
    connect(this->_asynchronosProcess, &QProcess::readyReadStandardError, this, &LinboCmd::_readFromStderr);
    connect(this->_asynchronosProcess, &QProcess::finished, this, &LinboCmd::commandFinished);

    // synchronos commands are not logged
    this->_synchronosProcess = new QProcess(this);
}


bool LinboCmd::startOs(LinboOs* os, QString cachePath) {
    return this->executeAsync(
               "start",
               os->bootPartition(),
               os->rootPartition(),
               os->kernel(),
               os->initrd(),
               os->kernelOptions(),
               cachePath
           );
}

bool LinboCmd::syncOs(LinboOs* os, QString serverIP, QString cachePath) {
    return this->executeAsync(
               "syncstart",
               serverIP,
               cachePath,
               os->baseImage()->name(),
               "",
               os->bootPartition(),
               os->rootPartition(),
               os->kernel(),
               os->initrd(),
               os->kernelOptions()
           );
}

bool LinboCmd::reinstallOs(LinboOs* os, QString serverIP, QString cachePath) {
    return this->executeAsync(
               "syncr",
               serverIP,
               cachePath,
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

bool LinboCmd::authenticate(QString password, QString serverIP) {
    int exitCode = this->executeSync("authenticate", serverIP, "linbo", password);
    return exitCode == 0;
}

bool LinboCmd::createImageOfOs(LinboOs* os, QString name, QString cachePath) {
    return this->executeAsync(
               "create",
               cachePath,
               name,
               name,
               os->bootPartition(),
               os->rootPartition(),
               os->kernel(),
               os->initrd()
           );
}

bool LinboCmd::uploadImage(LinboImage *image, QString password, QString serverIP, QString cachePath) {
    return this->executeAsync(
               "upload",
               serverIP,
               "linbo",
               password,
               cachePath,
               image->name()
           );
}

bool LinboCmd::partitionDrive(QList<LinboDiskPartition*> paritions, bool format) {
    QStringList commandArgs = QStringList(format ? "partition":"partition_noformat");
    for(LinboDiskPartition* partiton : paritions) {
        commandArgs
                << partiton->path()
                << QString::number(partiton->size())
                << partiton->id()
                << QString((partiton->bootable())?"bootable":"\" \"")
                << partiton->fstype();
    }

    return this->executeAsync(commandArgs);
}

bool LinboCmd::updateCache(LinboConfig::DownloadMethod downloadMethod, bool format, QList<LinboOs*> operaringSystems, QString serverIP, QString cachePath) {
    QStringList commandArgs;
    commandArgs
            << (format ? "initcache_format":"initcache")
            << serverIP
            << cachePath;


    commandArgs.append(LinboConfig::downloadMethodToString(downloadMethod));

    for(LinboOs* os : operaringSystems) {
        commandArgs
                << os->baseImage()->name()
                << "";
    }

    return this->executeAsync(commandArgs);
}

bool LinboCmd::updateLinbo(QString serverIP, QString cachePath) {
    return this->executeAsync("update", serverIP, cachePath);
}

bool LinboCmd::registerClient(QString room, QString hostname, QString ipAddress, QString hostGroup, LinboConfig::LinboDeviceRole deviceRole, QString password, QString serverIP) {
    return this->executeAsync(
               "register",
               serverIP,
               "linbo",
               password,
               room,
               hostname,
               ipAddress,
               hostGroup,
               LinboConfig::deviceRoleToString(deviceRole)
           );
}

QString LinboCmd::readImageDescription(LinboImage* image, QString cachePath) {
    QProcess readProcess;
    QString description = this->getOutput("readfile", cachePath, image->name() + ".desc");

    if(this->getExitCodeOfLastSyncCommand() == 0)
        return description;
    else
        return "";
}

bool LinboCmd::writeImageDescription(LinboImage* image, QString newDescription, QString cachePath) {
    return this->writeImageDescription(image->name(), newDescription, cachePath);
}

bool LinboCmd::writeImageDescription(QString imageName, QString newDescription, QString cachePath) {

    QProcess process;
    process.start(
        this->_linboCmdCommand,
        this->_buildCommand("writefile", cachePath, imageName + ".desc"));

    if(!process.waitForStarted()) {
        return false;
    }

    process.write(newDescription.toUtf8());

    if(!process.waitForBytesWritten()) {
        return false;
    }

    process.closeWriteChannel();

    if(!process.waitForFinished()) {
        return false;
    }

    return true;
}

bool LinboCmd::executeAsync(QStringList arguments) {
    this->_logExecution(arguments);
    _asynchronosProcess->start(this->_linboCmdCommand, arguments);
    return _asynchronosProcess->waitForStarted();
}

int LinboCmd::executeSync(QStringList arguments) {
    this->_logExecution(arguments);
    // clear old output
    if(this->_synchronosProcess->bytesAvailable())
        this->_synchronosProcess->readAll();

    this->_synchronosProcess->start(this->_linboCmdCommand, arguments);
    this->_synchronosProcess->waitForStarted();

    this->_synchronosProcess->waitForFinished(10000);

    this->_outputOfLastSyncExecution = this->_synchronosProcess->readAll();
    this->_exitCodeOfLastSyncExecution = this->_synchronosProcess->exitCode();
    return this->_exitCodeOfLastSyncExecution;
}

QString LinboCmd::getOutput(QStringList arguments) {
    this->executeSync(arguments);
    return this->_outputOfLastSyncExecution;
}

int LinboCmd::getExitCodeOfLastSyncCommand() {
    return this->_exitCodeOfLastSyncExecution;
}

QString LinboCmd::getOutputOfLastSyncCommand() {
    return this->_outputOfLastSyncExecution;
}

void LinboCmd::killAsyncProcess() {
    this->_asynchronosProcess->kill();
}

void LinboCmd::setStringToMaskInOutput(QString string) {
    this->_stringToMaskInOutput = string;
}

void LinboCmd::_readFromStdout() {
    if(this->_logger == nullptr)
        return;
    QString stdOut = this->_asynchronosProcess->readAllStandardOutput();
    QStringList lines = stdOut.split("\n");
    for(const QString &line : lines) {
        this->_logger->stdOut(line.simplified());
    }
}

void LinboCmd::_readFromStderr() {
    if(this->_logger == nullptr)
        return;
    QString stdOut = this->_asynchronosProcess->readAllStandardError();
    QStringList lines = stdOut.split("\n");
    for(const QString &line : lines) {
        this->_logger->stdErr(line.simplified());
    }
}

QString LinboCmd::_maskString(QString stringToMask) {
    if(!this->_stringToMaskInOutput.isEmpty())
        return stringToMask.replace(this->_stringToMaskInOutput, "***");
    else
        return stringToMask;
}

void LinboCmd::_logExecution(QStringList arguments) {
    if(this->_logger == nullptr)
        return;
    QString argumentsString = this->_maskString(arguments.join(" "));
    this->_logger->info("Executing: " + this->_linboCmdCommand + " " + argumentsString);
}
