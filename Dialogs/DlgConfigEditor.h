#ifndef DLGCONFIGEDITOR_H
#define DLGCONFIGEDITOR_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QInputDialog>
#include <QClipboard>

#include "Global/LocalConfig.h"
#include "Global/VariantConverter.h"
#include "Global/ActionShortcutMapper.h"

#include "ItemModels/LocalConfigModel.h"

namespace Ui {
class DlgConfigEditor;
}

class DlgConfigEditor : public QDialog {
    Q_OBJECT

public:
    explicit DlgConfigEditor(QWidget *parent = nullptr);
    ~DlgConfigEditor();

    enum Columns {
        IDColumn = 0,
        DescriptionColumn = 1,
        TypeColumn = 2,
        ValueColumn = 3
    };

protected:
    virtual void reject() override;

private slots:
    void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void onPreviewUpdate(const QVariant &value);

    void onSelectionChanged();

    void onSettingRestoreDefault();
    void onSettingDelete();
    void onSettingCopyID();

private:
    Ui::DlgConfigEditor *ui;

    QAction *_restoreDefaultAction, *_deleteAction, *_copyIDAction;

    LocalConfigModel _model;
};

#endif // DLGCONFIGEDITOR_H
