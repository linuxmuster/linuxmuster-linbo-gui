#include "linboconfigreader.h"
#include "linbobackend.h"

LinboConfigReader::LinboConfigReader(LinboBackend *backend) : QObject(backend)
{
    this->backend = backend;
}

LinboConfig* LinboConfigReader::readConfig() {
    LinboConfig* config = this->_loadStartConfiguration(_configFilePath);
    return config;
}

LinboConfig* LinboConfigReader::_loadStartConfiguration(QString startConfFilePath) {
    LinboConfig* config = new LinboConfig(this->backend);
    // read start.conf
    this->backend->getLogger()->info("Starting to parse start.conf");

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

        for(QMap<QString, QString> partitionConfig : partitionConfigs) {
            this->_parsePartitionConfig(partitionConfig, config);
        }

        for(QMap<QString, QString> osConfig : osConfigs) {
            this->_parseOsConfig(osConfig, config);
        }
    }
    else
        this->backend->getLogger()->error("Error opening the start configuration file: " + startConfFilePath);

    this->backend->getLogger()->info("Finished parsing start.conf");

    this->_loadEnvironmentValues(config);

    config->_theme = this->_loadThemeConfiguration(this->_iconBasePath + "/" + config->themeConfFile(), config);

    return config;
}

void LinboConfigReader::_loadEnvironmentValues(LinboConfig* config) {
    this->backend->getLogger()->log("Loading environment values", LinboLogger::LinboGuiInfo);
    //  client ip
    config->_ipAddress = this->backend->_executeCommand(true, "ip").replace("\n", "");

    // subnet mask
    config->_subnetMask = this->backend->_executeCommand(true, "netmask").replace("\n", "");

    // subnet bitmask
    config->_subnetBitmask = this->backend->_executeCommand(true, "bitmask").replace("\n", "");

    // mac address
    config->_macAddress = this->backend->_executeCommand(true, "mac").replace("\n", "");

    // Version
    config->_linboVersion = this->backend->_executeCommand(true, "version").simplified().replace("\n", "").split("[").first();

    // hostname
    config->_hostname = this->backend->_executeCommand(true, "hostname").replace("\n", "");

    // CPU
    config->_cpuModel = this->backend->_executeCommand(true, "cpu").replace("\n", "");

    // Memory
    config->_ramSize = this->backend->_executeCommand(true, "memory").replace("\n", "");

    // Cache Size
    config->_cacheSize = this->backend->_executeCommand(true, "size", config->cachePath()).replace("\n", "");

    // Harddisk Size
    QRegExp *removePartition = new QRegExp("[0-9]{1,2}");
    QString hd = config->cachePath();
    // e.g. turn /dev/sda1 into /dev/sda
    hd.remove( *removePartition );
    config->_hddSize = this->backend->_executeCommand(true, "size", hd).replace("\n", "");

    // Load all existing images
    QStringList existingImageNames = this->backend->_executeCommand(true, "listimages", config->cachePath()).split("\n");
    for(QString existingImageName : existingImageNames) {
        existingImageName = existingImageName.split("/").last();

        LinboImage* existingImage = nullptr;
        if(!existingImageName.isEmpty() && !config->_images.contains(existingImageName)) {
            existingImage = new LinboImage(existingImageName, this->backend);
            config->_images.insert(existingImageName, existingImage);
        }
        else if(config->_images.contains(existingImageName)) {
            existingImage = config->_images[existingImageName];
        }

        if(existingImage != nullptr)
            existingImage->_existsOnDisk = true;
    }

    this->backend->getLogger()->log("Finished loading environment values", LinboLogger::LinboGuiInfo);
}

LinboTheme* LinboConfigReader::_loadThemeConfiguration(QString themeConfFilePath, LinboConfig* config) {
    LinboTheme* themeConfig = new LinboTheme();

    QSettings settingsReader(themeConfFilePath, QSettings::IniFormat);
    if(settingsReader.status() != QSettings::NoError) {
        this->backend->getLogger()->error("Could not read theme config: " + themeConfFilePath);
        return themeConfig;
    }

    QMapIterator<LinboTheme::LinboThemeColorRole, QString> ic(themeConfig->getColorRolesAndNames());
    while (ic.hasNext()) {
        ic.next();
        //qDebug() << qPrintable("    - `" + ic.value().toLower().replace("color", "") + "`");
        QColor colorFromConf = settingsReader.value("colors/" + ic.value().toLower().replace("color", ""), "").toString();
        if(colorFromConf.isValid()) {
            themeConfig->_colors[ic.key()] = colorFromConf;
        }
    }

    QMapIterator<LinboTheme::LinboThemeIcon, QString> ii(themeConfig->getIconsAndNames());
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
    for(QString key : rawLinboConfig.keys()) {
        QString value = rawLinboConfig[key];
        if(key == "server")  linboConfig->_serverIpAddress = value;
        else if(key == "cache")   linboConfig->_cachePath = value;
        else if(key == "roottimeout")   linboConfig->_rootTimeout = (unsigned int)value.toInt();
        else if(key == "group")   linboConfig->_hostGroup = value;
        else if(key == "autopartition")  linboConfig->_autoPartition = stringToBool(value);
        else if(key == "autoinitcache")  linboConfig->_autoInitCache = stringToBool(value);
        else if(key == "autoformat")  linboConfig->_autoFormat = stringToBool(value);
        else if(key == "backgroundcolor" && QRegExp("^([a-fA-F0-9]{6})$").exactMatch(value)) linboConfig->_backgroundColor = "#" + value;
        else if(key == "downloadtype")  linboConfig->_downloadMethod = LinboConfig::stringToDownloadMethod(value);
        else if(key == "useminimallayout") linboConfig->_useMinimalLayout = this->stringToBool(value);
        else if(key == "locale") linboConfig->_locale = value;
        else if(key == "guidisabled") linboConfig->_guiDisabled = this->stringToBool(value);
        else if(key == "clientdetailsvisiblebydefault") linboConfig->_clientDetailsVisibleByDefault = this->stringToBool(value);
        else if(key == "themeconffile") linboConfig->_themeConfFile = value;
    }
}

void LinboConfigReader::_parsePartitionConfig(QMap<QString, QString> rawParitionConfig, LinboConfig* config) {
    LinboDiskPartition* partition = new LinboDiskPartition(this);

    for(QString key : rawParitionConfig.keys()) {
        QString value = rawParitionConfig[key];
        if(key == "dev") partition->_path = value;
        else if(key == "size")  partition->_size = value.toInt();
        else if(key == "id")  partition->_id = value;
        else if(key == "fstype")  partition->_fstype = value;
        else if(key.startsWith("bootable"))  partition->_bootable = stringToBool(value);
    }

    if(partition->path() != "")
        config->_diskPartitions.append(partition);
    else
        partition->deleteLater();
}

void LinboConfigReader::_parseOsConfig(QMap<QString, QString> rawOsConfig, LinboConfig* config) {
    LinboOs* os = new LinboOs(this->backend);

    for(QString key : rawOsConfig.keys()) {
        QString value = rawOsConfig[key];
        if(key == "name")               os->_name = value;
        else if(key == "description")   os->_description = value;
        else if(key == "version")       os->_version = value;
        else if(key == "iconname")      os->_iconName = value;
        //else if(key == "image")         os->_DifferentialImage(new LinboImage(value, LinboImage::DifferentialImage, os));
        else if(key == "boot")          os->_bootPartition = value;
        else if(key == "root")          os->_rootPartition = value;
        else if(key == "kernel")        os->_kernel = value;
        else if(key == "initrd")        os->_initrd = value;
        else if(key == "append")        os->_kernelOptions = value;
        else if(key == "syncenabled")   os->_syncButtonEnabled = stringToBool(value);
        else if(key == "startenabled")  os->_startButtonEnabled = stringToBool(value);
        else if((key == "remotesyncenabled") || (key == "newenabled"))   os->_reinstallButtonEnabled = stringToBool(value);
        else if(key == "defaultaction") os->_defaultAction = os->startActionFromString(value);
        else if(key == "autostart")     os->_autostartEnabled = stringToBool(value);
        else if(key == "autostarttimeout")   os->_autostartTimeout = value.toInt();
        else if(key == "hidden")        os->_hidden = stringToBool(value);
        else if(key == "baseimage") {
            if(!config->_images.contains(value))
                config->_images.insert(value, new LinboImage(value, this->backend));
            os->setBaseImage(config->_images[value]);
        }
    }

    if(os->name() != "")
        config->_operatingSystems.append(os);
    else
        os->deleteLater();

}

bool LinboConfigReader::stringToBool(const QString& value) {
    QStringList trueWords("yes");
    trueWords.append("true");
    trueWords.append("enable");
    return trueWords.contains(value.simplified().toLower());
}
