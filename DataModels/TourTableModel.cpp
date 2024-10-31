#include "TourTableModel.h"

TourTableModel::TourTableModel(QObject *parent) : UnorderedProjectDataRowModel(parent) {
    _dataFieldsCount = 6;
}

void TourTableModel::setProjectData(ProjectData *newProjectData) {
    projectData = newProjectData;
    connect(projectData, &ProjectData::wasReset,     this, &TourTableModel::reset);
    connect(projectData, &ProjectData::toursAdded,   this, &TourTableModel::addItems);
    connect(projectData, &ProjectData::toursChanged, this, &TourTableModel::changeItems);
    connect(projectData, &ProjectData::toursRemoved, this, &TourTableModel::removeItems);
    reset();
}

QVariant TourTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole || orientation == Qt::Vertical)
        return QVariant();

    switch(section) {
    case 0: return tr("Name");
    case 1: return tr("Lines");
    case 2: return tr("Days");
    case 3: return tr("Time");
    case 4: return tr("Duration");
    case 5: return tr("Preview");
    }

    return QVariant();
}

int TourTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return _dataFieldsCount;
}

// TODO: It's not possible to sort by lines. Implement sort role, like lines and busstops did!
QVariant TourTableModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid())
        return QVariant();

    Tour *o = _items[index.row()];
    switch(index.column()) {
    case 0:
        switch(role) {
        case Qt::DisplayRole:
            return o->name();

        case Qt::FontRole:
            QFont f;
            f.setBold(true);
            return f;
        } break;

    case 1:
        switch(role) {
        case Qt::DecorationRole:
            return projectData->linesPixmap(o);
        }

        break;
    case 2:
        switch(role) {
        case Qt::DisplayRole:
            return o->weekDays().toString();
        } break;
    case 3:
        switch(role) {
        case Qt::DisplayRole:
            return o->startTime().toString(LocalConfig::timeFormatString(false, false)) + " - " + o->endTime().toString(LocalConfig::timeFormatString(false, false));
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::BackgroundRole:
            return timeColor(o);
        } break;
    case 4:
        switch(role) {
        case Qt::DisplayRole:
            return o->duration().toString(LocalConfig::timeFormatString(false, false));
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ForegroundRole:
            return durationColor(o, true);
        case Qt::BackgroundRole:
            return durationColor(o, false);
        } break;
    case 5:
        switch(role) {
        case Qt::DecorationRole:
            return previewPixmap(o);
        } break;
    }

    return QVariant();
}

QColor TourTableModel::timeColor(Tour *o) const {
    int startTime = o->startTime().msecsSinceStartOfDay(), endTime = o->endTime().msecsSinceStartOfDay(), diff = 0, time = 0;

    if(endTime < startTime)
        endTime += 86400000;

    diff = endTime - startTime;
    time = startTime + diff;

    if(time > 86400000)
        time -= 86400000;

    int timeColorCode = 360 - (time / 240000);
    QColor timeColor;
    timeColor.setHsv(timeColorCode, 128, 255);
    return timeColor;
}

QColor TourTableModel::durationColor(Tour *o, bool foreground) const {
    int durationColorCode = 255 - o->duration().msecsSinceStartOfDay() / 168750;
    if(durationColorCode < 0)
        durationColorCode = 0;

    QColor durationColor;
    durationColor.setHsv(0, 0, durationColorCode);

    if(foreground) {
        if(durationColorCode < 128)
           return Qt::white;
        else
            return Qt::black;
    } else {
        return durationColor;
    }
}

QPixmap TourTableModel::previewPixmap(Tour *o) const {
    QPixmap preview(480, 20);
    preview.fill(Qt::transparent);
    QPainter p(&preview);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    int startPixel, endPixel;
    startPixel = o->startTime().msecsSinceStartOfDay() / 1000 / 60 / 3;
    endPixel = o->endTime().msecsSinceStartOfDay() / 1000 / 60 / 3;

    if(endPixel < startPixel) {
        p.drawRect(startPixel - 2,
                   2,
                   480 - startPixel + 4,
                   17);
        p.drawRect(0,
                   2,
                   endPixel + 4,
                   17);
    } else {
        p.drawRect(startPixel - 2,
                   2,
                   endPixel - startPixel + 4,
                   17);
    }

    for(int j = 0; j < o->tripCount(); j++) {
        Trip *t = o->tripAt(j);
        p.setBrush(projectData->lineOfTrip(t)->color());
        int startPixel, endPixel;
        startPixel = t->startTime().msecsSinceStartOfDay() / 1000 / 60 / 3;
        endPixel = t->endTime().msecsSinceStartOfDay() / 1000 / 60 / 3;

        if(endPixel < startPixel) {
            p.drawRect(startPixel,
                       4,
                       480 - startPixel,
                       13);
            p.drawRect(0,
                       4,
                       endPixel,
                       13);
        } else {
            p.drawRect(startPixel,
                       4,
                       endPixel - startPixel,
                       13);
        }
    }
    return preview;
}

QList<Tour *> TourTableModel::fetchData() const {
    return ProjectData::sortItems(projectData->tours());
}

