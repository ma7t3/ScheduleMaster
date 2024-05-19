#include "FileHandler.h"

#include "Mainwindow.h"

#include "AppInfo.h"

FileHandler::FileHandler(QObject *parent) :
    QThread(parent),
    projectData(((MainWindow *)parent)->projectData()),
    _succes(true) {
}

void FileHandler::run() {
    if(_action == ReadFileAction)
        _succes = readFile();
    else if(_action == SaveFileAction)
        _succes = saveFile();

    projectData->moveToThread(qApp->thread());
}

FileHandler::Action FileHandler::action() const {
    return _action;
}

void FileHandler::setAction(const Action &action) {
    _action = action;
}

QString FileHandler::filePath() const {
    return _filePath;
}

void FileHandler::setFilePath(const QString &path) {
    _filePath = path;
}

bool FileHandler::success() const {
    return _succes;
}

bool FileHandler::readFile() {
    QFile f(_filePath);
    if(!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed reading file" << _filePath << "! Reason:" << f.errorString();
        emit openFileError(_filePath, f.errorString());
        return false;
    }

    emit actionStarted(DlgProgressLogger::InfoType, tr("reading file..."), true);
    qInfo() << "reading file" << _filePath << "...";

    QByteArray data = f.readAll();
    int dataSize = data.size();

    if(dataSize < 4) {
        emit actionStarted(DlgProgressLogger::CriticalType, tr("Cannot read file - no valid file header found"));
        qCritical() << "Cannot read file - no valid file header found";
        return false;
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
        return false;
    }

    emit actionStarted(DlgProgressLogger::SuccessType, tr("read file - %1 bytes").arg(dataSize));
    qInfo() << "read file -" << dataSize << "bytes!";

    QJsonObject jMainObj = jDoc.object();

    emit actionStarted(DlgProgressLogger::InfoType, tr("reading data..."), true);

    if(AppInfo::currentVersion()) {
        const QString appVersionName = jMainObj.value("_fileInfo").toObject().value("appVersion").toString();
        AppInfo::AppVersion *version = AppInfo::version(appVersionName);
        if(!version) {
            emit unknownVersionDetected();
        } else {
            if(AppInfo::fileFormatChangesSinceVersion(version)) {
                emit oldVersionDetected(version);
            }
        }
    }

    projectData->setJson(jMainObj);
    emit actionStarted(DlgProgressLogger::SuccessType, tr("project loaded!"));

    return true;
}

bool FileHandler::saveFile() {
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

    // open file
    QFile f(_filePath);
    if(!f.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed reading file" << _filePath << "! Reason:" << f.errorString();
        emit saveFileError(_filePath, f.errorString());
        return false;
    }

    // write to file
    emit actionStarted(DlgProgressLogger::InfoType, tr("writing file..."), true);
    qInfo() << "writing file...";

    f.write(data);
    f.close();
    emit actionStarted(DlgProgressLogger::SuccessType, tr("saved file"));
    qInfo() << "saved data to file.";
    return true;
}
