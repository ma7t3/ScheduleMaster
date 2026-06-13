#include "DockUndoView.h"

#include <QVBoxLayout>
#include <QUndoView>
#include <QUndoStack>

#include "../ApplicationInterface.h"

DockUndoView::DockUndoView(QWidget *parent) :
    DockAbstract{parent}, _view{new QUndoView(this)} {
    QLayout *undoLayout = new QVBoxLayout(this);
    undoLayout->addWidget(_view);
    setLayout(undoLayout);
    _view->setStack(ApplicationInterface::projectData()->undoStack());
}
