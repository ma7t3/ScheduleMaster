#ifndef DLGPDFEXPORTER_H
#define DLGPDFEXPORTER_H

#include "ProjectData/projectdata.h"

#include <QDialog>
#include <QPdfWriter>

#include <QPen>
#include <QBrush>
#include <QFont>
#include <QPainter>

namespace Ui {
class DlgPdfExporter;
}

class DlgPdfExporter : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPdfExporter(QWidget *parent = nullptr, ProjectData *projectData = nullptr);
    ~DlgPdfExporter();

public slots:
    void exportAll();
    void exportAllLineSchedules();
    void exportAllTours();

private slots:
    void exportLineSchedule(PublishedLine *l);
    void writeNewPage();

    void exportTour(Tour *o);

private:
    Ui::DlgPdfExporter *ui;
    ProjectData *projectData;

    QPdfWriter *currentDocument;
    QPainter *painter;

    PublishedLine *_currentLine;
    PublishedLineDirection *_currentDirection;
    QList<Trip *> _trips;


    QPen penDefault;
    QPen penMedium;
    QPen penThin;

    QBrush brushGray;
    QBrush brushWhite;

    QFont fontHeadline;
    QFont fontNormal;
    QFont fontNormalBold;
};

#endif // DLGPDFEXPORTER_H
