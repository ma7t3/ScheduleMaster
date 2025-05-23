#ifndef DLGCONFIGEDITOR_H
#define DLGCONFIGEDITOR_H

#include <QDialog>

class LocalConfigModel;

namespace Ui {
class DlgConfigEditor;
}

class DlgConfigEditor : public QDialog {
    Q_OBJECT

public:
    explicit DlgConfigEditor(QWidget *parent = nullptr);
    ~DlgConfigEditor();

    enum Columns {
        IDColumn,
        DescriptionColumn,
        TypeColumn,
        ValueColumn
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

    LocalConfigModel *_model;
};

#endif // DLGCONFIGEDITOR_H
