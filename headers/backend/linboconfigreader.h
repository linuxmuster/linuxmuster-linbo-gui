#ifndef LINBOCONFIGREADER_H
#define LINBOCONFIGREADER_H

#include <QObject>
#include <QRegularExpression>
#include <QFile>
#include <QPair>
#include <QSettings>
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

    struct Block {
        QString name;
        QMap<QString, QString> config;
    };

    struct Line {
        bool isKeyValuePair;
        bool isNewBlock;
        QString content;
    };

    struct KeyValuePair {
        QString key;
        QString value;
    };

    LinboConfig* readConfig();

private:

    bool _loadStartConf(LinboConfig* config);
    bool _loadStartConf(QFile* file, LinboConfig* config);
    void _loadStartConf(QTextStream* input, LinboConfig* config);
    QList<Block> _parseStartConf(QTextStream* input);
    Line _parseLine(QString line);
    QString _sanitizeLine(QString line);
    bool _isLineKeyValuePair(QString line);
    bool _isLineBlockName(QString line);
    QString _parseLineAsBlockName(Line line);
    KeyValuePair _parseLineAsKeyValuePair(Line line);

    void _loadEnvironmentValues(LinboConfig* config);
    void _loadExistingImages(LinboConfig* config);

    void _loadConfigFromBlock(Block block, LinboConfig* config);
    void _loadLinboConfigFromBlock(QMap<QString, QString> rawLinboConfig, LinboConfig* config);
    void _loadPartitionConfigFromBlock(QMap<QString, QString> rawParitionConfig, LinboConfig* config);
    void _loadOsConfigFromBlock(QMap<QString, QString> rawOsConfig, LinboConfig* config);

    bool _loadThemeConf(LinboConfig* config);
    void _loadThemeConf(QSettings* settings, LinboConfig* config);
    void _loadIcons(QSettings* settings, LinboTheme* theme, LinboConfig* config);
    void _loadColors(QSettings* settings, LinboTheme* theme);

    bool _validateColorCode(QString code);
    bool _stringToBool(const QString& value);

private:
    LinboBackend* _backend;
    const QVector<QString> _trueWords = {"yes", "true", "enable"};
#ifdef TEST_ENV
    const QString _configFilePath = TEST_ENV"/start.conf";
    const QString _guiFileBasePath = TEST_ENV"/gui";
#else
    const QString _configFilePath = "start.conf";
    const QString _guiFileBasePath = "/";
#endif

    const QString _iconBasePath = _guiFileBasePath + "/icons";
    const QString _themeBasePath = _guiFileBasePath + "/themes";

signals:

};

#endif // LINBOCONFIGREADER_H
