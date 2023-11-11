#include "wdgundoview.h"
#include "ui_wdgundoview.h"

WdgUndoView::WdgUndoView(QWidget *parent, QUndoStack *undoStack) :
    QWidget(parent),
    ui(new Ui::WdgUndoView),
    undoStack(undoStack)
{
    ui->setupUi(this);
    ui->undoView->setStack(undoStack);
}

WdgUndoView::~WdgUndoView()
{
    delete ui;
}
