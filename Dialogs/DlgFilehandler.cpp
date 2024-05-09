#include "DlgFileHandler.h"
#include "ui_DlgFileHandler.h"

#include "App/global.h"
#include "localconfig.h"
#include "globalconfig.h"
#include "AppInfo.h"

#include <QMessageBox>

DlgFileHandler::DlgFileHandler(QWidget *parent, ProjectData *projectData) :
    QDialog(parent),
    ui(new Ui::DlgFileHandler),
    projectData(projectData)
{
    ui->setupUi(this);

    setModal(true);
}

DlgFileHandler::~DlgFileHandler()
{
    delete ui;
}

bool DlgFileHandler::readFromFile(QString filePath) {
    ui->buttonBox->setEnabled(false);
    ui->log->clear();

    qApp->processEvents();

    noErrors = true;

    QFile f(filePath);

    if(!f.exists()) {
        noErrors = false;
        logCritical(tr("couldn't load project - file not found: ") + filePath);
        qWarning() << "File" << filePath << "was not found!";
        ui->buttonBox->setEnabled(true);
        return false;
    }

    if(!f.open(QIODevice::ReadOnly)) {
        noErrors = false;
        logCritical(tr("couldn't load project - couldn't open file: ") + filePath);
        qWarning() << "Failed reading file" << filePath << "!";
        ui->buttonBox->setEnabled(true);
        return false;
    }

    logInfo(tr("reading file..."));
    qInfo() << "reading file" << filePath << "...";

    QByteArray data = f.readAll();
    int dataSize = data.size();

    if(dataSize < 4) {
        logCritical(tr("Cannot read file - no valid file header found"));
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
        noErrors = false;
        logCritical(tr("couldn't load project - file is not a valid json object"));
        qCritical() << "couldn't load file - no valid json object found!";
        ui->buttonBox->setEnabled(true);
        return false;
    }

    logSuccess(tr("read file - %1 bytes").arg(dataSize));
    qInfo() << "read file -" << dataSize << "bytes!";

    QJsonObject jMainObj = jDoc.object();

    logInfo(tr("reading data..."));

    for(int i = 0; i < AppInfo::versionCount(); i++)
        qDebug() << i << AppInfo::olderVersion(i)->name();

    if(AppInfo::currentVersion()) {
        const QString appVersionName = jMainObj.value("_fileInfo").toObject().value("appVersion").toString();
        AppInfo::AppVersion *version = AppInfo::version(appVersionName);
        if(!version) {
            QMessageBox::warning(this, tr("File format changed"), tr("<p><b>This file was created in an unkown version of ScheduleMaster!</b></p><p>We'll try to open the file anyway but it's recommended to create a backup of your original file to avoid data loss!</p>"));
        } else {
            if(AppInfo::fileFormatChangesSinceVersion(version)) {
                QMessageBox::warning(this, tr("File format changed"), tr("<p><b>This file was created in an older version of ScheduleMaster that used a different file format!</b></p><p>We'll try to convert it to the current format automatically but it's recommended to create a backup of your original file to avoid data loss!</p><p><table><tr><td><b>Current version:</b></td><td>%1</td></tr><tr><td><b>File version:</b></td><td>%2</td></tr></table></p>").arg(AppInfo::currentVersion()->name(), version->name()));
            }
        }
    }

    projectData->setJson(jMainObj);
    logSuccess(tr("project loaded!"));

    if(noErrors)
        close();

    return true;
}

void DlgFileHandler::logInfo(const QString &text) {
    QIcon icon(":/icons/info.ico");

    QTreeWidgetItem *itm = new QTreeWidgetItem({text});
    itm->setIcon(0, icon);

    ui->log->addTopLevelItem(itm);
    qApp->processEvents();
}

void DlgFileHandler::logSuccess(const QString &text) {
    QIcon icon(":/icons/success.ico");

    QTreeWidgetItem *itm = new QTreeWidgetItem({text});
    itm->setIcon(0, icon);

    ui->log->addTopLevelItem(itm);
    qApp->processEvents();
}

void DlgFileHandler::logWarning(const QString &text) {
    QIcon icon(":/icons/warning.ico");

    QTreeWidgetItem *itm = new QTreeWidgetItem({text});
    itm->setIcon(0, icon);

    noErrors = false;

    ui->log->addTopLevelItem(itm);
    qApp->processEvents();
}

void DlgFileHandler::logError(const QString &text) {
    QIcon icon(":/icons/error.ico");

    QTreeWidgetItem *itm = new QTreeWidgetItem({text});
    itm->setIcon(0, icon);

    noErrors = false;

    ui->log->addTopLevelItem(itm);
    qApp->processEvents();
}

void DlgFileHandler::logCritical(const QString &text) {
    QIcon icon(":/icons/error.ico");

    QTreeWidgetItem *itm = new QTreeWidgetItem({text});
    itm->setForeground(0, Qt::red);
    itm->setIcon(0, icon);

    noErrors = false;

    ui->log->addTopLevelItem(itm);
    qApp->processEvents();
}


bool DlgFileHandler::saveToFile(QString filePath)
{
    bool compress = filePath.endsWith(".smp");
    QJsonObject jFileInfo;
    jFileInfo.insert("appVersion", AppInfo::currentVersion() ? AppInfo::currentVersion()->name() : "UNKNOWN");

    QJsonObject jMainObj = projectData->toJson();
    jMainObj.insert("_fileInfo", jFileInfo);

    QJsonDocument jDoc;
    jDoc.setObject(jMainObj);

    QByteArray data;
    if(compress) {
        data = jDoc.toJson(QJsonDocument::Compact);
        qInfo() << "compressing file data...";
        data = fileHeaderCompressed + qCompress(data, 9);
    } else
        data = jDoc.toJson(QJsonDocument::Indented);

    qInfo() << "writing file...";

    // write to file
    QFile f(filePath);

    if(!f.open(QIODevice::WriteOnly)) {
        qCritical() << "Can not write to file:" << f.errorString();
        return false;
    }

    f.write(data);
    f.close();
    qInfo() << "saved data to file.";
    return true;
}


void DlgFileHandler::on_buttonBox_accepted() {
    accept();
}

