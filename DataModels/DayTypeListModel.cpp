#include "DayTypeListModel.h"

DayTypeListModel::DayTypeListModel(QObject *parent) :
    OrderedProjectDataRowModel<DayType>(parent),
    _projectData(nullptr),
    _projectSettings(nullptr),
    _mode(false) {
    _dataFieldsCount = 1;
}

void DayTypeListModel::setProjectData(ProjectData *newProjectData) {
    if(_projectData)
        this->disconnect(_projectData);
    if(_projectSettings)
        this->disconnect(_projectSettings);

    _projectData = newProjectData;
    _projectSettings = nullptr;

    connect(_projectData->projectSettings(), &ProjectSettings::dayTypesAdded,   this, &DayTypeListModel::addItems);
    connect(_projectData->projectSettings(), &ProjectSettings::dayTypesChanged, this, &DayTypeListModel::changeItems);
    connect(_projectData->projectSettings(), &ProjectSettings::dayTypesRemoved, this, &DayTypeListModel::removeItems);
    connect(_projectData->projectSettings(), &ProjectSettings::changed,         this, &DayTypeListModel::reset);
    connect(_projectData,                    &ProjectData::wasReset,            this, &DayTypeListModel::reset);

    _mode = true;
    reset();
}

void DayTypeListModel::setProjectSettings(ProjectSettings *newProjectSettings) {
    if(_projectData)
        this->disconnect(_projectData);
    if(_projectSettings)
        this->disconnect(_projectSettings);

    _projectSettings = newProjectSettings;
    _projectData = dynamic_cast<ProjectData *>(_projectSettings->parent());

    connect(_projectSettings,                &ProjectSettings::dayTypesAdded,   this, &DayTypeListModel::addItems);
    connect(_projectSettings,                &ProjectSettings::dayTypesChanged, this, &DayTypeListModel::changeItems);
    connect(_projectSettings,                &ProjectSettings::dayTypesRemoved, this, &DayTypeListModel::removeItems);

    _mode = false;
    reset();
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
    if(_mode && _projectData)
        return _projectData->projectSettings()->dayTypes();
    if(!_mode && _projectSettings)
        return _projectSettings->dayTypes();
    return {};
}
