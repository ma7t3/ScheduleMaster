#include "DlgFileHandler.h"
#include "ui_DlgFileHandler.h"

#include "App/global.h"
#include "../ProjectData/weekdays.h"

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

    QTextStream s(&f);
    s.setEncoding(QStringConverter::Utf8);

    logInfo(tr("reading file..."));
    qInfo() << "reading file" << filePath << "...";
    QString jsonStr = s.readAll();
    jsonStr = jsonStr.remove(fileHeader);
    QByteArray br = jsonStr.toUtf8();
    QJsonDocument jDoc = QJsonDocument::fromJson(br);

    if(!jDoc.isObject()) {
        noErrors = false;
        logCritical(tr("couldn't load project - file is not a valid json object"));
        qCritical() << "couldn't load file - no valid json object found!";
        ui->buttonBox->setEnabled(true);
        return false;
    }

    logSuccess(tr("read file - %1 characters").arg(jsonStr.length()));
    qInfo() << "read file - " << jsonStr.length() << " characters!";

    QJsonObject jMainObj = jDoc.object();

    logInfo(tr("reading data..."));
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
    QJsonObject jFileInfo;
    jFileInfo.insert("primaryVersion", global::primaryVersion);
    jFileInfo.insert("secondaryVersion", global::secondaryVersion);
    jFileInfo.insert("tertiaryVersion", global::tertiaryVersion);
    jFileInfo.insert("releaseType", global::releaseType);

    QJsonObject jMainObj = projectData->toJson();
    jMainObj.insert("_fileInfo", jFileInfo);

    QJsonDocument jDoc;
    jDoc.setObject(jMainObj);

    QString jsonString = jDoc.toJson(QJsonDocument::Indented);

    // write to file

    QFile f(filePath);

    if(!f.open(QIODevice::WriteOnly))
        return false;

    QTextStream s(&f);
    s.setEncoding(QStringConverter::Utf8);

    s << fileHeader;
    s << jsonString;

    f.close();

    return true;
}


void DlgFileHandler::on_buttonBox_accepted() {
    accept();
}

