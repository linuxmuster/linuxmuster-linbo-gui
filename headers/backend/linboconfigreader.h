#ifndef LINBOCONFIGREADER_H
#define LINBOCONFIGREADER_H

#include <QObject>
#include <QRegularExpression>
#include "linboconfig.h"
#include "linboimage.h"
#include "linbodiskpartition.h"
#include "linboos.h"
#include "linbotheme.h"

class LinboBackend;

class LinboConfigReader : public QObject
{
    Q_OBJECT

    friend class LinboBackend;

protected:
    explicit LinboConfigReader(LinboBackend *backend);
    LinboConfig* readConfig();

private:
    LinboConfig* _loadStartConfiguration(QString startConfFilePath);
    void _loadEnvironmentValues(LinboConfig* config);
    LinboTheme* _loadThemeConfiguration(QString themeConfFilePath, LinboConfig* config);

    void _parseLinboConfig(QMap<QString, QString> rawLinboConfig, LinboConfig* config);
    void _parsePartitionConfig(QMap<QString, QString> rawParitionConfig, LinboConfig* config);
    void _parseOsConfig(QMap<QString, QString> rawOsConfig, LinboConfig* config);

    bool _validateColorCode(QString code);
    bool _stringToBool(const QString& value);

private:
    LinboBackend* backend;
#ifdef TEST_ENV
    const QString _configFilePath = TEST_ENV"/start.conf";
    const QString _iconBasePath = TEST_ENV"/icons";
#else
    const QString _configFilePath = "start.conf";
    const QString _iconBasePath = "/icons";
#endif

signals:

};

#endif // LINBOCONFIGREADER_H
