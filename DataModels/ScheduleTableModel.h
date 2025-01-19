#ifndef SCHEDULETABLEMODEL_H
#define SCHEDULETABLEMODEL_H

#include <QAbstractTableModel>

#include "DataModels/OrderedProjectDataColumnModel.h"

class ScheduleTableModel : public OrderedProjectDataColumnModel<Trip> {
    Q_OBJECT

public:
    explicit ScheduleTableModel(QObject *parent);
    void setLine(Line *);
    void setDirection(LineDirection *);
    void setWeekDays(WeekDays);
    void setAll(Line *, LineDirection *, WeekDays);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int      rowCount(const QModelIndex &parent = QModelIndex())                              const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

    Busstop *busstopAt(const int &) const;

    const static inline int HeaderRowCount = 7;
    const static inline QList<QColor> profileColors = {QColor(0, 163, 255), QColor(185, 0, 255), QColor(255, 0, 184), QColor(255, 134, 0), QColor(128, 255, 0)};

public slots:
    void setShowOnlyImportant(const bool &);

protected:
    class TripCache {public:
        enum TourStatusType {
            None,
            Match,
            TooFew,
            TooMuch
        };
        QList<Tour *> tours;
        QList<Footnote *> footnotes;
        TourStatusType tourStatusType;
    };

    QList<Trip*> fetchData() const override;
    void reset() override;
    void reloadCache();

    QList<Busstop *> createBusstopList(QList<Trip *>);
    void recalculateDataFieldsCount();

private:
    ProjectData *_projectData;
    Line *_line;
    LineDirection *_direction;
    WeekDays _weekDays;
    bool _showOnlyImportant;

    QList<Busstop *> _busstopList;

    QHash<Trip *, TripCache> _tripCache;
};

#endif // SCHEDULETABLEMODEL_H
