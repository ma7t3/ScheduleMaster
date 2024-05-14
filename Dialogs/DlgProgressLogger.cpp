#include "DlgProgressLogger.h"
#include "ui_DlgProgressLogger.h"

#include <QCloseEvent>

DlgProgressLogger::DlgProgressLogger(QWidget *parent) :
    QDialog(parent), ui(new Ui::DlgProgressLogger) {
    ui->setupUi(this);

    ui->twLog->setColumnWidth(0, 125);

    reset();
}

DlgProgressLogger::~DlgProgressLogger() {
    delete ui;
}

void DlgProgressLogger::closeEvent(QCloseEvent *e) {
    if(_finished) {
        reset();
        e->accept();
    } else
        e->ignore();
}

void DlgProgressLogger::reset() {
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(0);
    ui->twLog->clear();
    ui->lCurrentAction->setText("");
    _sensitivity = WarningType;
    _autoExpand = true;
    _expanded = false;
    _finished = false;

    setWindowTitle("");

    ui->twLog->setHidden(true);
    ui->lCurrentAction->setHidden(false);
    ui->progressBar->setHidden(false);
    ui->buttonBox->setHidden(true);
    ui->buttonBox->setDisabled(true);

    resize(200, 50);
}

void DlgProgressLogger::addEntry(const EntryType &type,
                                 const QString &message,
                                 const bool &showAsCurrentAction) {
    QIcon icon;
    QString typeStr;
    switch(type) {
    case SuccessType:
        icon = QIcon(":/icons/success.ico");
        typeStr = tr("Success:");
        break;

    case InfoType:
        icon = QIcon(":/icons/info.ico");
        typeStr = tr("Information:");
        break;

    case WarningType:
        icon = QIcon(":/icons/warning.ico");
        typeStr = tr("Warning:");
        break;

    case ErrorType:
        icon = QIcon(":/icons/error.ico");
        typeStr = tr("Error:");
        break;

    case CriticalType:
        icon = QIcon(":/icons/error.ico");
        typeStr = tr("Critical:");
        break;
    }

    QTreeWidgetItem *itm = new QTreeWidgetItem(ui->twLog);
    itm->setText(0, typeStr);
    itm->setIcon(0, icon);
    itm->setText(1, message);

    if(showAsCurrentAction)
        ui->lCurrentAction->setText(message);

    if(type >= _sensitivity && _autoExpand && !_expanded)
        expand();
}

void DlgProgressLogger::setProgressValue(const int &value) {
    ui->progressBar->setValue(value);
}

void DlgProgressLogger::setProgressMaximum(const int &value) {
    ui->progressBar->setMaximum(value);
}

void DlgProgressLogger::setAutoExpand(const bool &b) {
    _autoExpand = b;
}

void DlgProgressLogger::setAutoExpandSensitivity(const EntryType &type) {
    _sensitivity = type;
}

void DlgProgressLogger::finish() {
    _finished = true;

    if(_expanded)
        ui->buttonBox->setEnabled(true);
    else
        close();
}

void DlgProgressLogger::expand() {
    resize(600, 400);

    QRect rect = ((QWidget *)parent())->geometry();
    int x = rect.center().x() - width() / 2;
    int y = rect.center().y() - height() / 2;
    move(x, y);

    ui->twLog->setHidden(false);
    ui->lCurrentAction->setHidden(true);
    ui->progressBar->setHidden(false);
    ui->buttonBox->setHidden(false);

    _expanded = true;
}
