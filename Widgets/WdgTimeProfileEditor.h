#ifndef WDGTIMEPROFILEEDITOR_H
#define WDGTIMEPROFILEEDITOR_H

#include <QWidget>

class TimeProfile;
class Route;

namespace Ui {
class WdgTimeProfileEditor;
}

class WdgTimeProfileEditor : public QWidget {
    Q_OBJECT

public:
    explicit WdgTimeProfileEditor(QWidget *parent = nullptr);
    ~WdgTimeProfileEditor();

public slots:
    void setTimeProfile(TimeProfile *p);
    void reload();

protected slots:
    void onInputModeChanged();
    void onInputBehaviorChanged();

signals:
    void somethingChanged();

private:
    Ui::WdgTimeProfileEditor *ui;

    TimeProfile *_timeProfile;
};

#endif // WDGTIMEPROFILEEDITOR_H
