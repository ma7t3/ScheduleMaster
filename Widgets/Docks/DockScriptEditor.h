#ifndef DOCKSCRIPTEDITOR_H
#define DOCKSCRIPTEDITOR_H

#include <QWidget>

#include "DockAbstract.h"

namespace Ui {
class DockScriptEditor;
}

class DockScriptEditor : public DockAbstract {
    Q_OBJECT

public:
    explicit DockScriptEditor(QWidget *parent = nullptr);
    ~DockScriptEditor();

public slots:
    void executeScript();

private:
    Ui::DockScriptEditor *ui;
};

#endif // DOCKSCRIPTEDITOR_H
