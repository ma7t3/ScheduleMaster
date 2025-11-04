#include "WdgTimeProfileEditorTable.h"

#include "ProjectData/Route.h"

#include <QHeaderView>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>

WdgTimeProfileEditorTable::WdgTimeProfileEditorTable(QWidget *parent) :
    QTableWidget(parent), _timeProfile(nullptr), _route(nullptr), _highlightNegativeTime(false),
    _inputMode(RawInputMode), _inputBehavior(RawInputBehavior), _keepLayovers(false),
    _preventNegativeTravelTimesWhileProportionalMode(false),
    _keepLayoversWhileProportionalMode(false) {
    verticalHeader()->setDefaultSectionSize(25);
    setSelectionMode(QAbstractItemView::NoSelection);
}

void WdgTimeProfileEditorTable::setTimeProfile(TimeProfile *p) {
    _timeProfile = p;

    if(!_timeProfile) {
        _route = nullptr;
        reloadRoute();
        return;
    }

    Route *r = _timeProfile->findParent<Route>();
    if(_route != r) {
        _route = r;
        reloadRoute();
    } else {
        _route = r;
    }

    if(!_route) {
        qWarning().noquote() << metaObject()->className() << ": TimeProfile with id " << _timeProfile->id() << " has no valid Route parent!";
        return;
    }

    for(int i = 0; i < _routeBusstops.count(); i++) {
        RouteBusstopItem *rb = _routeBusstops.at(i);
        TimeProfileItem *item = _timeProfile->item(rb);

        const bool hasArrival = item->hasArrival();

        QCheckBox *cb = static_cast<QCheckBox *>(cellWidget(i, SeperateValuesCheckboxColumn));
        cb->blockSignals(true);
        cb->setChecked(hasArrival);
        cb->blockSignals(false);

        QWidget *arr = cellWidget(i, ArrivalColumn);
        arr->blockSignals(true);
        setMinValueAtCell(i, ArrivalColumn, item->arrival());
        arr->setEnabled(hasArrival);
        arr->blockSignals(false);

        QWidget *dep = cellWidget(i, DepartureColumn);
        dep->blockSignals(true);
        setMinValueAtCell(i, DepartureColumn, item->departure());
        dep->blockSignals(false);
    }

    invalidateHighlighting();
}

void WdgTimeProfileEditorTable::reload() {
    _route = nullptr;
    setTimeProfile(_timeProfile);
}

void WdgTimeProfileEditorTable::setInputMode(const InputMode &mode) {
    _inputMode = mode;
    reloadRoute();
    setReferenceTime(_referenceTime);
}

void WdgTimeProfileEditorTable::setInputBehavior(const InputBehavior &behavior) {
    _inputBehavior = behavior;
}

void WdgTimeProfileEditorTable::setReferenceTime(const QTime &refTime) {
    _referenceTime = refTime;
    setTimeProfile(_timeProfile);

    if(_inputMode == ReferenceTimeInputMode)
        for(int i = 0; i < rowCount(); i++) {
            static_cast<QTimeEdit *>(cellWidget(i, ArrivalColumn))->setMinimumTime(_referenceTime);
            static_cast<QTimeEdit *>(cellWidget(i, DepartureColumn))->setMinimumTime(_referenceTime);
        }
}

void WdgTimeProfileEditorTable::setHighlightNegativeTime(const bool &b) {
    _highlightNegativeTime = b;
    invalidateHighlighting();
}

void WdgTimeProfileEditorTable::reloadRoute() {
    if(!_route) {
        clear();
        _routeBusstops.clear();
        return;
    }

    _routeBusstops = _route->busstops();

    clear();
    setRowCount(_routeBusstops.count());
    setVerticalHeaderLabels(_routeBusstops.map<QString>([](RouteBusstopItem *rb){
        return rb->busstop()->name();
    }));

    setColumnCount(3);
    setHorizontalHeaderLabels({tr("S. A."), tr("Arrival"), tr("Departure")});
    setColumnWidth(0, 25);
    setColumnWidth(1, 75);
    setColumnWidth(2, 75);

    for(int i = 0; i < _routeBusstops.count(); i++) {
        QCheckBox *cb = new QCheckBox(this);

        QWidget *arr, *dep;
        if(_inputMode == RawInputMode) {
            arr = new QDoubleSpinBox(this);
            dep = new QDoubleSpinBox(this);

            QDoubleSpinBox *arrD = static_cast<QDoubleSpinBox *>(arr);
            QDoubleSpinBox *depD = static_cast<QDoubleSpinBox *>(dep);

            arrD->setDecimals(2);
            arrD->setMaximum(999);
            arrD->setSuffix(tr(" min"));

            depD->setDecimals(2);
            depD->setMaximum(999);
            depD->setSuffix(tr(" min"));
        } else {
            arr = new QTimeEdit(this);
            dep = new QTimeEdit(this);

            QTimeEdit *arrT = static_cast<QTimeEdit *>(arr);
            QTimeEdit *depT = static_cast<QTimeEdit *>(dep);

            arrT->setDisplayFormat("HH:mm:ss");
            depT->setDisplayFormat("HH:mm:ss");
        }

        connect(cb, &QCheckBox::checkStateChanged, this, [this, i, arr](const Qt::CheckState &checkState){
            arr->setEnabled(checkState == Qt::Checked);

            setMinValueAtCell(i, ArrivalColumn, minValueAtCell(i, DepartureColumn));

            if(checkState == Qt::Checked)
                _timeProfile->item(_routeBusstops.at(i))->setArrival(minValueAtCell(i, DepartureColumn));
            else
                _timeProfile->item(_routeBusstops.at(i))->removeArrival();

            invalidateHighlighting();
        });

        if(_inputMode == RawInputMode) {
            QDoubleSpinBox *arrD = static_cast<QDoubleSpinBox *>(arr);
            QDoubleSpinBox *depD = static_cast<QDoubleSpinBox *>(dep);

            connect(arrD, &QDoubleSpinBox::valueChanged, this, [this, i](const float &value) {
                TimeProfileItem *item = _timeProfile->item(_routeBusstops.at(i));
                const float oldValue = item->arrival();
                item->setArrival(value);
                onValueChanged(i, ArrivalColumn, oldValue, value);
            });

            connect(depD, &QDoubleSpinBox::valueChanged, this, [this, i](const float &value){
                TimeProfileItem *item = _timeProfile->item(_routeBusstops.at(i));
                const float oldValue = item->departure();
                item->setDeparture(value);
                onValueChanged(i, DepartureColumn, oldValue, value);
            });
        } else {
            QTimeEdit *arrT = static_cast<QTimeEdit *>(arr);
            QTimeEdit *depT = static_cast<QTimeEdit *>(dep);

            connect(arrT, &QTimeEdit::timeChanged, this, [this, i](){
                const float value = minValueAtCell(i, ArrivalColumn);
                TimeProfileItem *item = _timeProfile->item(_routeBusstops.at(i));
                const float oldValue = item->arrival();
                item->setArrival(value);
                onValueChanged(i, ArrivalColumn, oldValue, value);
            });

            connect(depT, &QTimeEdit::timeChanged, this, [this, i](){
                const float value = minValueAtCell(i, DepartureColumn);
                TimeProfileItem *item = _timeProfile->item(_routeBusstops.at(i));
                const float oldValue = item->departure();
                item->setDeparture(value);
                onValueChanged(i, DepartureColumn, oldValue, value);
            });
        }

        setCellWidget(i, SeperateValuesCheckboxColumn, cb);
        setCellWidget(i, ArrivalColumn, arr);
        setCellWidget(i, DepartureColumn, dep);
    }

    invalidateHighlighting();
}

void WdgTimeProfileEditorTable::onValueChanged(const int &row, const InputColumn &column,
                                               const float &oldValue, const float &newValue) {
    if(oldValue == newValue || column == SeperateValuesCheckboxColumn)
        return;

    if(column == DepartureColumn) {
        QWidget *arrWidget = cellWidget(row, ArrivalColumn);
        if(!arrWidget->isEnabled()) {
            arrWidget->blockSignals(true);
            setMinValueAtCell(row, ArrivalColumn, newValue);
            arrWidget->blockSignals(false);
        }
    }

    invalidateHighlighting();
    if(_inputBehavior == RawInputBehavior)
        return;

    bool shiftOrBound, direction;
    constexpr bool Shift = true, Bound = false;
    constexpr bool Backward = false, Forward = true;

    shiftOrBound = Bound;
    int compareRow, compareColumn;

    if(_inputBehavior == PreventNegativeTravelTimesInputBehavior) {
        if(newValue > oldValue) {
            direction = Forward;

            if(column == DepartureColumn) {
                compareRow = row + 1;
                if(compareRow >= rowCount())
                    return;

                compareColumn = cellWidget(compareRow, ArrivalColumn)->isEnabled()
                                    ? ArrivalColumn
                                    : DepartureColumn;

            } else {
                compareRow = row;
                compareColumn = DepartureColumn;
                shiftOrBound = _keepLayovers;
            }
        } else {
            direction = Backward;
            if(column == DepartureColumn) {
                const bool arrEnabled = cellWidget(row, ArrivalColumn)->isEnabled();
                compareRow = arrEnabled ? row : row - 1;
                compareColumn = arrEnabled ? ArrivalColumn : DepartureColumn;
                shiftOrBound = arrEnabled && _keepLayovers;
            } else {
                compareRow = row - 1;
                compareColumn = DepartureColumn;
            }
            if(compareRow < 0)
                return;
        }
    } else {
        direction = Forward;
        if(column == DepartureColumn) {
            compareRow = row + 1;
            if(compareRow >= rowCount())
                return;

            compareColumn = cellWidget(compareRow, ArrivalColumn)->isEnabled()
                                ? ArrivalColumn
                                : DepartureColumn;
        } else {
            compareRow = row;
            compareColumn = DepartureColumn;
        }

        shiftOrBound = Shift;
    }

    const float compareValue = minValueAtCell(compareRow, compareColumn);

    if(shiftOrBound == Shift) {
        const float diff = newValue - oldValue;
        setMinValueAtCell(compareRow,
                          compareColumn,
                          _inputBehavior == ProportionalInputBehavior && direction == Forward
                                  && _preventNegativeTravelTimesWhileProportionalMode
                              ? std::max(minValueAtCell(compareRow, compareColumn) + diff, newValue)
                              : minValueAtCell(compareRow, compareColumn) + diff);
    } else {
        if(newValue > oldValue && compareValue < newValue)
            setMinValueAtCell(compareRow, compareColumn, newValue);
        else if(newValue < oldValue && compareValue > newValue)
            setMinValueAtCell(compareRow, compareColumn, newValue);
    }
}

void WdgTimeProfileEditorTable::invalidateHighlighting() {
    static const QString styleSheet = "background-color: rgba(255, 0, 0, 0.3);";

    float previousValue = 0.0f;

    for(int i = 0; i < rowCount(); i++) {
        QWidget *arr = cellWidget(i, 1);
        QWidget *dep = cellWidget(i, 2);

        float arrVal = minValueAtCell(i, ArrivalColumn);
        float depVal = minValueAtCell(i, DepartureColumn);

        if(arr->isEnabled()) {
            arr->setStyleSheet(previousValue > arrVal && _highlightNegativeTime ? styleSheet : "");
            dep->setStyleSheet(arrVal > depVal && _highlightNegativeTime ? styleSheet : "");
        } else {
            arr->setStyleSheet("");
            dep->setStyleSheet(previousValue > depVal && _highlightNegativeTime ? styleSheet : "");
        }

        previousValue = depVal;
    }
}

float WdgTimeProfileEditorTable::minValueAtCell(const int &row, const int &column) {
    if (column < 1 || column > 2 || row < 0 || row >= rowCount())
        return -1;

    if(_inputMode == RawInputMode)
        return static_cast<QDoubleSpinBox *>(cellWidget(row, column))->value();
    else
        return (_referenceTime.msecsSinceStartOfDay()
                + static_cast<QTimeEdit *>(cellWidget(row, column))->time().msecsSinceStartOfDay())
               / 1000.0f / 60.0f;
}

void WdgTimeProfileEditorTable::setMinValueAtCell(const int &row, const int &column,
                                                  const float &value) {
    if (column < 1 || column > 2 || row < 0 || row >= rowCount())
        return;

    if(_inputMode == RawInputMode)
        static_cast<QDoubleSpinBox *>(cellWidget(row, column))->setValue(value);
    else
        static_cast<QTimeEdit *>(cellWidget(row, column))
            ->setTime(QTime::fromMSecsSinceStartOfDay(_referenceTime.msecsSinceStartOfDay()
                                                      + (value * 60 * 1000)));
}

void WdgTimeProfileEditorTable::setKeepLayovers(const bool &newKeepLayovers) {
    _keepLayovers = newKeepLayovers;
}

void WdgTimeProfileEditorTable::setPreventNegativeTravelTimesWhileProportionalMode(
    const bool &newPreventNegativeTravelTimes) {
    _preventNegativeTravelTimesWhileProportionalMode = newPreventNegativeTravelTimes;
}

void WdgTimeProfileEditorTable::setKeepLayoversWhileProportionalMode(const bool &newKeepLayovers) {
    _keepLayoversWhileProportionalMode = newKeepLayovers;
}
