#include "linboconfigreader.h"
#include "linbobackend.h"

LinboConfigReader::LinboConfigReader(LinboBackend *backend) : QObject(backend)
{
    this->backend = backend;
}

LinboConfig* LinboConfigReader::readConfig() {
    LinboConfig* config = new LinboConfig(this->backend);
    this->loadStartConfiguration(configFilePath, config);
    this->loadEnvironmentValues(config);
    return config;
}

void LinboConfigReader::loadStartConfiguration(QString startConfFilePath, LinboConfig* config) {
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

            QStringList keyValueList = thisLine.split("=");

            if(keyValueList.length() < 2)
                continue;

            QString key = keyValueList[0].simplified().toLower();
            QString value = keyValueList[1].simplified();

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
        this->parseLinboConfig(linboConfig, config);

        for(QMap<QString, QString> partitionConfig : partitionConfigs) {
            this->parsePartitionConfig(partitionConfig, config);
        }

        for(QMap<QString, QString> osConfig : osConfigs) {
            this->parseOsConfig(osConfig, config);
        }
    }
    else
        this->backend->getLogger()->error("Error opening the start configuration file: " + startConfFilePath);

    this->backend->getLogger()->info("Finished parsing start.conf");
}

void LinboConfigReader::loadEnvironmentValues(LinboConfig* config) {
    this->backend->getLogger()->log("Loading environment values", LinboLogger::LinboGuiInfo);
    //  client ip
    config->ipAddress = this->backend->executeCommand(true, "ip").replace("\n", "");

    // subnet mask
    config->subnetMask = this->backend->executeCommand(true, "netmask").replace("\n", "");

    // subnet bitmask
    config->subnetBitmask = this->backend->executeCommand(true, "bitmask").replace("\n", "");

    // mac address
    config->macAddress = this->backend->executeCommand(true, "mac").replace("\n", "");

    // Version
    config->linboVersion = this->backend->executeCommand(true, "version").simplified().replace("\n", "").split("[").first();

    // hostname
    config->hostname = this->backend->executeCommand(true, "hostname").replace("\n", "");

    // CPU
    config->cpuModel = this->backend->executeCommand(true, "cpu").replace("\n", "");

    // Memory
    config->ramSize = this->backend->executeCommand(true, "memory").replace("\n", "");

    // Cache Size
    config->cacheSize = this->backend->executeCommand(true, "size", config->getCachePath()).replace("\n", "");

    // Harddisk Size
    QRegExp *removePartition = new QRegExp("[0-9]{1,2}");
    QString hd = config->getCachePath();
    // e.g. turn /dev/sda1 into /dev/sda
    hd.remove( *removePartition );
    config->hddSize = this->backend->executeCommand(true, "size", hd).replace("\n", "");

    // Load all existing images
    QStringList existingImageNames = this->backend->executeCommand(true, "listimages", config->getCachePath()).split("\n");
    for(QString existingImageName : existingImageNames) {
        existingImageName = existingImageName.split("/").last();
        if(!existingImageName.endsWith(".cloop"))
            continue;

        LinboImage* existingImage = nullptr;
        if(!existingImageName.isEmpty() && !config->_images.contains(existingImageName)) {
            existingImage = new LinboImage(existingImageName, this->backend);
            config->_images.insert(existingImageName, existingImage);
        }
        else if(config->_images.contains(existingImageName)) {
            existingImage = config->_images[existingImageName];
        }

        if(existingImage != nullptr)
            existingImage->setExistsOnDisk(true);
    }

    this->backend->getLogger()->log("Finished loading environment values", LinboLogger::LinboGuiInfo);
}

void LinboConfigReader::parseLinboConfig(QMap<QString, QString> rawLinboConfig, LinboConfig* linboConfig) {
    for(QString key : rawLinboConfig.keys()) {
        QString value = rawLinboConfig[key];
        if(key == "server")  linboConfig->serverIpAddress = value;
        else if(key == "cache")   linboConfig->cachePath = value;
        else if(key == "roottimeout")   linboConfig->rootTimeout = (unsigned int)value.toInt();
        else if(key == "group")   linboConfig->hostGroup = value;
        else if(key == "autopartition")  linboConfig->autoPartition = stringToBool(value);
        else if(key == "autoinitcache")  linboConfig->autoInitCache = stringToBool(value);
        else if(key == "autoformat")  linboConfig->autoFormat = stringToBool(value);
        else if(key == "backgroundcolor" && QRegExp("^([a-fA-F0-9]{6})$").exactMatch(value)) linboConfig->backgroundColor = "#" + value;
        else if(key == "downloadtype")  linboConfig->downloadMethod = LinboConfig::stringToDownloadMethod(value);
        else if(key == "useminimallayout") linboConfig->useMinimalLayout = this->stringToBool(value);
        else if(key == "locale") linboConfig->locale = value;
        else if(key == "guidisabled") linboConfig->guiDisabled = this->stringToBool(value);
        else if(key == "clientdetailsvisiblebydefault") linboConfig->clientDetailsVisibleByDefault = this->stringToBool(value);
    }
}

void LinboConfigReader::parsePartitionConfig(QMap<QString, QString> rawParitionConfig, LinboConfig* config) {
    LinboDiskPartition* partition = new LinboDiskPartition(this);

    for(QString key : rawParitionConfig.keys()) {
        QString value = rawParitionConfig[key];
        if(key == "dev") partition->setPath(value);
        else if(key == "size")  partition->setSize(value.toInt());
        else if(key == "id")  partition->setId(value);
        else if(key == "fstype")  partition->setFstype(value);
        else if(key.startsWith("bootable"))  partition->setBootable(stringToBool(value));
    }

    if(partition->getPath() != "")
        config->_diskPartitions.append(partition);
    else
        partition->deleteLater();
}

void LinboConfigReader::parseOsConfig(QMap<QString, QString> rawOsConfig, LinboConfig* config) {
    LinboOs* os = new LinboOs(this->backend);

    for(QString key : rawOsConfig.keys()) {
        QString value = rawOsConfig[key];
        if(key == "name")               os->setName(value);
        else if(key == "description")   os->setDescription(value);
        else if(key == "version")       os->setVersion(value);
        else if(key == "iconname")      os->setIconName(value);
        //else if(key == "image")         os->setDifferentialImage(new LinboImage(value, LinboImage::DifferentialImage, os));
        else if(key == "boot")          os->setBootPartition(value);
        else if(key == "root")          os->setRootPartition(value);
        else if(key == "kernel")        os->setKernel(value);
        else if(key == "initrd")        os->setInitrd(value);
        else if(key == "append")        os->setKernelOptions(value);
        else if(key == "syncenabled")   os->setSyncButtonEnabled(stringToBool(value));
        else if(key == "startenabled")  os->setStartButtonEnabled(stringToBool(value));
        else if((key == "remotesyncenabled") || (key == "newenabled"))   os->setReinstallButtonEnabled(stringToBool(value));
        else if(key == "defaultaction") os->setDefaultAction(os->startActionFromString(value));
        else if(key == "autostart")     os->setAutostartEnabled(stringToBool(value));
        else if(key == "autostarttimeout")   os->setAutostartTimeout(value.toInt());
        else if(key == "hidden")        os->setHidden(stringToBool(value));
        else if(key == "baseimage") {
            if(!config->_images.contains(value))
                config->_images.insert(value, new LinboImage(value, this->backend));
            os->setBaseImage(config->_images[value]);
        }
    }

    if(os->getName() != "")
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
