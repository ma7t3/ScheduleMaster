#ifndef DLGCOPYTRIP_H
#define DLGCOPYTRIP_H

#include <QDialog>

#include <QTime>

namespace Ui {
class DlgCopyTrip;
}

class DlgCopyTrip : public QDialog
{
    Q_OBJECT

public:
    explicit DlgCopyTrip(QWidget *parent = nullptr, const QTime &startTime = QTime(0, 0, 0, 0));
    ~DlgCopyTrip();

    void setStartTime(const QTime &);

    int copyCount() const;
    QTime interval() const;

private slots:
    void refresh();

    void on_rbEndTime_clicked();
    void on_rbCount_clicked();
    void on_teInterval_timeChanged(const QTime &time);
    void on_teEndTime_timeChanged(const QTime &time);
    void on_sbCount_valueChanged(int arg1);

private:
    Ui::DlgCopyTrip *ui;

    QTime _startTime;
};

#endif // DLGCOPYTRIP_H
