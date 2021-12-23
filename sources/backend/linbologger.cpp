/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020-2021  Dorian Zedler <dorian@itsblue.de>
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

#include "linbologger.h"

LinboLogger::LinboLogger(QString logFilePath, QObject *parent) : QObject(parent)
{
    this->_logFilePath = logFilePath;
}

QString LinboLogger::logTypeToString(LinboLogType logType) {
    switch (logType) {
    case LinboLogType::StdErr:
        return "StdErr";
    case LinboLogType::StdOut:
        return "StdOut";
    case LinboLogType::LinboGuiInfo:
        return "Info";
    case LinboLogType::LinboGuiError:
        return "Error";
    case LinboLogType::LinboLogChapterBeginning:
        return "+++ Chapter +++";
    case LinboLogType::LinboLogChapterEnd:
        return "+++ Chapter end +++";
    default:
        return "UNKNOW";
    }
}

void LinboLogger::_log(QString logText, LinboLogType logType) {
    if(logText.isEmpty() || logText == "")
        return;
    qDebug() << qPrintable("[" + this->logTypeToString(logType) + "] " + logText);

    LinboLog latestLog {logText, logType, QDateTime::currentDateTime()};
    this->_logHistory.append(latestLog);

    // TODO: Log error, when this fails
    this->_writeToLogFile("[" + this->logTypeToString(logType) + "] " + logText);

    emit this->latestLogChanged(latestLog);
}

void LinboLogger::info(QString logText) {
    this->_log(logText, LinboLogger::LinboGuiInfo);
}

void LinboLogger::error(QString logText) {
    this->_log(logText, LinboLogger::LinboGuiError);
}

void LinboLogger::chapterBeginning(QString logText) {
    this->_log(logText, LinboLogger::LinboLogChapterBeginning);
}

void LinboLogger::chapterEnd(QString logText) {
    this->_log(logText, LinboLogger::LinboLogChapterEnd);
}

void LinboLogger::stdErr(QString logText) {
    this->_log(logText, LinboLogger::StdErr);
}

void LinboLogger::stdOut(QString logText) {
    this->_log(logText, LinboLogger::StdOut);
}

bool LinboLogger::_writeToLogFile(QString text) {
    // write to logfile
    QFile logfile(this->_logFilePath);
    if(!logfile.open( QIODevice::WriteOnly | QIODevice::Append ))
        return false;

    QTextStream logstream( &logfile );
    logstream << text << "\n";
    logfile.flush();
    logfile.close();
    return true;
}

const LinboLogger::LinboLog& LinboLogger::getLatestLog() {
    return this->_logHistory.last();
}

QList<LinboLogger::LinboLog> LinboLogger::logs() {
    return this->_logHistory;
}

QList<LinboLogger::LinboLog> LinboLogger::logsOfCurrentChapter() {
    QList<LinboLog> tmpLogs;

    for(int i = this->_logHistory.length() - 1; i >= 0; i--) {
        tmpLogs.append(this->_logHistory[i]);
        if(this->_logHistory[i].type == LinboLogType::LinboLogChapterBeginning)
            break;
        else if(this->_logHistory[i].type == LinboLogType::LinboLogChapterEnd)
            tmpLogs.clear();
    }

    return tmpLogs;
}

QList<LinboLogger::LinboLog> LinboLogger::filteredLogs(QList<LinboLog> logs, LinboLogTypes filterTypes) {
    QList<LinboLog> tmpLogs;

    for(const LinboLog &log : logs) {
        if(filterTypes.testFlag(log.type))
            tmpLogs.append(log);
    }

    return tmpLogs;
}


QStringList LinboLogger::logsToStacktrace(QList<LinboLog> logs, int limit) {
    QStringList logStrings;
    for(int i = 0; i < logs.length(); i++) {
        if(logStrings.length() >= limit && limit > 0)
            break;

        logStrings.append("[" + LinboLogger::logTypeToString(logs[i].type) + "] " + logs[i].message);
    }

    return logStrings;
}
