#include "FileHandler.h"

#include "Mainwindow.h"

#include "AppInfo.h"

FileHandler::FileHandler(QObject *parent) :
    QThread(parent),
    projectData(((MainWindow *)parent)->projectData()){
    qDebug() << projectData;
}

void FileHandler::run() {
    if(_action == ReadFileAction)
        readFile();
    else if(_action == SaveFileAction)
        saveFile();
}

void FileHandler::setAction(const Action &action) {
    _action = action;
}

void FileHandler::setFilePath(const QString &path) {
    _filePath = path;
}

void FileHandler::readFile() {
    QFile f(_filePath);

    if(!f.exists()) {
        emit actionStarted(DlgProgressLogger::CriticalType, tr("couldn't load project - file not found: ") + _filePath);
        qWarning() << "File" << _filePath << "was not found!";
        return;
    }

    if(!f.open(QIODevice::ReadOnly)) {
        emit actionStarted(DlgProgressLogger::CriticalType, tr("couldn't load project - couldn't open file: ") + _filePath);
        qWarning() << "Failed reading file" << _filePath << "!";
        return;
    }

    emit actionStarted(DlgProgressLogger::InfoType, tr("reading file..."), true);
    qInfo() << "reading file" << _filePath << "...";

    QByteArray data = f.readAll();
    int dataSize = data.size();

    if(dataSize < 4) {
        emit actionStarted(DlgProgressLogger::CriticalType, tr("Cannot read file - no valid file header found"));
        qCritical() << "Cannot read file - no valid file header found";
        return;
    }

    QByteArray header = data.left(4);
    if(header == fileHeaderCompressed) {
        qInfo() << "uncompressing file data...";
        data.remove(0, 4);
        data = qUncompress(data);
    } else if(header == fileHeaderUncompressed)
        data.remove(0, 4);

    qInfo() << "loading data...";
    QJsonDocument jDoc = QJsonDocument::fromJson(data);

    if(!jDoc.isObject()) {
        emit actionStarted(DlgProgressLogger::CriticalType, tr("couldn't load project - file is not a valid json object"));
        qCritical() << "couldn't load file - no valid json object found!";
        return;
    }

    emit actionStarted(DlgProgressLogger::SuccessType, tr("read file - %1 bytes").arg(dataSize));
    qInfo() << "read file -" << dataSize << "bytes!";

    QJsonObject jMainObj = jDoc.object();

    emit actionStarted(DlgProgressLogger::InfoType, tr("reading data..."), true);

    if(AppInfo::currentVersion()) {
        const QString appVersionName = jMainObj.value("_fileInfo").toObject().value("appVersion").toString();
        AppInfo::AppVersion *version = AppInfo::version(appVersionName);
        if(!version) {
            //QMessageBox::warning(this, tr("File format changed"), tr("<p><b>This file was created in an unkown version of ScheduleMaster!</b></p><p>We'll try to open the file anyway but it's recommended to create a backup of your original file to avoid data loss!</p>"));
            emit unknownVersionDetected();
        } else {
            if(AppInfo::fileFormatChangesSinceVersion(version)) {
                //QMessageBox::warning(this, tr("File format changed"), tr("<p><b>This file was created in an older version of ScheduleMaster that used a different file format!</b></p><p>We'll try to convert it to the current format automatically but it's recommended to create a backup of your original file to avoid data loss!</p><p><table><tr><td><b>Current version:</b></td><td>%1</td></tr><tr><td><b>File version:</b></td><td>%2</td></tr></table></p>").arg(AppInfo::currentVersion()->name(), version->name()));
                emit oldVersionDetected(version);
            }
        }
    }

    qDebug() << QThread::currentThread();

    projectData->setJson(jMainObj);
    emit actionStarted(DlgProgressLogger::SuccessType, tr("project loaded!"));

    return;
}

void FileHandler::saveFile() {
    bool compress = _filePath.endsWith(".smp");
    QJsonObject jFileInfo;
    jFileInfo.insert("appVersion", AppInfo::currentVersion() ? AppInfo::currentVersion()->name() : "UNKNOWN");

    QJsonObject jMainObj = projectData->toJson();
    jMainObj.insert("_fileInfo", jFileInfo);

    QJsonDocument jDoc;
    jDoc.setObject(jMainObj);

    QByteArray data;
    if(compress) {
        data = jDoc.toJson(QJsonDocument::Compact);
        emit actionStarted(DlgProgressLogger::InfoType, tr("Compressing data..."), true);
        qInfo() << "compressing file data...";
        data = fileHeaderCompressed + qCompress(data, 9);
    } else
        data = jDoc.toJson(QJsonDocument::Indented);


    // write to file
    QFile f(_filePath);

    if(!f.open(QIODevice::WriteOnly)) {
        emit actionStarted(DlgProgressLogger::InfoType, tr("couldn't write to file: %1").arg(f.errorString()));
        qCritical() << "Can not write to file:" << f.errorString();
        return;
    }

    emit actionStarted(DlgProgressLogger::InfoType, tr("writing file..."), true);
    qInfo() << "writing file...";

    f.write(data);
    f.close();
    emit actionStarted(DlgProgressLogger::SuccessType, tr("saved file"));
    qInfo() << "saved data to file.";
}
