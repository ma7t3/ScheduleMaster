#ifndef LINEEDITOR_H
#define LINEEDITOR_H

#include <QDialog>
#include <QListWidgetItem>

#include "ProjectData/line.h"

#include "DataModels/LineDirectionListModel.h"

namespace Ui {
class DlgLineEditor;
}

class DlgLineEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DlgLineEditor(QWidget *parent, Line *, bool createMode = false);
    ~DlgLineEditor();

    void setCreateMode(const bool &);

    Line line() const;
    void loadLine();

private slots:
    void on_pbColor_clicked();

    void actionNewDirection();
    void actionRenameDirection();
    void actionDeleteDirection();
    void actionDirectionUp();
    void actionDirectionDown();

    void onDirectionInserted(QModelIndex parent, int first, int last);

private:
    Ui::DlgLineEditor *ui;
    Line _line;
    Line *_linePtr;

    LineDirectionListModel *_directionsModel;
};

#endif // LINEEDITOR_H
