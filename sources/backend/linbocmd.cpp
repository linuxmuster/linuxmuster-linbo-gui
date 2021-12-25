#include "../../headers/backend/linbocmd.h"
#include "linbobackend.h"

LinboCmd::LinboCmd(LinboBackend *parent)
    : QObject(parent)
{
    this->_stringToMaskInOutput = "";
    this->_backend = parent;
    // Processes
    this->_asynchronosProcess = new QProcess(this);
    // ascynchorons commands are logged to logger
    connect(this->_asynchronosProcess, &QProcess::readyReadStandardOutput, this, &LinboCmd::_readFromStdout);
    connect(this->_asynchronosProcess, &QProcess::readyReadStandardError, this, &LinboCmd::_readFromStderr);
    connect(this->_asynchronosProcess, &QProcess::finished, this, &LinboCmd::commandFinished);

    // synchronos commands are not logged
    this->_synchronosProcess = new QProcess(this);
}

QString LinboCmd::readImageDescription(LinboImage* image) {
    QProcess readProcess;
    QString description = this->getOutput("readfile", this->_backend->config()->cachePath(), image->name() + ".desc");

    if(this->getExitCodeOfLastSyncCommand() == 0)
        return description;
    else
        return "";
}

bool LinboCmd::writeImageDescription(LinboImage* image, QString newDescription) {
    return this->writeImageDescription(image->name(), newDescription);
}

bool LinboCmd::writeImageDescription(QString imageName, QString newDescription) {

    QProcess process;
    process.start(
        this->_linboCmdCommand,
        this->_buildCommand("writefile", this->_backend->config()->cachePath(), imageName + ".desc"));

    if(!process.waitForStarted()) {
        this->_backend->logger()->error("Description writer didn't start: " + QString::number(process.exitCode()));
        return false;
    }

    process.write(newDescription.toUtf8());

    if(!process.waitForBytesWritten()) {
        this->_backend->logger()->error("Description writer didn't write: " + QString::number(process.exitCode()));
        return false;
    }

    process.closeWriteChannel();

    if(!process.waitForFinished()) {
        this->_backend->logger()->error("Description writer didn't finish: " + QString::number(process.exitCode()));
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
    QString stdOut = this->_asynchronosProcess->readAllStandardOutput();
    QStringList lines = stdOut.split("\n");
    for(const QString &line : lines) {
        this->_backend->logger()->stdOut(line.simplified());
    }
}

void LinboCmd::_readFromStderr() {
    QString stdOut = this->_asynchronosProcess->readAllStandardError();
    QStringList lines = stdOut.split("\n");
    for(const QString &line : lines) {
        this->_backend->logger()->stdErr(line.simplified());
    }
}

QString LinboCmd::_maskString(QString stringToMask) {
    if(!this->_stringToMaskInOutput.isEmpty())
        return stringToMask.replace(this->_stringToMaskInOutput, "***");
    else
        return stringToMask;
}

void LinboCmd::_logExecution(QStringList arguments) {
    QString argumentsString = this->_maskString(arguments.join(" "));
    this->_backend->logger()->info("Executing: " + this->_linboCmdCommand + " " + argumentsString);
}
