#ifndef LINESELECTIONLISTMODEL_H
#define LINESELECTIONLISTMODEL_H

#include "UnorderedProjectDataRowModel.h"

class LineSelectionListModel : public UnorderedProjectDataRowModel<Line> {
    Q_OBJECT

public:
    explicit LineSelectionListModel(QObject *parent = nullptr);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setCheckboxesEnabled(const bool &b);

    PDISet<Line> checkedLines() const;
    void clearCheckedLines();

protected:
    virtual PDISet<Line> fetch() const override;

signals:
    void checkedLinesChanged(PDISet<Line> lines);

private:
    ProjectData *_projectData;
    bool _checkboxesEnabled;
    PDISet<Line> _checkedLines;
};

#endif // LINESELECTIONLISTMODEL_H
