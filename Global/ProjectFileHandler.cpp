#include "ProjectFileHandler.h"

#include "src/projectdata/ProjectData.h"

#include <QApplication>
#include <QFile>

ProjectFileHandler::ProjectFileHandler(ProjectData *projectData, QObject *parent) :
    QThread(parent), _compress(false), _projectData(projectData) {

    connect(_projectData, &ProjectData::progressMaximum, this, &ProjectFileHandler::progressStepMaximum);
    connect(_projectData, &ProjectData::progressUpdate,  this, &ProjectFileHandler::progressStepUpdate);
}

ProjectFileHandler::~ProjectFileHandler() {
    requestInterruption();
    wait();
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

ProjectFileHandler::FinishReason ProjectFileHandler::finishReason() const {
    return _finishReason;
}

QString ProjectFileHandler::errorString() const {
    return _errorString;
}

void ProjectFileHandler::beforeStart(const QString &filePath) {
    _filePath = filePath;
    //Global::blockSignalsRecursive(_projectData, true); // Why do we use it? Just leave it out for now
    _projectData->setParent(nullptr);
    _projectData->moveToThread(this);
    _errorString.clear();
    start();
}

bool ProjectFileHandler::startStep(const QString &text) {
    if(isInterruptionRequested()) {
        finishRun(CancelReason);
        return false;
    }

    emit progressStepChanged(text);
    return true;
}

void ProjectFileHandler::run() {
    if(_workMode == UnknownMode) {
        qWarning() << "[ProjectFileHandler] Unknown work mode!";
        finishRun(ErrorReason, tr("Internal error: Unkown working mode (read/write)")); return;
    }

    QFile f(_filePath);

    if(_workMode == ReadMode) {
        if(!f.open(QFile::ReadOnly)) {
            qWarning().noquote() << "Cannot read file. Error:" << f.errorString();
            finishRun(ErrorReason, tr("Cannot read file. Error: %1").arg(f.errorString())); return;
        }

        if(!startStep(tr("Reading file...")))
            return;

        qInfo() << "Reading file...";

        QByteArray data = f.readAll();

        const int headerMaxSize = std::max(compressedHeader.size(), uncompressedHeader.size());
        QByteArray header = data.size() >= headerMaxSize ? data.left(headerMaxSize) : QByteArray();
        if(header == compressedHeader) {
            data.remove(0, compressedHeader.size());
            if(!startStep(tr("Uncompressing data...")))
                return;

            qInfo() << "Uncompressing data...";

            data = qUncompress(data);
            if(data.isEmpty()) {
                qWarning().noquote() << "Cannot decompress data - corrupt!";
                finishRun(ErrorReason, tr("Cannot decompress data - corrupt!")); return;
            }

        } else if(header == uncompressedHeader)
            data.remove(0, uncompressedHeader.size());

        QJsonParseError error;
        if(!startStep(tr("Parsing JSON...")))
            return;

        qInfo() << "Parsing JSON...";

        QJsonDocument doc = QJsonDocument::fromJson(data, &error);

        if(error.error != QJsonParseError::NoError) {
            qWarning().noquote() << "Cannot parse file. Error:" << error.errorString();
            finishRun(ErrorReason, tr("Cannot parse file. Error: %1").arg(error.errorString())); return;
        }

        // _projectData->blockSignals(false); // see above
        if(!startStep(tr("Loading project data...")))
            return;

        qInfo() << "Loading project data...";

        bool result = _projectData->setJson(doc.object(), [this](){return isInterruptionRequested();});
        // _projectData->blockSignals(true); // see above
        if(result)
            _projectData->setFilePath(_filePath);

        finishRun(result ? SuccessfulReason : CancelReason);
        return;
    }

    if(_workMode == WriteMode) {
        qInfo() << "Saving to file:" << _filePath;
        if(!f.open(QFile::WriteOnly)) {
            qWarning().noquote() << "Cannot write to file. Error:" << f.errorString();
            finishRun(ErrorReason, tr("Cannot write to file. Error: %1").arg(f.errorString())); return;
        }

        if(!startStep(tr("Creating JSON...")))
            return;

        qInfo() << "Creating JSON...";

        const QJsonObject object = _projectData->toJson();
        const QJsonDocument doc(object);

        QByteArray writeData;
        if(_compress) {
            if(!startStep(tr("Compressing data...")))
                return;

            qInfo() << "Compressing data...";

            writeData = compressedHeader + qCompress(doc.toJson(QJsonDocument::Compact), 9);
        } else
            writeData = doc.toJson(QJsonDocument::Indented);

        if(!startStep(tr("Writing data to file...")))
            return;

        qInfo() << "Writing data to file...";
        f.write(writeData);

        _projectData->setFilePath(_filePath);

        finishRun(SuccessfulReason);
    }
}

void ProjectFileHandler::finishRun(const FinishReason &reason, const QString &errorString) {
    _finishReason = reason;
    _errorString = errorString;
    _projectData->moveToThread(QApplication::instance()->thread());
    _projectData->notifyMovedBackToMainThread();
    //Global::blockSignalsRecursive(_projectData, false); // see above
    switch(reason) {
        case SuccessfulReason: qInfo()    << "   Finished successfully!"; break;
        case CancelReason:     qInfo()    << "   Canceled!"; break;
        case ErrorReason:      qWarning() << "   Failed!"; break;
    }
}
