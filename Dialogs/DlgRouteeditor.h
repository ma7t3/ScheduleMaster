#ifndef ROUTEEDITOR_H
#define ROUTEEDITOR_H

#include <QDialog>
#include <QTreeWidgetItem>

#include "ProjectData\projectdata.h"

namespace Ui {
class routeEditor;
}

class routeEditor : public QDialog
{
    Q_OBJECT

public:
    explicit routeEditor(QWidget *parent = nullptr, bool createMode = false, Route * r = nullptr, QList<LineDirection *> ld = {}, QList<Busstop*> l = QList<Busstop*>(), QList<Route *> m = QList<Route *>());
    ~routeEditor();

    void moveCurrentRouteBusstop(bool);

    QString name();
    int getCode();
    LineDirection *getDirection();
    QStringList getBusstopList();
    QStringList getTimeProfileList();
    QList<Route *> matchingRoutes;

private slots:
    void on_pbBusstopAdd_clicked();

    void on_pbBusstopRemove_clicked();

    void on_pbBusstopUp_clicked();

    void on_pbBusstopDown_clicked();

    void on_pbBusstopReverse_clicked();

    void on_leBusstopsSearch_textChanged(const QString &arg1);

    void on_pbProfileNew_clicked();

    void on_pbProfileEdit_clicked();

    void on_twProfiles_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_twAllBusstops_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_pbProfileDelete_clicked();

private:
    Ui::routeEditor *ui;

    void refreshAllBusstops(QString);
    void refreshProfiles();

    QList<LineDirection *> directions;
    Route * routeData;
    QList<Busstop *> allBusstops;
};

#endif // ROUTEEDITOR_H
