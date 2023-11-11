#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>

#include <QListWidgetItem>

namespace Ui {
class StartupDialog;
}

class StartupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartupDialog(QWidget *parent = nullptr);
    ~StartupDialog();

    int getAction();
    QString getFilePath();

    static const int quit = 0;
    static const int newFile = 1;
    static const int openFile = 2;
    static const int openRecentFile = 3;

private slots:
    void on_commandLinkButton_clicked();

    void on_commandLinkButton_2_clicked();

    void on_commandLinkButton_3_clicked();

    void on_lwRecentFiles_itemDoubleClicked(QListWidgetItem *item);

    void on_pbOpenRecentFile_clicked();

private:
    Ui::StartupDialog *ui;

    int action;
    QString filePath;
};

#endif // STARTUPDIALOG_H
