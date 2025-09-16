#ifndef WDGPROJECTDATATABLEVIEW_H
#define WDGPROJECTDATATABLEVIEW_H

#include <QTableView>
#include <QSortFilterProxyModel>

#include "ProjectData/ProjectDataItemSet.h"
#include "ProjectDataModels/UnorderedProjectDataRowModel.h"

class WdgProjectDataTableViewSignals : public QTableView {
    Q_OBJECT

public:
    WdgProjectDataTableViewSignals(QWidget *parent = nullptr) : QTableView(parent) {}

signals:
    void currentItemChanged();
    void selectedItemsChanged();
};

template<typename T>
class WdgProjectDataTableView : public WdgProjectDataTableViewSignals {
public:
    WdgProjectDataTableView(QWidget *parent = nullptr) :
        WdgProjectDataTableViewSignals(parent), _proxyModel(nullptr), _model(nullptr) {}

    void setModel(QAbstractItemModel *model) override {
        if(_model)
            _model->disconnect(this);

        if(_proxyModel)
            _proxyModel->disconnect(this);

        if(selectionModel())
            selectionModel()->disconnect(this);

        QTableView::setModel(model);
        _model = dynamic_cast<UnorderedProjectDataRowModel<T> *>(model);
        if(!_model) {
            // try if it uses a proxy model
            _proxyModel = dynamic_cast<QSortFilterProxyModel *>(model);
            if(_proxyModel) {
                _model = dynamic_cast<UnorderedProjectDataRowModel<T> *>(_proxyModel->sourceModel());
                connect(_proxyModel, &QSortFilterProxyModel::sourceModelChanged, this, [this](){
                    _model = dynamic_cast<UnorderedProjectDataRowModel<T> *>(_proxyModel->sourceModel());
                });
            }
        }

        connect(_model, &UnorderedProjectDataRowModelSignals::multipleRowsInserted, this, &WdgProjectDataTableView::onRowsAdded);
        connect(this, &QTableView::currentIndex, this, [this]() { emit currentItemChanged(); });
        connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, [this]() {
            updateSelectionDependentActions();
            emit selectedItemsChanged();
        });

        connect(_model, &QAbstractItemModel::modelReset, this, &WdgProjectDataTableView::updateSelectionDependentActions);

        updateSelectionDependentActions();
    }

    T *currentItem() const {
        if(!currentIndex().isValid() || !_model)
            return nullptr;

        return _proxyModel ? _model->itemAt(_proxyModel->mapToSource(currentIndex()).row())
                           : _model->itemAt(currentIndex().row());
    }

    PDISet<T> selectedItems() const {
        const QModelIndexList list = selectionModel()->selectedRows();
        PDISet<T> items;
        for(const QModelIndex &index : list) {
            T *item = _proxyModel ? _model->itemAt(_proxyModel->mapToSource(index).row())
                                  : _model->itemAt(index.row());
            items.add(item);
        }
        return items;
    }

    int selectedItemsCount() const {
        return selectionModel()->selectedRows().count();
    }

    void addSelectionDependentAction(QAction *action, std::function<bool(const int &)> function) {
        _selectionDependentActions.insert(action, function);
        action->setEnabled(function(selectedItemsCount()));
    }

protected:
    void onRowsAdded(const QList<QPersistentModelIndex> &indexes) {
        if(!_model->shouldSelectItemsOnInsert())
            return;

        clearSelection();
        for(const QPersistentModelIndex &index : indexes)
            selectRow(_proxyModel->mapFromSource(index).row());
    }

    void updateSelectionDependentActions() {
        const int n = selectedItemsCount();
        for(auto it = _selectionDependentActions.begin(); it != _selectionDependentActions.end(); it++)
            it.key()->setEnabled(it.value()(n));
    }

protected:
    QSortFilterProxyModel *_proxyModel;
    UnorderedProjectDataRowModel<T> *_model;
    QHash<QAction *, std::function<bool(const int &)>> _selectionDependentActions;
};

#endif // WDGPROJECTDATATABLEVIEW_H
