#ifndef LINEEDITOR_H
#define LINEEDITOR_H

#include <QDialog>
#include <QListWidgetItem>

#include "App/global.h"
#include "ProjectData/line.h"

namespace Ui {
class lineEditor;
}

class lineEditor : public QDialog
{
    Q_OBJECT

public:
    explicit lineEditor(QWidget *parent = nullptr, Line l = Line(global::getNewID(), ""));
    ~lineEditor();

    Line line();

private slots:
    void on_pbColor_clicked();
    void actionRenameDirection();

    void refreshDirections();

private:
    Ui::lineEditor *ui;

    QList<LineDirection *> directionTableReference;
    Line m_line;
};

#endif // LINEEDITOR_H
