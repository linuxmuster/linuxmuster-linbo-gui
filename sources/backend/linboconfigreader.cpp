#include "linboconfigreader.h"
#include "linbobackend.h"

LinboConfigReader::LinboConfigReader(LinboBackend *backend) : QObject(backend)
{
    this->_backend = backend;
}

LinboConfig* LinboConfigReader::readConfig() {
    LinboConfig* config = this->_loadStartConfiguration(_configFilePath);
    return config;
}

LinboConfig* LinboConfigReader::_loadStartConfiguration(QString startConfFilePath) {
    LinboConfig* config = new LinboConfig(this->_backend);
    // read start.conf
    this->_backend->logger()->info("Starting to parse start.conf");

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

            thisLine = thisLine.split("#")[0];
            thisLine = thisLine.simplified();
            if(thisLine.length() == 0)
                continue;

            if(thisLine.startsWith("[")) {
                currentSection = thisLine.toLower();
                firstLineOfSection = true;
                continue;
            }

            if(!thisLine.contains("=") || thisLine.startsWith("="))
                continue;

            QString key = thisLine.section('=', 0, 0).simplified().toLower();
            QString value = thisLine.section('=', 1).simplified();

            if(key.isEmpty() || value.isEmpty())
                continue;

            if(currentSection == "[linbo]") {
                linboConfig.insert(key, value);
            }
            else if(currentSection == "[partition]") {
                if(firstLineOfSection)
                    partitionConfigs.append(QMap<QString, QString>());

                partitionConfigs.last().insert(key, value);
            }
            else if(currentSection == "[os]") {
                if(firstLineOfSection)
                    osConfigs.append(QMap<QString, QString>());

                osConfigs.last().insert(key, value);
            }

            firstLineOfSection = false;
        }

        inputFile.close();

        // write the config our internal objects
        this->_parseLinboConfig(linboConfig, config);

        for(const QMap<QString, QString> &partitionConfig : partitionConfigs) {
            this->_parsePartitionConfig(partitionConfig, config);
        }

        for(const QMap<QString, QString> &osConfig : osConfigs) {
            this->_parseOsConfig(osConfig, config);
        }
    }
    else
        this->_backend->logger()->error("Error opening the start configuration file: " + startConfFilePath);

    this->_backend->logger()->info("Finished parsing start.conf");

    this->_loadEnvironmentValues(config);

    config->_theme = this->_loadThemeConfiguration(this->_iconBasePath + "/" + config->themeConfFile(), config);

    return config;
}

void LinboConfigReader::_loadEnvironmentValues(LinboConfig* config) {
    this->_backend->logger()->_log("Loading environment values", LinboLogger::LinboGuiInfo);
    //  client ip
    config->_ipAddress = this->_backend->_linboCmd->getOutput("ip").replace("\n", "");

    // subnet mask
    config->_subnetMask = this->_backend->_linboCmd->getOutput("netmask").replace("\n", "");

    // subnet bitmask
    config->_subnetBitmask = this->_backend->_linboCmd->getOutput("bitmask").replace("\n", "");

    // mac address
    config->_macAddress = this->_backend->_linboCmd->getOutput("mac").replace("\n", "");

    // Version
    config->_linboVersion = this->_backend->_linboCmd->getOutput("version").simplified().replace("\n", "").split("[").first();

    // hostname
    config->_hostname = this->_backend->_linboCmd->getOutput("hostname").replace("\n", "");

    // CPU
    config->_cpuModel = this->_backend->_linboCmd->getOutput("cpu").replace("\n", "");

    // Memory
    config->_ramSize = this->_backend->_linboCmd->getOutput("memory").replace("\n", "");

    // Cache Size
    config->_cacheSize = this->_backend->_linboCmd->getOutput("size", config->cachePath()).replace("\n", "");

    // Harddisk Size
    QRegularExpression *removePartition = new QRegularExpression("[0-9]{1,2}");
    QString hd = config->cachePath();
    // e.g. turn /dev/sda1 into /dev/sda
    hd.remove( *removePartition );
    config->_hddSize = this->_backend->_linboCmd->getOutput("size", hd).replace("\n", "");

    // Load all existing images
    QStringList existingImageNames = this->_backend->_linboCmd->getOutput("listimages", config->cachePath()).split("\n");
    for(QString existingImageName : existingImageNames) {
        existingImageName = existingImageName.split("/").last();
        if(!existingImageName.endsWith(".cloop"))
            continue;

        LinboImage* existingImage = nullptr;
        if(!existingImageName.isEmpty() && !config->_images.contains(existingImageName)) {
            existingImage = new LinboImage(existingImageName, this->_backend);
            config->_images.insert(existingImageName, existingImage);
        }
        else if(config->_images.contains(existingImageName)) {
            existingImage = config->_images[existingImageName];
        }

        if(existingImage != nullptr)
            existingImage->_existsOnDisk = true;
    }

    this->_backend->logger()->_log("Finished loading environment values", LinboLogger::LinboGuiInfo);
}

LinboTheme* LinboConfigReader::_loadThemeConfiguration(QString themeConfFilePath, LinboConfig* config) {
    LinboTheme* themeConfig = new LinboTheme();

    QSettings settingsReader(themeConfFilePath, QSettings::IniFormat);
    if(settingsReader.status() != QSettings::NoError) {
        this->_backend->logger()->error("Could not read theme config: " + themeConfFilePath);
        return themeConfig;
    }

    QMapIterator<LinboTheme::LinboThemeColorRole, QString> ic(themeConfig->colorRolesAndNames());
    while (ic.hasNext()) {
        ic.next();
        //qDebug() << qPrintable("    - `" + ic.value().toLower().replace("color", "") + "`");
        QColor colorFromConf = settingsReader.value("colors/" + ic.value().toLower().replace("color", ""), "").toString();
        if(colorFromConf.isValid()) {
            themeConfig->_colors[ic.key()] = colorFromConf;
        }
    }

    QMapIterator<LinboTheme::LinboThemeIcon, QString> ii(themeConfig->iconsAndNames());
    while (ii.hasNext()) {
        ii.next();
        QString iconConfKey = "icons/" + ii.value().toLower().replace("icon", "");
        //qDebug() << qPrintable("    - `" + ii.value().toLower().replace("icon", "") + "`");
        QString iconFromConf = settingsReader.value(iconConfKey, "").toString();
        if(!iconFromConf.isEmpty()) {
            themeConfig->_icons[ii.key()] = this->_iconBasePath + "/" + iconFromConf;
        }
    }

    if(!config->backgroundColor().isEmpty())
        themeConfig->_colors[LinboTheme::PrimaryColor] = config->backgroundColor();

    return themeConfig;
}

void LinboConfigReader::_parseLinboConfig(QMap<QString, QString> rawLinboConfig, LinboConfig* linboConfig) {
    for(auto iterator = rawLinboConfig.begin(); iterator != rawLinboConfig.end(); iterator++) {
        QString key = iterator.key();
        QString value = iterator.value();
        if(key == "server")  linboConfig->_serverIpAddress = value;
        else if(key == "cache")   linboConfig->_cachePath = value;
        else if(key == "roottimeout")   linboConfig->_rootTimeout = (unsigned int)value.toInt();
        else if(key == "group")   linboConfig->_hostGroup = value;
        else if(key == "autopartition")  linboConfig->_autoPartition = _stringToBool(value);
        else if(key == "autoinitcache")  linboConfig->_autoInitCache = _stringToBool(value);
        else if(key == "autoformat")  linboConfig->_autoFormat = _stringToBool(value);
        else if(key == "backgroundcolor" && this->_validateColorCode(value)) linboConfig->_backgroundColor = "#" + value;
        else if(key == "downloadtype")  linboConfig->_downloadMethod = LinboConfig::stringToDownloadMethod(value);
        else if(key == "locale") linboConfig->_locale = value;
        else if(key == "guidisabled") linboConfig->_guiDisabled = this->_stringToBool(value);
        else if(key == "clientdetailsvisiblebydefault") linboConfig->_clientDetailsVisibleByDefault = this->_stringToBool(value);
        else if(key == "themeconffile") linboConfig->_themeConfFile = value;
    }
}

void LinboConfigReader::_parsePartitionConfig(QMap<QString, QString> rawParitionConfig, LinboConfig* config) {
    LinboDiskPartition* partition = new LinboDiskPartition(this);

    for(auto iterator = rawParitionConfig.begin(); iterator != rawParitionConfig.end(); iterator++) {
        QString key = iterator.key();
        QString value = iterator.value();
        if(key == "dev") partition->_path = value;
        else if(key == "size")  partition->_size = value.toInt();
        else if(key == "id")  partition->_id = value;
        else if(key == "fstype")  partition->_fstype = value;
        else if(key.startsWith("bootable"))  partition->_bootable = _stringToBool(value);
    }

    if(partition->path() != "")
        config->_diskPartitions.append(partition);
    else
        partition->deleteLater();
}

void LinboConfigReader::_parseOsConfig(QMap<QString, QString> rawOsConfig, LinboConfig* config) {
    LinboOs* os = new LinboOs(this->_backend);

    for(auto iterator = rawOsConfig.begin(); iterator != rawOsConfig.end(); iterator++) {
        QString key = iterator.key();
        QString value = iterator.value();
        if(key == "name")               os->_name = value;
        else if(key == "description")   os->_description = value;
        else if(key == "version")       os->_version = value;
        else if(key == "iconname")      os->_iconName = value;
        else if(key == "boot")          os->_bootPartition = value;
        else if(key == "root")          os->_rootPartition = value;
        else if(key == "kernel")        os->_kernel = value;
        else if(key == "initrd")        os->_initrd = value;
        else if(key == "append")        os->_kernelOptions = value;
        else if(key == "syncenabled")   os->_syncButtonEnabled = _stringToBool(value);
        else if(key == "startenabled")  os->_startButtonEnabled = _stringToBool(value);
        else if((key == "remotesyncenabled") || (key == "newenabled"))   os->_reinstallButtonEnabled = _stringToBool(value);
        else if(key == "defaultaction") os->_defaultAction = os->startActionFromString(value);
        else if(key == "autostart")     os->_autostartEnabled = _stringToBool(value);
        else if(key == "autostarttimeout")   os->_autostartTimeout = value.toInt();
        else if(key == "hidden")        os->_hidden = _stringToBool(value);
        else if(key == "baseimage") {
            if(!config->_images.contains(value))
                config->_images.insert(value, new LinboImage(value, this->_backend));
            os->_setBaseImage(config->_images[value]);
        }
    }

    if(os->name() != "")
        config->_operatingSystems.append(os);
    else
        os->deleteLater();

}

bool LinboConfigReader::_validateColorCode(QString code) {
    QRegularExpression regex("^([a-fA-F0-9]{6})$");
    return regex.match(code).hasMatch();
}

bool LinboConfigReader::_stringToBool(const QString& value) {
    return this->_trueWords.contains(value.simplified().toLower());
}
