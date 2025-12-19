#ifndef WDGBUSSTOPFLAGEDITOR_H
#define WDGBUSSTOPFLAGEDITOR_H

#include <QWidget>
#include <QCheckBox>

#include "src/projectdata/Busstop.h"

namespace Ui {
class WdgBusstopFlagEditor;
}

class WdgBusstopFlagEditor : public QWidget {
    Q_OBJECT

public:
    explicit WdgBusstopFlagEditor(QWidget *parent = nullptr);
    ~WdgBusstopFlagEditor();

    BusstopFlags flags() const;

public slots:
    void setEditorMode(const bool &b);
    void setFlags(const BusstopFlags &flags);

protected slots:
    void onStandardCheckboxToggled(const bool &checked);
    void onAnyCheckboxToggled(QCheckBox *checkbox);

protected:
    void blockAllCheckboxSignals(const bool &block);

signals:
    void flagsChanged(const BusstopFlags &flags);

private:
    Ui::WdgBusstopFlagEditor *ui;
    QList<QCheckBox *> _checkboxes;

    bool _editorMode;
    bool _anotherChecked;
};

#endif // WDGBUSSTOPFLAGEDITOR_H
