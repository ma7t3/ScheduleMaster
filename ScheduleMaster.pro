QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 rtti

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    App/logger.cpp \
    AppInfo.cpp \
    DataModels/BusstopTableModel.cpp \
    DataModels/LineTableModel.cpp \
    DataModels/RouteTableModel.cpp \
    Dialogs/DlgChangelog.cpp \
    Dialogs/DlgCopyTrip.cpp \
    Dialogs/DlgFootnoteEditor.cpp \
    Dialogs/DlgOmsiImport.cpp \
    Dialogs/DlgPdfExporter.cpp \
    Dialogs/DlgPreferences.cpp \
    Dialogs/DlgProgressLogger.cpp \
    Dialogs/DlgProjectsettings.cpp \
    ProjectData/FileHandler.cpp \
    Widgets/WdgChangelogBrowser.cpp \
    Widgets/WdgFootnotes.cpp \
    Mainwindow.cpp \
    ProjectData/daytype.cpp \
    ProjectData/footnote.cpp \
    ProjectData/linedirection.cpp \
    ProjectData/projectsettings.cpp \
    ProjectData/publications.cpp \
    ProjectData/publishedbusstop.cpp \
    ProjectData/publishedline.cpp \
    ProjectData/publishedlinedirection.cpp \
    ProjectData/weekdays.cpp \
    ProjectData/ProjectDataItem.cpp \
    ProjectData/busstop.cpp \
    Dialogs/DlgBusstopeditor.cpp \
    Dialogs/DlgDataexporter.cpp \
    Widgets/Publications/wdgpublishedlines.cpp \
    ProjectData/line.cpp \
    Dialogs/DlgLineeditor.cpp \
    Widgets/WdgDaySelector.cpp \
    Widgets/WdgRouteSelector.cpp \
    Widgets/WdgScheduleTable.cpp \
    Widgets/wdgbusstops.cpp \
    Widgets/wdglines.cpp \
    Widgets/wdgroutes.cpp \
    Widgets/wdgschedule.cpp \
    Widgets/wdgtoureditor.cpp \
    Widgets/wdgtours.cpp \
    Widgets/wdgtripeditor.cpp \
    Widgets/wdgundoview.cpp \
    Widgets/wdgbusstopschedule.cpp \
    localconfig.cpp \
    main.cpp \
    ProjectData/projectdata.cpp \
    Dialogs/DlgProjecttreeviewer.cpp \
    ProjectData/route.cpp \
    Dialogs/DlgRouteeditor.cpp \
    Dialogs/DlgStartupdialog.cpp \
    ProjectData/timeProfile.cpp \
    Dialogs/DlgTimeprofileeditor.cpp \
    ProjectData/tour.cpp \
    Dialogs/DlgToureditor.cpp \
    ProjectData/trip.cpp \
    Dialogs/DlgTripselector.cpp \
    Dialogs/DlgTroubleshooter.cpp \
    Plugins/PlgOmsiImport.cpp

HEADERS += \
    App/global.h \
    App/logger.h \
    AppInfo.h \
    Commands/CmdBusstops.h \
    Commands/CmdFootnotes.h \
    Commands/CmdGeneral.h \
    Commands/CmdLines.h \
    Commands/CmdPublications.h \
    Commands/CmdRoutes.h \
    Commands/CmdSchedule.h \
    Commands/CmdTours.h \
    DataModels/BusstopTableModel.h \
    DataModels/LineTableModel.h \
    DataModels/ProjectDataRowBasedTableModel.h \
    DataModels/ProjectDataTableModel.h \
    DataModels/ProjectDataTableSignals.h \
    DataModels/RouteTableModel.h \
    Dialogs/DlgChangelog.h \
    Dialogs/DlgCopyTrip.h \
    Dialogs/DlgFootnoteEditor.h \
    Dialogs/DlgOmsiImport.h \
    Dialogs/DlgPdfExporter.h \
    Dialogs/DlgPreferences.h \
    Dialogs/DlgProgressLogger.h \
    Dialogs/DlgProjectsettings.h \
    Mainwindow.h \
    ProjectData/FileHandler.h \
    ProjectData/daytype.h \
    ProjectData/footnote.h \
    ProjectData/linedirection.h \
    ProjectData/projectsettings.h \
    ProjectData/ProjectDataItem.h \
    ProjectData/busstop.h \
    Dialogs/DlgBusstopeditor.h \
    Dialogs/DlgDataexporter.h \
    ProjectData/publications.h \
    ProjectData/publishedbusstop.h \
    ProjectData/publishedline.h \
    ProjectData/publishedlinedirection.h \
    Widgets/Publications/wdgpublishedlines.h \
    Widgets/WdgChangelogBrowser.h \
    Widgets/WdgDaySelector.h \
    Widgets/WdgFootnotes.h \
    Widgets/WdgRouteSelector.h \
    Widgets/WdgScheduleTable.h \
    Widgets/wdgbusstops.h \
    Widgets/wdglines.h \
    Widgets/wdgroutes.h \
    Widgets/wdgschedule.h \
    Widgets/wdgtoureditor.h \
    Widgets/wdgtours.h \
    Widgets/wdgtripeditor.h \
    Widgets/wdgundoview.h \
    Widgets/wdgbusstopschedule.h \
    ProjectData/line.h \
    Dialogs/DlgLineeditor.h \
    ProjectData/projectdata.h \
    Dialogs/DlgProjecttreeviewer.h \
    ProjectData/route.h \
    Dialogs/DlgRouteeditor.h \
    Dialogs/DlgStartupdialog.h \
    ProjectData/timeProfile.h \
    Dialogs/DlgTimeprofileeditor.h \
    ProjectData/tour.h \
    Dialogs/DlgToureditor.h \
    ProjectData/trip.h \
    Dialogs/DlgTripselector.h \
    Dialogs/DlgTroubleshooter.h \
    ProjectData/weekdays.h \
    localconfig.h \
    Plugins/PlgOmsiImport.h

FORMS += \
    Dialogs/DlgChangelog.ui \
    Dialogs/DlgCopyTrip.ui \
    Dialogs/DlgFootnoteEditor.ui \
    Dialogs/DlgProgressLogger.ui \
    Widgets/WdgFootnotes.ui \
    Dialogs/DlgOmsiImport.ui \
    Dialogs/DlgPdfExporter.ui \
    Dialogs/DlgPreferences.ui \
    Dialogs/DlgProjectsettings.ui \
    Dialogs/DlgBusstopeditor.ui \
    Dialogs/DlgDataexporter.ui \
    Dialogs/DlgLineeditor.ui \
    Dialogs/DlgProjecttreeviewer.ui \
    Dialogs/DlgRouteeditor.ui \
    Dialogs/DlgStartupdialog.ui \
    Dialogs/DlgTimeprofileeditor.ui \
    Dialogs/DlgToureditor.ui \
    Dialogs/DlgTripselector.ui \
    Dialogs/DlgTroubleshooter.ui \
    Mainwindow.ui \
    Widgets/Publications/wdgpublishedlines.ui \
    Widgets/WdgDaySelector.ui \
    Widgets/WdgRouteSelector.ui \
    Widgets/wdgbusstops.ui \
    Widgets/wdglines.ui \
    Widgets/wdgroutes.ui \
    Widgets/wdgschedule.ui \
    Widgets/wdgtoureditor.ui \
    Widgets/wdgtours.ui \
    Widgets/wdgtripeditor.ui \
    Widgets/wdgundoview.ui \
    Widgets/wdgbusstopschedule.ui \

RC_ICONS = appIcon/Icon_64px.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    main.qrc
