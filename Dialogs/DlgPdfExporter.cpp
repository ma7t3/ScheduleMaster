#include "DlgPdfExporter.h"
#include "ui_DlgPdfExporter.h"

#include "localconfig.h"

#include <QPdfWriter>
#include <QPainter>
#include <QDesktopServices>
#include <QMessageBox>

DlgPdfExporter::DlgPdfExporter(QWidget *parent, ProjectData *projectData) :
    QDialog(parent),
    ui(new Ui::DlgPdfExporter),
    projectData(projectData),
    currentDocument(nullptr),
    _currentLine(nullptr),
    _currentDirection(nullptr)
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

void DlgPdfExporter::exportAll() {
    exportAllLineSchedules();
    exportAllTours();
}

void DlgPdfExporter::exportAllLineSchedules() {
    this->show();
    qApp->processEvents();

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
    _currentLine = l;
    
    QString fileName = _currentLine->filePath();
    QFile f(fileName);
    if(!f.open(QFile::WriteOnly)) {
        logError(tr("Couldn't write file: %1 - Access denied").arg(l->title()));
        QMessageBox::critical(this, tr("Failed creating PDF file"), tr("<p><b>Couldn't write file: %1</b></p><p>Reason: %2</p>").arg(l->title(), f.errorString()));
        return;
    }

    QFileInfo fi(f);
    QString pdfTitle = fi.baseName();

    // init pdf document
    currentDocument = new QPdfWriter(fileName);
    currentDocument->setResolution(254);
    currentDocument->setCreator("ScheduleMaster");
    currentDocument->setTitle(tr("Linienfahrplan - %1").arg(pdfTitle));
    currentDocument->setPageLayout(QPageLayout(QPageSize::A4, QPageLayout::Landscape, QMargins(10, 10, 10, 10), QPageLayout::Millimeter));

    painter = new QPainter(currentDocument);

    for(int i = 0; i < _currentLine->directionCount(); i++) {
        _currentDirection = _currentLine->directionAt(i);

        // collect trips
        QList<Trip *> parentTrips;
        QList<Trip *> allTrips;
        for(int j = 0; j < _currentDirection->routeCount(); j++) {
            Route *r = _currentDirection->routeAt(j);
            parentTrips << projectData->lineOfRoute(r)->tripsOfRoute(r);
        }
        for(int j = 0; j < parentTrips.count(); j++) {
            Trip *t = parentTrips[j];

            if(!t->weekDays().isIn(WeekDays(nullptr, 995)))
                continue;

            allTrips << t;
        }
        allTrips = ProjectData::sortItems(allTrips);
        _trips = allTrips;
        writeNewPage();
    }

    painter->end();

    logInfo(tr("Line finished: %1").arg(_currentLine->title()));

    qApp->processEvents();

    //QDesktopServices::openUrl(QUrl(fileName));
}

void DlgPdfExporter::exportAllTours() {
    this->show();
    qApp->processEvents();

    ui->twLog->clear();

    ui->progressBar->setMaximum(projectData->tourCount() - 1);

    for(int i = 0; i < projectData->tourCount(); i++) {
        Tour *o = projectData->tourAt(i);
        exportTour(o);
        ui->progressBar->setValue(i + 1);
    }

    this->close();
}

void DlgPdfExporter::writeNewPage() {

    if(_trips.isEmpty())
        return;

    //qDebug() << _currentLine->title() + " " + _currentDirection->name() + " " << _trips.count();
    int pageWidth = 2770;
    int pageHeight = 1900;

    painter->setPen(penDefault);
    painter->setBrush(brushWhite);
    painter->setFont(fontHeadline);

    // headline
    QRect headlineRectLeft(0, 0, 250, 75);
    QRect headlineRectCenter(270, 0, 2230, 75);
    QRect headlineRectRight(2520, 0, 250, 75);
    painter->drawRect(headlineRectLeft);
    painter->drawRect(headlineRectCenter);
    painter->drawRect(headlineRectRight);
    painter->drawText(headlineRectLeft.adjusted(10, 0, -10, 0), Qt::AlignCenter|Qt::AlignVCenter, _currentLine->title());
    painter->drawText(headlineRectCenter.adjusted(10, 0, -10, 0), Qt::AlignCenter|Qt::AlignVCenter, _currentDirection->name());
    painter->setFont(fontNormal);

    // icon
    QPixmap icon(projectData->projectSettings()->icon());
    int rectWidth = icon.scaledToHeight(headlineRectRight.height() - 10).width();
    int diff = headlineRectRight.width() - rectWidth;
    painter->drawPixmap(headlineRectRight.adjusted(diff / 2, 5, - (diff / 2), -5), icon);


    // table
    painter->setBackground(brushGray);
    int tableStart = 150;
    int tableBodyStart = tableStart;
    int maxBusstopCount = 45;
    int columnWidth = 110;
    int firstColumnWidth = columnWidth * 4 + 15;
    int maxColumnCount = 21; // 25 - firstColumnWidthFactor sozusagen
    int rowHeight = 40;

    int busstopCount = _currentDirection->busstopCount();
    if(busstopCount > maxBusstopCount)
        busstopCount = maxBusstopCount;


    bool headerShowLine = true;
    bool headerShowTour = false;
    bool headerShowFootnotes = true;
    bool headerShowTripNo = false;


    // table header
    painter->setFont(fontNormalBold);
    if(headerShowTour) {
        tableBodyStart += rowHeight;
        QRect labelField(5, tableBodyStart - rowHeight, firstColumnWidth - 10, rowHeight);
        painter->drawText(labelField, tr("Tour"), Qt::AlignLeft|Qt::AlignVCenter);
    }
    if(headerShowLine) {
        tableBodyStart += rowHeight;
        QRect labelField(5, tableBodyStart - rowHeight, firstColumnWidth - 10, rowHeight);
        painter->drawText(labelField, tr("Line"), Qt::AlignLeft|Qt::AlignVCenter);
    }
    if(headerShowTripNo) {
        tableBodyStart += rowHeight;
        QRect labelField(5, tableBodyStart - rowHeight, firstColumnWidth - 10, rowHeight);
        painter->drawText(labelField, tr("Trip Number"), Qt::AlignLeft|Qt::AlignVCenter);
    }
    if(headerShowFootnotes) {
        tableBodyStart += rowHeight;
        QRect labelField(5, tableBodyStart - rowHeight, firstColumnWidth - 10, rowHeight);
        painter->drawText(labelField, tr("Footnotes"), Qt::AlignLeft|Qt::AlignVCenter);
    }
    QRect tableHeaderBorder(0, tableStart, pageWidth, tableBodyStart);
    painter->setPen(penDefault);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(tableHeaderBorder.adjusted(- penDefault.width() / 2, - penDefault.width() / 2, penDefault.width() / 2, penDefault.width() / 2));

    // table body and busstops
    for(int i = 0; i < busstopCount; i++) {
        QRect row(0, tableBodyStart + (i * rowHeight), pageWidth, rowHeight);
        QRect busstopNameField(5, tableBodyStart + (i * rowHeight), firstColumnWidth - 10, rowHeight);
        if(i % 2 == 0) {
            //painter->setBrush(brushGray);
        } else {
            painter->setBrush(brushWhite);
        }

        painter->setPen(Qt::NoPen);
        painter->drawRect(row);

        painter->setPen(penDefault);


        PublishedBusstop *b =  _currentDirection->busstopAt(i);
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
    painter->drawLine(firstColumnWidth, tableStart, firstColumnWidth, tableBodyStart + busstopCount * rowHeight);


    // trips

    QList<Trip *> trips = _trips.sliced(0, _trips.count() >= maxColumnCount ? maxColumnCount : _trips.count());
    _trips.remove(0, _trips.count() >= maxColumnCount ? maxColumnCount : _trips.count());

    painter->setPen(penMedium);

    for(int i = 0; i < trips.count(); i++) { // for each trip
        int lastBusstopIndex = -1;

        // head data

        QList<Footnote *> footnotes = projectData->autoAssignedFootnotesOfTrip(trips[i]);
        QStringList footnotesStr;
        for(int j = 0; j < footnotes.count(); j++)
            footnotesStr << footnotes[j]->identifier();

        QRect labelField(i * columnWidth + 10 + firstColumnWidth, tableStart, columnWidth - 10, rowHeight);

        painter->setFont(fontNormal);
        if(headerShowTour) {
            QList<Tour *> tours = projectData->toursOfTrip(trips[i]);
            painter->drawText(labelField, tours.isEmpty() ? "" : tours[0]->name(), Qt::AlignCenter|Qt::AlignVCenter);
            labelField.adjust(0, rowHeight, 0, rowHeight);
        }
        if(headerShowLine) {
            painter->drawText(labelField, projectData->lineOfTrip(trips[i])->name(), Qt::AlignCenter|Qt::AlignVCenter);
            labelField.adjust(0, rowHeight, 0, rowHeight);
        }
        if(headerShowTripNo) {
            painter->drawText(labelField, "", Qt::AlignCenter|Qt::AlignVCenter);
            labelField.adjust(0, rowHeight, 0, rowHeight);
        }
        if(headerShowFootnotes) {
            painter->setFont(fontNormalBold);
            painter->drawText(labelField, footnotesStr.join(" "), Qt::AlignCenter|Qt::AlignVCenter);
            labelField.adjust(0, rowHeight, 0, rowHeight);
        }


        for(int j = 0; j < busstopCount; j++) { // for each busstop
            QRect field(i * columnWidth + 10 + firstColumnWidth, tableBodyStart + (j * rowHeight), columnWidth - 10, rowHeight);
            if(_currentDirection->busstopAt(j)->linkedBusstop()->isImportant())
                painter->setFont(fontNormalBold);
            else
                painter->setFont(fontNormal);

            if(!trips[i]->route()->hasBusstop(_currentDirection->busstopAt(j)->linkedBusstop()))
                continue;

            // draw line
            if(j - lastBusstopIndex > 1 && lastBusstopIndex != -1) {
                painter->drawLine(i * columnWidth + firstColumnWidth + (columnWidth / 2) + 5, tableBodyStart + ((lastBusstopIndex + 1) * rowHeight) + 5, i * columnWidth + firstColumnWidth + (columnWidth / 2) + 5, tableBodyStart + (j * rowHeight) - 5);
            }

            lastBusstopIndex = j;

            QString time = trips[i]->busstopTime(_currentDirection->busstopAt(j)->linkedBusstop()).toString(LocalConfig::timeFormatString(false, false));

            painter->drawText(field, Qt::AlignCenter|Qt::AlignVCenter, time);

            painter->setPen(penThin);
            // draw horizontal divider
            if(_currentDirection->busstopAt(j)->showDivider())
                painter->drawLine(0, tableBodyStart + (j + 1) * rowHeight, pageWidth, tableBodyStart + (j + 1) * rowHeight);

            painter->setPen(penMedium);
        }
        // draw vertical divider
        if(i > 0)
            painter->drawLine(i * columnWidth + firstColumnWidth + 5, tableStart, i * columnWidth + firstColumnWidth + 5, tableBodyStart + (busstopCount * rowHeight));

        qApp->processEvents();
    }
    painter->setFont(fontNormal);

    QRect tableBorder(0, tableBodyStart, pageWidth, rowHeight * busstopCount);
    painter->setPen(penDefault);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(tableBorder.adjusted(- penDefault.width() / 2, - penDefault.width() / 2, penDefault.width() / 2, penDefault.width() / 2));

    // footer
    QRect footer(0, pageHeight - 45 + 25, pageWidth, 45);
    painter->drawText(footer, Qt::AlignLeft|Qt::AlignBottom, _currentLine->footer());
    painter->drawText(footer, Qt::AlignRight|Qt::AlignBottom, tr("Page %1 of %2").arg("1", "1"));

    currentDocument->newPage();

    writeNewPage();
}

void DlgPdfExporter::exportTour(Tour *o) {

    QDir dir = QDir::homePath() + "/.ScheduleMaster/Publications/" + projectData->projectSettings()->displayName() + "/Tour Plans";
    if(!dir.exists())
        dir.mkpath(dir.path());

    QString fileName = dir.path() + "/" + o->name() + ".pdf";

    QFile f(fileName);
    if(!f.open(QFile::WriteOnly))
        return;

    QFileInfo fi(f);
    QString pdfTitle = fi.baseName();

    // init pdf document
    currentDocument = new QPdfWriter(fileName);
    currentDocument->setResolution(254);
    currentDocument->setCreator("ScheduleMaster");
    currentDocument->setTitle(tr("Tour plan - %1").arg(pdfTitle));
    currentDocument->setPageLayout(QPageLayout(QPageSize::A4, QPageLayout::Portrait, QMargins(10, 10, 10, 10), QPageLayout::Millimeter));

    painter = new QPainter(currentDocument);

    int pageWidth = 1900;
    int pageHeight = 2770;

    painter->setFont(fontHeadline);
    painter->setPen(penDefault);

    QRect headlineRect1(0, 0, 300, 75);
    QRect headlineRect2(320, 0, 1260, 75);
    QRect headlineRect3(1600, 0, 300, 75);
    painter->drawRect(headlineRect1);
    painter->drawRect(headlineRect2);
    painter->drawRect(headlineRect3);

    painter->drawText(headlineRect1.adjusted(5, 0, -5, 0), o->name(), Qt::AlignCenter|Qt::AlignVCenter);
    painter->drawText(headlineRect2.adjusted(5, 0, -5, 0), tr("Tour plan"), Qt::AlignCenter|Qt::AlignVCenter);
    painter->drawText(headlineRect3.adjusted(5, 0, -5, 0), o->weekDays().toString(), Qt::AlignCenter|Qt::AlignVCenter);


    // draw table
    QRect table(0, 100, 1900, 2500);
    painter->drawRect(table);
    painter->setPen(penMedium);
    painter->drawLine(200, 100, 200, 2600);
    painter->drawLine(400, 100, 400, 2600);

    /*QRect lineField(10, 100, 180, 85);
    QRect timeField(210, 100, 180, 85);
    QRect tripField(410, 100, 1480, 85);*/

    painter->setFont(fontNormal);

    int lastXCoordinate = 100;

    for(int i = 0; i < o->tripCount(); i++) {
        Trip *t = o->tripAt(i);

        QString timeText, tripText;
        int rowCount = 2;

        Busstop *firstBusstop, *lastBusstop;
        firstBusstop = t->route()->firstBusstop();
        lastBusstop = t->route()->lastBusstop();

        timeText += t->startTime().toString(LocalConfig::timeFormatString(false, false)) + "\r\n";
        if(firstBusstop)
            tripText += t->route()->firstBusstop()->name() + "\r\n";
        else
            tripText += "\r\n";

        Route *r = t->route();
        for(int j = 0; j < r->busstopCount(); j++) {
            Busstop *b = r->busstopAt(j);
            if(!b->important())
                continue;

            if(b == firstBusstop || b == lastBusstop)
                continue;

            timeText += t->busstopTime(b).toString(LocalConfig::timeFormatString(false, false)) + "\r\n";
            tripText += b->name() + "\r\n";
            rowCount++;
        }

        timeText += t->endTime().toString(LocalConfig::timeFormatString(false, false));

        if(lastBusstop)
            tripText += t->route()->lastBusstop()->name();
        else
            tripText += "\r\n";

        QRect lineField(10, lastXCoordinate, 180, rowCount * 50);
        QRect timeField(210, lastXCoordinate, 180, rowCount * 50);
        QRect tripField(410, lastXCoordinate, 1480, rowCount * 50);

        lastXCoordinate += rowCount * 50;

        QString routeCodeStr = QString::number(t->route()->code());
        if(routeCodeStr.length() == 1)
            routeCodeStr = "0" + routeCodeStr;

        painter->drawText(lineField, projectData->lineOfTrip(t)->name() + " / " + routeCodeStr);
        painter->drawText(timeField, timeText);
        painter->drawText(tripField, tripText);

        // adjust fields

    }

    painter->end();

    QIcon icon(":/icons/success.ico");
    QTreeWidgetItem *itm = new QTreeWidgetItem({tr("Tour finished finished: %1").arg(o->name())});
    itm->setIcon(0, icon);
    ui->twLog->addTopLevelItem(itm);
    ui->twLog->scrollToBottom();
    qApp->processEvents();
}

void DlgPdfExporter::logInfo(QString message) {
    QIcon icon(":/icons/success.ico");
    QTreeWidgetItem *itm = new QTreeWidgetItem({message});
    itm->setIcon(0, icon);
    ui->twLog->addTopLevelItem(itm);
    ui->twLog->scrollToBottom();
}

void DlgPdfExporter::logError(QString message) {
    QIcon icon(":/icons/error.ico");
    QTreeWidgetItem *itm = new QTreeWidgetItem({message});
    itm->setIcon(0, icon);
    ui->twLog->addTopLevelItem(itm);
    ui->twLog->scrollToBottom();
}
