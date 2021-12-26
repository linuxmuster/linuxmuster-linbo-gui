#ifndef LINBOCMD_H
#define LINBOCMD_H

#include <QObject>
#include <QProcess>

#include "linbologger.h"
#include "linboimage.h"
#include "linboconfig.h"
#include "linbodiskpartition.h"
#include "linboos.h"

class LinboBackend;

class LinboCmd : public QObject
{
    Q_OBJECT
public:
    explicit LinboCmd(LinboLogger* logger, QObject *parent);

    bool startOs(LinboOs* os, QString cachePath);
    bool syncOs(LinboOs* os, QString serverIP, QString cachePath);
    bool reinstallOs(LinboOs* os, QString serverIP, QString cachePath);

    bool createImageOfOs(LinboOs* os, QString name, QString cachePath);

    bool partitionDrive(QList<LinboDiskPartition*> paritions, bool format);
    bool uploadImage(LinboImage *image, QString password, QString serverIP, QString cachePath);

    bool authenticate(QString password, QString serverIP);

    bool updateCache(LinboConfig::DownloadMethod downloadMethod, bool format, QList<LinboOs*> operaringSystems, QString serverIP, QString cachePath);
    bool updateLinbo(QString serverIP, QString cachePath);
    bool registerClient(QString room, QString hostname, QString ipAddress, QString hostGroup, LinboConfig::LinboDeviceRole deviceRole, QString password, QString serverIP);

    template<typename ... Strings>
    bool executeAsync(QString argument, const Strings&... arguments) {
        return this->executeAsync(this->_buildCommand(argument, arguments ...));
    }
    bool executeAsync(QStringList arguments);

    template<typename ... Strings>
    int executeSync(QString argument, const Strings&... arguments) {
        return this->executeSync(this->_buildCommand(argument, arguments ...));
    }
    int executeSync(QStringList arguments);

    template<typename ... Strings>
    QString getOutput(QString argument, const Strings&... arguments) {
        return this->getOutput(this->_buildCommand(argument, arguments ...));
    }
    QString getOutput(QStringList arguments);
    QString getOutputOfLastSyncCommand();
    int getExitCodeOfLastSyncCommand();

    QString readImageDescription(LinboImage* image, QString cachePath);
    bool writeImageDescription(LinboImage* image, QString newDescription, QString cachePath);
    bool writeImageDescription(QString imageName, QString newDescription, QString cachePath);

    void setStringToMaskInOutput(QString string);
    void killAsyncProcess();

protected:


private:
    LinboLogger* _logger;
    QProcess* _asynchronosProcess;
    QProcess* _synchronosProcess;

    QString _stringToMaskInOutput;

    QString _outputOfLastSyncExecution;
    int _exitCodeOfLastSyncExecution;

    QStringList _linboCommandCache;
#ifdef TEST_ENV
    QString const _linboCmdCommand = TEST_ENV"/linbo_cmd";
#else
    QString const _linboCmdCommand = "linbo_cmd";
#endif

    QStringList _buildCommand() {
        QStringList tmpArguments = this->_linboCommandCache;
        this->_linboCommandCache.clear();
        return tmpArguments;
    }

    template<typename ... Strings>
    QStringList _buildCommand(QString argument, const Strings&... arguments) {
        // this appends a quoted space in case item is empty and resolves
        // problems with linbo_cmd's weird "shift"-usage
        if (argument.isEmpty())
            this->_linboCommandCache.append("");
        else
            this->_linboCommandCache.append(argument);

        return _buildCommand(arguments...);
    }

    QString _maskString(QString stringToMask);
    void _logExecution(QStringList arguments);

private slots:
    void _readFromStdout();
    void _readFromStderr();

signals:
    void commandFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // LINBOCMD_H
