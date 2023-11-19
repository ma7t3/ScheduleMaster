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
    void exportAllLineSchedules();

private slots:
    void exportLineSchedule(PublishedLine *l);

    void writeNewPage();

private:
    Ui::DlgPdfExporter *ui;
    ProjectData *projectData;

    QPdfWriter *currentDocument;
    QPainter *painter;

    PublishedLine *m_currentLine;
    PublishedLineDirection *m_currentDirection;
    QList<Trip *> m_trips;


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
