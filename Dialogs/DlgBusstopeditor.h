#ifndef BUSSTOPEDITOR_H
#define BUSSTOPEDITOR_H

#include <QDialog>

#include <QCloseEvent>

namespace Ui {
class busstopEditor;
}

class busstopEditor : public QDialog
{
    Q_OBJECT

public:
    explicit busstopEditor(QWidget *parent = nullptr, bool m = true, QString = "", bool = false);
    ~busstopEditor();

    void setName(QString);
    void setImportant(bool);

    QString name();
    bool isImportant();

    void setCreateNewMode(bool);

private slots:
    void on_busstopEditor_accepted();

private:
    Ui::busstopEditor *ui;

    bool createNewMode;
};

#endif // BUSSTOPEDITOR_H
