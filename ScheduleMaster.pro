QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    Global/AppInfo.cpp \
    Global/Global.cpp \
    Global/LocalConfig.cpp \
    Global/Logger.cpp \
    ProjectData/ProjectData.cpp \
    ProjectData/Busstop.cpp \
    ProjectData/BusstopPlatform.cpp \
    ProjectData/Line.cpp \
    ProjectData/LineDirection.cpp \
    ProjectData/Route.cpp \
    ProjectData/RouteBusstopItem.cpp \
    Widgets/WdgAbstract.cpp \
    Widgets/WdgWelcome.cpp \
    Widgets/WdgWelcomeRecentProjectEntry.cpp

HEADERS += \
    MainWindow.h \
    Global/AppInfo.h \
    Global/Global.h \
    Global/LocalConfig.h \
    Global/Logger.h \
    ProjectData/ProjectData.h \
    ProjectData/ProjectDataItem.h \
    ProjectData/ProjectDataItemList.h \
    ProjectData/ProjectDataItemSet.h \
    ProjectData/ProjectDataItemSignals.h \
    ProjectData/Busstop.h \
    ProjectData/BusstopPlatform.h \
    ProjectData/Line.h \
    ProjectData/LineDirection.h \
    ProjectData/Route.h \
    ProjectData/RouteBusstopItem.h \
    Widgets/WdgAbstract.h \
    Widgets/WdgWelcome.h \
    Widgets/WdgWelcomeRecentProjectEntry.h

FORMS += \
    MainWindow.ui \
    Widgets/WdgWelcome.ui \
    Widgets/WdgWelcomeRecentProjectEntry.ui

RESOURCES = Resources.qrc

RC_ICONS = Icons/ScheduleMaster_64px.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
