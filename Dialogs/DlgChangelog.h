#ifndef DLGCHANGELOG_H
#define DLGCHANGELOG_H

#include <QDialog>

namespace Ui {
class DlgChangelog;
}

class DlgChangelog : public QDialog {
    Q_OBJECT

public:
    explicit DlgChangelog(QWidget *parent = nullptr);
    ~DlgChangelog();

private slots:
    void refreshChangelog();

    void on_pbCheckForUpdates_clicked();

signals:
    void updateCheckRequested();

private:
    Ui::DlgChangelog *ui;
};

#endif // DLGCHANGELOG_H
