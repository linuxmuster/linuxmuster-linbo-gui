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

#ifndef LINBOLOGGER_H
#define LINBOLOGGER_H

#include <QObject>
#include <QtDebug>
#include <QDateTime>
#include <QFile>
#include <QStringList>

class LinboLogger : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LinboLog latestLog READ getLatestLog NOTIFY latestLogChanged)
public:

    friend class LinboBackend;
    friend class LinboConfigReader;

    enum LinboLogType {
        UnknownLogType = -1,
        StdOut = 1,
        StdErr = 2,
        LinboGuiInfo = 4,
        LinboGuiError = 8,
        LinboLogChapterBeginning = 16,
        LinboLogChapterEnd = 32
    };
    Q_DECLARE_FLAGS(LinboLogTypes, LinboLogType)
    Q_FLAG(LinboLogTypes)

    typedef struct {
        QString message;
        LinboLogType type;
        QDateTime time;
    } LinboLog;

    const LinboLog& getLatestLog();

    static QString logTypeToString(LinboLogType logType);
    QList<LinboLog> getLogs();
    QList<LinboLog> getLogsOfCurrentChapter();
    static QList<LinboLog> getFilterLogs(QList<LinboLog> logs, LinboLogTypes filterType);
    static QStringList logsToStacktrace(QList<LinboLog> logs, int limit = -1);

protected slots:
    void info(QString logText);
    void error(QString logText);
    void chapterBeginning(QString logText);
    void chapterEnd(QString logText);
    void stdErr(QString logText);
    void stdOut(QString logText);

private:
    explicit LinboLogger(QString logFilePath, QObject *parent = nullptr);

    void log(QString logText, LinboLogType logType);

    bool writeToLogFile(QString text);

    QString logFilePath;
    QList<LinboLog> logHistory;

signals:
    void latestLogChanged(const LinboLogger::LinboLog& latestLog);

};

Q_DECLARE_OPERATORS_FOR_FLAGS(LinboLogger::LinboLogTypes)

#endif // LINBOLOGGER_H
