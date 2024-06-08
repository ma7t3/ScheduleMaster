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
    DataModels/SimpleBusstopListModel.cpp \
    DataModels/SimpleRouteBusstopListModel.cpp \
    DataModels/TimeProfileTableModel.cpp \
    Dialogs/DlgBusstopeditor.cpp \
    Dialogs/DlgChangelog.cpp \
    Dialogs/DlgDataexporter.cpp \
    Dialogs/DlgCopyTrip.cpp \
    Dialogs/DlgFootnoteEditor.cpp \
    Dialogs/DlgLineeditor.cpp \
    Dialogs/DlgOmsiImport.cpp \
    Dialogs/DlgPdfExporter.cpp \
    Dialogs/DlgPreferences.cpp \
    Dialogs/DlgProgressLogger.cpp \
    Dialogs/DlgProjectsettings.cpp \
    Dialogs/DlgProjecttreeviewer.cpp \
    Dialogs/DlgRouteeditor.cpp \
    Dialogs/DlgStartupdialog.cpp \
    Dialogs/DlgTimeprofileeditor.cpp \
    Dialogs/DlgToureditor.cpp \
    Dialogs/DlgTripselector.cpp \
    Dialogs/DlgTroubleshooter.cpp \	
    LocalConfig.cpp \
    main.cpp \
    Mainwindow.cpp \
    Plugins/PlgOmsiImport.cpp \
    ProjectData/Busstop.cpp \
    ProjectData/DayType.cpp \
    ProjectData/FileHandler.cpp \
    ProjectData/Footnote.cpp \
    ProjectData/Line.cpp \
    ProjectData/LineDirection.cpp \
    ProjectData/ProjectData.cpp \
    ProjectData/ProjectDataItem.cpp \
    ProjectData/ProjectSettings.cpp \
    ProjectData/Publications.cpp \
    ProjectData/PublishedBusstop.cpp \
    ProjectData/PublishedLine.cpp \
    ProjectData/PublishedLineDirection.cpp \
    ProjectData/Route.cpp \
    ProjectData/TimeProfile.cpp \
    ProjectData/Tour.cpp \
    ProjectData/Trip.cpp \
    ProjectData/Weekdays.cpp \
    Widgets/Publications/WdgPublishedLines.cpp \
    Widgets/WdgBusstops.cpp \
    Widgets/WdgBusstopSchedule.cpp \
    Widgets/WdgChangelogBrowser.cpp \
    Widgets/WdgDaySelector.cpp \
    Widgets/WdgFootnotes.cpp \
    Widgets/WdgLines.cpp \
    Widgets/WdgRoutes.cpp \
    Widgets/WdgRouteSelector.cpp \
    Widgets/WdgSchedule.cpp \
    Widgets/WdgScheduleTable.cpp \
    Widgets/WdgTourEditor.cpp \
    Widgets/WdgTours.cpp \
    Widgets/WdgTripEditor.cpp \
    Widgets/WdgUndoView.cpp

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
    DataModels/OrderedProjectDataRowModel.h \
    DataModels/ProjectDataModel.h \
    DataModels/ProjectDataModelSignals.h \
    DataModels/RouteTableModel.h \
    DataModels/SimpleBusstopListModel.h \
    DataModels/SimpleRouteBusstopListModel.h \
    DataModels/TimeProfileTableModel.h \
    DataModels/UnorderedProjectDataRowModel.h \
    Dialogs/DlgBusstopeditor.h \
    Dialogs/DlgChangelog.h \
    Dialogs/DlgCopyTrip.h \
    Dialogs/DlgDataexporter.h \
    Dialogs/DlgFootnoteEditor.h \
    Dialogs/DlgLineeditor.h \
    Dialogs/DlgOmsiImport.h \
    Dialogs/DlgPdfExporter.h \
    Dialogs/DlgPreferences.h \
    Dialogs/DlgProgressLogger.h \
    Dialogs/DlgProjectsettings.h \
    Dialogs/DlgProjecttreeviewer.h \
    Dialogs/DlgRouteeditor.h \
    Dialogs/DlgStartupdialog.h \
    Dialogs/DlgTimeprofileeditor.h \
    Dialogs/DlgToureditor.h \
    Dialogs/DlgTripselector.h \
    Dialogs/DlgTroubleshooter.h \
    LocalConfig.h \
    Mainwindow.h \
    Plugins/PlgOmsiImport.h \
    ProjectData/Busstop.h \
    ProjectData/DayType.h \
    ProjectData/FileHandler.h \
    ProjectData/Footnote.h \
    ProjectData/Line.h \
    ProjectData/LineDirection.h \
    ProjectData/ProjectData.h \
    ProjectData/ProjectDataItem.h \
    ProjectData/ProjectSettings.h \
    ProjectData/Publications.h \
    ProjectData/PublishedBusstop.h \
    ProjectData/PublishedLine.h \
    ProjectData/PublishedLineDirection.h \
    ProjectData/Route.h \
    ProjectData/TimeProfile.h \
    ProjectData/Tour.h \
    ProjectData/Trip.h \
    ProjectData/Weekdays.h \
    Widgets/Publications/WdgPublishedLines.h \
    Widgets/WdgBusstops.h \
    Widgets/WdgBusstopSchedule.h \
    Widgets/WdgChangelogBrowser.h \
    Widgets/WdgDaySelector.h \
    Widgets/WdgFootnotes.h \
    Widgets/WdgLines.h \
    Widgets/WdgRoutes.h \
    Widgets/WdgRouteSelector.h \
    Widgets/WdgSchedule.h \
    Widgets/WdgScheduleTable.h \
    Widgets/WdgTourEditor.h \
    Widgets/WdgTours.h \
    Widgets/WdgTripEditor.h \
    Widgets/WdgUndoView.h

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
