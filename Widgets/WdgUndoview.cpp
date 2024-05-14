#include "wdgundoview.h"
#include "ui_wdgundoview.h"

#include "Mainwindow.h"

WdgUndoView::WdgUndoView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgUndoView),
    undoStack(((MainWindow *)parent)->undoStack())
{
    ui->setupUi(this);
    ui->undoView->setStack(undoStack);
}

WdgUndoView::~WdgUndoView()
{
    delete ui;
}
