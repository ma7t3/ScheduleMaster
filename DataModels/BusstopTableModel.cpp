#include "BusstopTableModel.h"

#include "App/global.h"

#include <QPainter>
#include <QPixmap>
#include <QFontMetrics>

BusstopTableModel::BusstopTableModel(QObject *parent) :
    UnorderedProjectDataRowModel<Busstop>(parent) {
}

void BusstopTableModel::setProjectData(ProjectData *newProjectData) {
    projectData = newProjectData;
    connect(projectData, &ProjectData::wasReset,        this, &BusstopTableModel::reset);
    connect(projectData, &ProjectData::busstopsAdded,   this, &BusstopTableModel::addItems);
    connect(projectData, &ProjectData::busstopsChanged, this, &BusstopTableModel::changeItems);
    connect(projectData, &ProjectData::busstopsRemoved, this, &BusstopTableModel::removeItems);
    reset();
}

QVariant BusstopTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole || orientation == Qt::Vertical)
        return QVariant();

    switch(section) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Used by Lines");
    }

    return QVariant();
}

int BusstopTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant BusstopTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    Busstop *b = _items[index.row()];

    switch(index.column()) {
    case 0:
        switch(role) {
            case Qt::DisplayRole:
                return b->name();
            case 0x0100:
                return b->name();
            case Qt::FontRole:
                QFont f;
                if(b->isImportant())
                    f.setBold(true);
                return f;
        } break;
    case 1:
        switch(role) {
        case Qt::DecorationRole: return generateLinesPixmap(b);
        case 0x0100:
            return generateUsedLinesSortString(b);
        } break;
    }

    return QVariant();
}

QList<Busstop *> BusstopTableModel::fetchData() const {
    return ProjectData::sortItems(projectData->busstops());
}

QPixmap BusstopTableModel::generateLinesPixmap(Busstop *b) const {
    QList<Line *> usedLines = projectData->linesAtBusstop(b);

    QFont f;
    QFontMetrics fm(f);

    int xValue = 5;
    for(int j = 0; j < usedLines.count(); j++) {
        int width = fm.boundingRect(usedLines[j]->name()).width();
        xValue += (width + 20);
    }

    QPixmap pixmap(xValue, 15);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    xValue = 5;
    for(int j = 0; j < usedLines.count(); j++) {
        int width = fm.boundingRect(usedLines[j]->name()).width();
        QColor color = usedLines[j]->color();
        QRect rect(xValue, 0, width + 15, 15);
        xValue += (width + 20);
        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect, 7.5, 7.5);
        painter.setPen(global::getContrastColor(color));
        painter.drawText(rect, Qt::AlignCenter, usedLines[j]->name());
    }

    return pixmap;
}

QString BusstopTableModel::generateUsedLinesSortString(Busstop *b) const {
    QStringList result;
    QList<Line *> usedLines = projectData->linesAtBusstop(b);
    for(Line *l : usedLines)
        result << l->name();

    return result.join(",");
}
