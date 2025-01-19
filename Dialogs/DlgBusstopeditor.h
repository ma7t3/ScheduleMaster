#ifndef BUSSTOPEDITOR_H
#define BUSSTOPEDITOR_H

#include <QDialog>
#include <QCloseEvent>

#include "ProjectData/busstop.h"

namespace Ui {
class busstopEditor;
}

class busstopEditor : public QDialog
{
    Q_OBJECT

public:
    explicit busstopEditor(QWidget *parent, Busstop, bool createMode = false);
    ~busstopEditor();

    void setCreateNewMode(const bool &);

    Busstop busstop() const;
    void loadBusstop();

private:
    Ui::busstopEditor *ui;
    Busstop _busstop;
};

#endif // BUSSTOPEDITOR_H
