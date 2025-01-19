#ifndef TIMEPROFILEEDITOR_H
#define TIMEPROFILEEDITOR_H

#include "ProjectData\route.h"
#include "ProjectData\timeProfile.h"

#include <QDialog>

namespace Ui {
class DlgTimeProfileEditor;
}

class DlgTimeProfileEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DlgTimeProfileEditor(QWidget *parent, TimeProfile *, Route *, const bool &createMode = false);
    ~DlgTimeProfileEditor();

    void setCreateMode(const bool &);

    TimeProfile timeProfile() const;
    void loadTimeProfile();


private slots:
    void loadBusstopList(Route *);
    void refreshTable(int);
    void executeCopy();

private:
    Ui::DlgTimeProfileEditor *ui;

    TimeProfile _timeProfile;
    TimeProfile *_timeProfilePtr;

    QList<Route *> _matchingRoutes;
};

#endif // TIMEPROFILEEDITOR_H
