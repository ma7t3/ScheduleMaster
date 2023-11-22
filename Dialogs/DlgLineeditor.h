#ifndef LINEEDITOR_H
#define LINEEDITOR_H

#include <QDialog>
#include <QListWidgetItem>

#include "App/global.h"
#include "ProjectData/line.h"

namespace Ui {
class DlgLineEditor;
}

class DlgLineEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DlgLineEditor(QWidget *parent = nullptr, Line l = Line(global::getNewID(), ""));
    ~DlgLineEditor();

    Line line();

private slots:
    void on_pbColor_clicked();
    void actionRenameDirection();

    void refreshDirections();

private:
    Ui::DlgLineEditor *ui;
    Line m_line;
};

#endif // LINEEDITOR_H
