#ifndef DLGBUSSTOPEDITOR_H
#define DLGBUSSTOPEDITOR_H

#include <QDialog>
#include <QSortFilterProxyModel>

#include "src/projectdata/Busstop.h"
#include "src/projectdata/model/BusstopPlatformTableModel.h"

namespace Ui {
class DlgBusstopEditor;
}

class DlgBusstopEditor : public QDialog {
    Q_OBJECT

public:
    explicit DlgBusstopEditor(Busstop *busstop = nullptr, QWidget *parent = nullptr);
    ~DlgBusstopEditor();

    Busstop *busstop() const;

protected slots:
    void onPlatformNew();
    void onPlatformDelete();
    void onPlatformSetDefault();

    void onCurrentPlatformChanged();

    void accept() override;

private:
    Ui::DlgBusstopEditor *ui;

    QAction *_actionNewPlatform, *_actionDeletePlatform, *_actionSetDefaultPlatform;
    QMenu *_platformMenu;

    Busstop *_busstop;

    BusstopPlatformTableModel *_model;
    QSortFilterProxyModel *_proxyModel;
};

#endif // DLGBUSSTOPEDITOR_H
