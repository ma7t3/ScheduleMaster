#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include <QDialog>

namespace Ui {
class DataExporter;
}

class DataExporter : public QDialog
{
    Q_OBJECT

public:
    explicit DataExporter(QWidget *parent = nullptr);
    ~DataExporter();

    void setFileName(QString);
    void setFolderName(QString);
    void setText(QString, QString, QString);

private slots:
    void on_pbCopy_clicked();
    void on_pbSave_clicked();

    void exportPDF(QString);

    void on_cbFormat_currentIndexChanged(int index);

private:
    Ui::DataExporter *ui;

    QString folderName, fileName;
    QString plainText, csvText, htmlText;
};

#endif // DATAEXPORTER_H
