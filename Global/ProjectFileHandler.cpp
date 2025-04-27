#include "ProjectFileHandler.h"

ProjectFileHandler::ProjectFileHandler(ProjectData *projectData, QObject *parent) :
    QThread(parent), _compress(false), _projectData(projectData) {

    connect(this, &QThread::finished, this, &ProjectFileHandler::afterFinish);
}

void ProjectFileHandler::readFile(const QString &filePath) {
    _workMode = ReadMode;
    beforeStart(filePath);
}

void ProjectFileHandler::saveFile(const QString &filePath, const bool &compress) {
    _workMode = WriteMode;
    _compress = compress;
    beforeStart(filePath);
}

void ProjectFileHandler::beforeStart(const QString &filePath) {
    _filePath = filePath;
    Global::blockSignalsRecursive(_projectData, true);
    _projectData->setParent(nullptr);
    _projectData->moveToThread(this);
    start();
}

void ProjectFileHandler::run() {
    if(_workMode == UnknownMode) {
        qWarning() << "[ProjectFileHandler] Unknown work mode!";
        finishRun(); return;
    }

    QFile f(_filePath);

    if(_workMode == ReadMode) {
        qInfo() << "Reading file:" << _filePath;
        if(!f.open(QFile::ReadOnly)) {
            qWarning().noquote() << "Cannot read file. Error:" << f.errorString();
            finishRun(); return;
        }

        QByteArray data = f.readAll();
        const int headerMaxSize = std::max(compressedHeader.size(), uncompressedHeader.size());
        QByteArray header = data.size() >= headerMaxSize ? data.left(headerMaxSize) : QByteArray();
        if(header == compressedHeader) {
            data.remove(0, compressedHeader.size());
            data = qUncompress(data);
        } else if(header == uncompressedHeader)
            data.remove(0, uncompressedHeader.size());

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        f.close();

        if(error.error != QJsonParseError::NoError) {
            qWarning().noquote() << "Cannot parse file. Error:" << error.errorString();
            finishRun(); return;
        }

        _projectData->blockSignals(false);
        _projectData->setJson(doc.object()); // TODO: Fortschitts-Signale abfangen
        _projectData->blockSignals(true);

    } else if(_workMode == WriteMode) {
        qInfo() << "Saving to file:" << _filePath;
        if(!f.open(QFile::WriteOnly)) {
            qWarning().noquote() << "Cannot write to file. Error:" << f.errorString();
            finishRun(); return;
        }

        const QJsonObject object = _projectData->toJson();
        const QJsonDocument doc(object);

        QByteArray writeData;
        if(_compress)
            writeData = compressedHeader + qCompress(doc.toJson(QJsonDocument::Compact), 9);
        else
            writeData = doc.toJson(QJsonDocument::Indented);

        f.write(writeData);
        f.close();
    }

    finishRun();
}

void ProjectFileHandler::finishRun() {
    _projectData->moveToThread(QApplication::instance()->thread());
    Global::blockSignalsRecursive(_projectData, false);
}

void ProjectFileHandler::afterFinish() {
    _filePath.clear();
    _workMode = UnknownMode;
    _compress = false;
}
