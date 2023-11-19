#include "Dialogs\DlgDataexporter.h"
#include "ui_DlgDataexporter.h"

#include <QGuiApplication>
#include <QClipboard>

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QtPrintSupport/QtPrintSupport>

DataExporter::DataExporter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataExporter)
{
    ui->setupUi(this);
}

DataExporter::~DataExporter()
{
    delete ui;
}

void DataExporter::setText(QString plainText, QString csvText, QString htmlText)
{
    this->plainText = plainText;
    this->csvText = csvText;
    this->htmlText = htmlText;

    ui->cbFormat->setCurrentText(0);
    ui->pteText->setPlainText(plainText);
}

void DataExporter::setFileName(QString name)
{
    fileName = name;
}

void DataExporter::setFolderName(QString name)
{
    folderName = name;
}

void DataExporter::on_pbCopy_clicked()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(ui->pteText->toPlainText());
}

void DataExporter::on_pbSave_clicked()
{
    QString type;
    int index = ui->cbFormat->currentIndex();
    if(index == 0)
        type = "Text file (*.txt)";
    else if(index == 1)
        type = "Comma-separated values (*.csv)";
    else if(index == 2)
        type = "Hyper text markup language (*.html)";
    else if(index == 3)
        type = "Portable document format (*.pdf)";
    else
        return;

    QDir dir = QDir::homePath() + "/.ScheduleMaster/Exports/" + this->folderName;
    if(!dir.exists())
        dir.mkpath(dir.path());

    QString fileName = QFileDialog::getSaveFileName(this, "", dir.path() + "/" + this->fileName, type);
    if(fileName == "")
        return;

    if(index == 3) {
        exportPDF(fileName);
    } else {
        QFile f(fileName);
        if(!f.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this, "Couldn't open file", QString("<p><b>Couldn't open file for writing:</b></p><p>%1</p>").arg(f.errorString()));
            return;
        }

        QTextStream s(&f);
        s << ui->pteText->toPlainText();
        f.close();
    }

    QDesktopServices::openUrl(QUrl(fileName));
}

void DataExporter::exportPDF(QString path) {
    QTextDocument document;
    document.setDefaultFont(QFont("Helvetica"));
    document.setHtml(htmlText);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::Custom);
    printer.setOutputFileName(path);
    document.print(&printer);
}

void DataExporter::on_cbFormat_currentIndexChanged(int index)
{
    if(index == 0)
        ui->pteText->setPlainText(plainText);
    else if(index == 1)
        ui->pteText->setPlainText(csvText);
    else if(index == 2)
        ui->pteText->setPlainText(htmlText);
    else if(index == 3)
        ui->pteText->setPlainText("There is no preview for PDF available.");
    else {
        ui->cbFormat->setCurrentText(0);
        ui->pteText->setPlainText(plainText);
    }
}

