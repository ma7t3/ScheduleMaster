#ifndef DLGLINEEDITOR_H
#define DLGLINEEDITOR_H

#include <QDialog>

#include "src/projectdata/Line.h"
#include "src/projectdata/model/LineDirectionTableModel.h"

namespace Ui {
class DlgLineEditor;
}

class DlgLineEditor : public QDialog {
    Q_OBJECT

public:
    explicit DlgLineEditor(Line *line = nullptr, QWidget *parent = nullptr);
    ~DlgLineEditor();

    Line *line();

protected slots:
    void onDirectionNew();
    void onDirectionEdit();
    void onDirectionDelete();
    void onDirectionMoveUp();
    void onDirectionMoveDown();

    void onCurrentDirectionChanged();

    void accept() override;

private:
    Ui::DlgLineEditor *ui;
    QAction *_directionNew, *_directionEdit, *_directionDelete, *_directionMoveUp, *_directionMoveDown;
    QMenu *_directionMenu;
    Line *_line;

    LineDirectionTableModel *_model;
};

#endif // DLGLINEEDITOR_H
