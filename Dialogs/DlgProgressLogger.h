#ifndef DLGPROGRESSLOGGER_H
#define DLGPROGRESSLOGGER_H

#include <QDialog>

namespace Ui {
class DlgProgressLogger;
}

class DlgProgressLogger : public QDialog {
    Q_OBJECT

public:
    explicit DlgProgressLogger(QWidget *parent = nullptr);
    ~DlgProgressLogger();

    void closeEvent(QCloseEvent *) override;

    enum EntryType {
        SuccessType,
        InfoType,
        WarningType,
        ErrorType,
        CriticalType
    };

public slots:
    void reset();

    void addEntry(const EntryType &type, const QString &message, const bool &showAsCurrentAction = false);
    void setProgressValue(const int &);
    void setProgressMaximum(const int &);

    void setAutoExpand(const bool &);
    void setAutoExpandSensitivity(const EntryType &type);

    void finish();

private slots:
    void expand();

private:
    Ui::DlgProgressLogger *ui;
    EntryType _sensitivity;
    bool _autoExpand;
    bool _expanded;
    bool _finished;
};

#endif // DLGPROGRESSLOGGER_H
