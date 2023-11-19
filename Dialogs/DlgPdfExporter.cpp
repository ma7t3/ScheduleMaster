#include "DlgPdfExporter.h"
#include "ui_DlgPdfExporter.h"

#include <QPdfWriter>
#include <QPainter>
#include <QDesktopServices>

DlgPdfExporter::DlgPdfExporter(QWidget *parent, ProjectData *projectData) :
    QDialog(parent),
    ui(new Ui::DlgPdfExporter),
    projectData(projectData),
    currentDocument(nullptr),
    m_currentLine(nullptr),
    m_currentDirection(nullptr)
{
    ui->setupUi(this);

    penDefault = QPen(Qt::black, 5);
    penDefault.setJoinStyle(Qt::MiterJoin);

    penMedium = QPen(Qt::black, 3);
    penMedium.setJoinStyle(Qt::MiterJoin);

    penThin = QPen(Qt::black, 2);
    penThin.setJoinStyle(Qt::MiterJoin);

    brushGray = QBrush(QColor(225, 225, 225));
    brushWhite = QBrush(Qt::white);

    fontHeadline = QFont("Helvetica", 16);
    fontHeadline.setBold(true);

    fontNormal = QFont("Helvetica", 10);
    fontNormalBold = QFont("Helvetica", 10);
    fontNormalBold.setBold(true);
}

DlgPdfExporter::~DlgPdfExporter()
{
    delete ui;
}

void DlgPdfExporter::exportAllLineSchedules() {
    ui->twLog->clear();

    ui->progressBar->setMaximum(projectData->publications()->lineCount() - 1);

    for(int i = 0; i < projectData->publications()->lineCount(); i++) {
        PublishedLine *l = projectData->publications()->lineAt(i);
        exportLineSchedule(l);
        ui->progressBar->setValue(i + 1);
    }

    this->close();
}

void DlgPdfExporter::exportLineSchedule(PublishedLine *l) {
    m_currentLine = l;

    QString fileName = m_currentLine->filePath();
    QFile f(fileName);
    if(!f.open(QFile::WriteOnly))
        return;

    QFileInfo fi(f);
    QString pdfTitle = fi.baseName();

    // init pdf document
    currentDocument = new QPdfWriter(fileName);
    currentDocument->setResolution(254);
    currentDocument->setCreator("ScheduleMaster");
    currentDocument->setTitle(tr("Linienfahrplan - %1").arg(pdfTitle));
    currentDocument->setPageLayout(QPageLayout(QPageSize::A4, QPageLayout::Landscape, QMargins(10, 10, 10, 10), QPageLayout::Millimeter));

    painter = new QPainter(currentDocument);

    for(int i = 0; i < m_currentLine->directionCount(); i++) {
        m_currentDirection = m_currentLine->directionAt(i);

        // collect trips
        QList<Trip *> parentTrips;
        QList<Trip *> allTrips;
        for(int j = 0; j < m_currentDirection->routeCount(); j++) {
            Route *r = m_currentDirection->routeAt(j);
            parentTrips << projectData->lineOfRoute(r)->tripsOfRoute(r);
        }
        for(int j = 0; j < parentTrips.count(); j++) {
            Trip *t = parentTrips[j];

            if(!t->weekDays()->isIn(WeekDays(WeekDays::MonFri)))
                continue;

            if(!t->hasRepeat())
                allTrips << t;
            else {
                for(int k = 0; k < t->childCount(); k++) {
                    allTrips << t->repetitionAt(k);
                }
            }
        }
        allTrips = ProjectData::sortTrips(allTrips);
        m_trips = allTrips;
        writeNewPage();
    }

    painter->end();

    QIcon icon(":/main/icons/success.ico");
    QTreeWidgetItem *itm = new QTreeWidgetItem({tr("Line finished: %1").arg(m_currentLine->title())});
    itm->setIcon(0, icon);
    ui->twLog->addTopLevelItem(itm);
    qApp->processEvents();

    //QDesktopServices::openUrl(QUrl(fileName));
}

void DlgPdfExporter::writeNewPage() {

    if(m_trips.isEmpty())
        return;

    //qDebug() << m_currentLine->title() + " " + m_currentDirection->name() + " " << m_trips.count();
    int pageWidth = 2770;
    int pageHeight = 1900;

    painter->setPen(penDefault);
    painter->setBrush(brushWhite);
    painter->setFont(fontHeadline);

    // headline
    QRect rect(0, 0, 2770, 75);
    painter->drawRect(rect);
    painter->drawText(rect.adjusted(10, 10, -10, -10), Qt::AlignLeft|Qt::AlignVCenter, m_currentLine->title() + " " + m_currentDirection->name());
    painter->setFont(fontNormal);
    painter->drawText(rect.adjusted(10, 10, -10, -10), Qt::AlignRight|Qt::AlignBottom, m_currentLine->subTitle());


    // table
    painter->setBackground(brushGray);
    int tableStart = 150;
    int maxBusstopCount = 45;
    int columnWidth = 111;
    int firstColumnWidth = columnWidth * 4;
    int maxColumnCount = 21; // 25 - firstColumnWidthFactor sozusagen
    int rowHeight = 40;

    int busstopCount = m_currentDirection->busstopCount();
    if(busstopCount > maxBusstopCount)
        busstopCount = maxBusstopCount;

    for(int i = 0; i < busstopCount; i++) {
        QRect row(0, tableStart + (i * rowHeight), pageWidth, rowHeight);
        QRect busstopNameField(5, tableStart + (i * rowHeight), firstColumnWidth - 10, rowHeight);
        if(i % 2 == 0) {
            painter->setBrush(brushGray);
        } else {
            painter->setBrush(brushWhite);
        }

        painter->setPen(Qt::NoPen);
        painter->drawRect(row);

        painter->setPen(penDefault);


        PublishedBusstop *b =  m_currentDirection->busstopAt(i);
        QString text = b->label().isEmpty() ? b->linkedBusstop()->name() : b->label();
        if(b->linkedBusstop()->isImportant())
            painter->setFont(fontNormalBold);
        else
            painter->setFont(fontNormal);

        painter->drawText(busstopNameField, Qt::AlignLeft|Qt::AlignVCenter, text);

        qApp->processEvents();
    }


    // vertical line between trips and busstop column
    painter->setFont(fontNormal);
    painter->setPen(penDefault);
    painter->drawLine(firstColumnWidth, tableStart, firstColumnWidth, tableStart + busstopCount * rowHeight);


    // trips

    QList<Trip *> trips = m_trips.sliced(0, m_trips.count() >= maxColumnCount ? maxColumnCount : m_trips.count());
    m_trips.remove(0, m_trips.count() >= maxColumnCount ? maxColumnCount : m_trips.count());

    painter->setPen(penMedium);

    for(int i = 0; i < trips.count(); i++) { // for each trip
        int lastBusstopIndex = -1;
        for(int j = 0; j < busstopCount; j++) { // for each busstop
            QRect field(i * columnWidth + 5 + firstColumnWidth, tableStart + (j * rowHeight), columnWidth, rowHeight);
            if(m_currentDirection->busstopAt(j)->linkedBusstop()->isImportant())
                painter->setFont(fontNormalBold);
            else
                painter->setFont(fontNormal);

            if(!trips[i]->route()->hasBusstop(m_currentDirection->busstopAt(j)->linkedBusstop()))
                continue;

            // draw line
            if(j - lastBusstopIndex > 1 && lastBusstopIndex != -1) {
                painter->drawLine(i * columnWidth + firstColumnWidth + (columnWidth / 2) + 5, tableStart + ((lastBusstopIndex + 1) * rowHeight) + 5, i * columnWidth + firstColumnWidth + (columnWidth / 2) + 5, tableStart + (j * rowHeight) - 5);
            }

            lastBusstopIndex = j;

            QString time = trips[i]->busstopTime(m_currentDirection->busstopAt(j)->linkedBusstop()).toString("hh:mm");

            painter->drawText(field, Qt::AlignCenter|Qt::AlignVCenter, time);

            painter->setPen(penThin);
            // draw horizontal divider
            if(j % 5 == 0 && i > 0)
                painter->drawLine(0, tableStart + j * rowHeight, pageWidth, tableStart + j * rowHeight);

            painter->setPen(penMedium);
        }
        // draw vertical divider
        if(i > 0)
            painter->drawLine(i * columnWidth + firstColumnWidth + 5, tableStart, i * columnWidth + firstColumnWidth + 5, tableStart + (busstopCount * rowHeight));

        qApp->processEvents();
    }
    painter->setFont(fontNormal);

    QRect tableBorder(0, tableStart, pageWidth, rowHeight * busstopCount);
    painter->setPen(penDefault);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(tableBorder.adjusted(- penDefault.width() / 2, - penDefault.width() / 2, penDefault.width() / 2, penDefault.width() / 2));

    // footer
    QRect footer(0, pageHeight - 45 + 25, pageWidth, 45);
    painter->drawText(footer, Qt::AlignRight|Qt::AlignBottom, tr("Page 1 of 1"));

    currentDocument->newPage();

    writeNewPage();
}
