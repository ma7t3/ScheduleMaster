#include "SimpleBusstopListModel.h"

#include <QFont>

SimpleBusstopListModel::SimpleBusstopListModel(QObject *parent) : UnorderedProjectDataRowModel(parent) {}

void SimpleBusstopListModel::setProjectData(ProjectData *newProjectData) {
    projectData = newProjectData;
    connect(projectData, &ProjectData::wasReset,        this, &SimpleBusstopListModel::reset);
    connect(projectData, &ProjectData::busstopsAdded,   this, &SimpleBusstopListModel::addItems);
    connect(projectData, &ProjectData::busstopsChanged, this, &SimpleBusstopListModel::changeItems);
    connect(projectData, &ProjectData::busstopsRemoved, this, &SimpleBusstopListModel::removeItems);
    reset();
}

QVariant SimpleBusstopListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    Busstop *b = _items[index.row()];

    switch(role) {
    case Qt::DisplayRole:
        return b->name();

    case Qt::FontRole:
        QFont f;
        if(b->isImportant())
            f.setBold(true);
        return f;
    }

    return QVariant();
}

QList<Busstop *> SimpleBusstopListModel::fetchData() const {
    return ProjectData::sortItems(projectData->busstops());
}

bool SimpleBusstopListModel::testSearchMatch(Busstop *b) const {
    return b->name().contains(_search, Qt::CaseInsensitive);
}
