#ifndef LINBOCONFIGREADER_H
#define LINBOCONFIGREADER_H

#include <QObject>
#include "linboconfig.h"
#include "linboimage.h"
#include "linbodiskpartition.h"
#include "linboos.h"

class LinboBackend;

class LinboConfigReader : public QObject
{
    Q_OBJECT

    friend class LinboBackend;

protected:
    explicit LinboConfigReader(LinboBackend *backend);
    LinboConfig* readConfig();

private:
    void loadStartConfiguration(QString startConfFilePath, LinboConfig* config);
    void loadEnvironmentValues(LinboConfig* config);

    void parseLinboConfig(QMap<QString, QString> rawLinboConfig, LinboConfig* config);
    void parsePartitionConfig(QMap<QString, QString> rawParitionConfig, LinboConfig* config);
    void parseOsConfig(QMap<QString, QString> rawOsConfig, LinboConfig* config);

    bool stringToBool(const QString& value);

private:
    LinboBackend* backend;
#ifdef TEST_ENV
    const QString configFilePath = TEST_ENV"/start.conf";
#else
    const QString configFilePath = "start.conf";
#endif

signals:

};

#endif // LINBOCONFIGREADER_H
