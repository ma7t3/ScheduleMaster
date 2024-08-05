#include "DayTypeListModel.h"

DayTypeListModel::DayTypeListModel(QObject *parent) : OrderedProjectDataRowModel<DayType>(parent) {
    _dataFieldsCount = 1;
}

void DayTypeListModel::setProjectData(ProjectData *newProjectData) {
    projectData = newProjectData;

    connect(projectData->projectSettings(), &ProjectSettings::dayTypesAdded,   this, &DayTypeListModel::addItems);
    connect(projectData->projectSettings(), &ProjectSettings::dayTypesChanged, this, &DayTypeListModel::changeItems);
    connect(projectData->projectSettings(), &ProjectSettings::dayTypesRemoved, this, &DayTypeListModel::removeItems);
    connect(projectData->projectSettings(), &ProjectSettings::changed,         this, &DayTypeListModel::reset);
    connect(projectData,                    &ProjectData::wasReset,            this, &DayTypeListModel::reset);
}

QVariant DayTypeListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    Q_UNUSED(section);Q_UNUSED(orientation);Q_UNUSED(role);
    return QVariant();
}

int DayTypeListModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return _dataFieldsCount;
}

QVariant DayTypeListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    DayType *dt = _items[index.row()];

    switch(index.column()) {
    case 0:
        switch(role) {
        case Qt::DisplayRole:
            return dt->name();
        }
    }

    return QVariant();
}

QList<DayType *> DayTypeListModel::fetchData() const {
    if(projectData)
        return projectData->projectSettings()->dayTypes();
    return {};
}
