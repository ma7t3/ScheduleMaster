#ifndef WDGTIMEPROFILEEDITORTABLE_H
#define WDGTIMEPROFILEEDITORTABLE_H

#include <QTableWidget>

#include "ProjectData/RouteBusstopItem.h"
#include "ProjectData/ProjectDataItemList.h"

class TimeProfile;
class Route;

class WdgTimeProfileEditorTable : public QTableWidget {
    Q_OBJECT
public:
    WdgTimeProfileEditorTable(QWidget *parent);

    enum InputMode {
        RawInputMode,
        ReferenceTimeInputMode
    };

    enum InputBehavior {
        RawInputBehavior,
        PreventNegativeTravelTimesInputBehavior,
        ProportionalInputBehavior
    };

    enum InputColumn {
        SeperateValuesCheckboxColumn = 0,
        ArrivalColumn = 1,
        DepartureColumn = 2
    };


public slots:
    void setTimeProfile(TimeProfile *p);
    void reload();

    void setInputMode(const InputMode &mode);
    void setInputBehavior(const InputBehavior &behavior);
    void setReferenceTime(const QTime &refTime);
    void setHighlightNegativeTime(const bool &b);
    void setKeepLayovers(const bool &newKeepLayovers);
    void setPreventNegativeTravelTimesWhileProportionalMode(const bool &newPreventNegativeTravelTimes);
    void setKeepLayoversWhileProportionalMode(const bool &newKeepLayovers);

protected slots:
    void reloadRoute();
    void onValueChanged(const int &row, const InputColumn &column, const float &oldValue, const float &newValue);
    void invalidateHighlighting();

protected:
    float minValueAtCell(const int &row, const int &column);
    void setMinValueAtCell(const int &row, const int &column, const float &value);

private:
    TimeProfile *_timeProfile;
    Route *_route;
    PDIList<RouteBusstopItem> _routeBusstops;

    bool _highlightNegativeTime;
    InputMode _inputMode;
    InputBehavior _inputBehavior;
    QTime _referenceTime;
    bool _keepLayovers, _preventNegativeTravelTimesWhileProportionalMode, _keepLayoversWhileProportionalMode;
};

#endif // WDGTIMEPROFILEEDITORTABLE_H
