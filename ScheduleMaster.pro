QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Dialogs/DlgConfigEditor.cpp \
    Dialogs/DlgPreferences.cpp \
    Global/ActionShortcutMapper.cpp \
    Global/AppInfo.cpp \
    Global/Global.cpp \
    Global/GlobalConfig.cpp \
    Global/LocalConfig.cpp \
    Global/Logger.cpp \
    Global/StyleHandler.cpp \
    Global/VariantConverter.cpp \
    Global/Workspace.cpp \
    Global/WorkspaceHandler.cpp \
    ItemModels/KeyboardShortcutsModel.cpp \
    ItemModels/LanguagesModel.cpp \
    ItemModels/LocalConfigModel.cpp \
    main.cpp \
    MainWindow.cpp \
    ProjectData/ProjectData.cpp \
    ProjectData/Busstop.cpp \
    ProjectData/BusstopPlatform.cpp \
    ProjectData/Line.cpp \
    ProjectData/LineDirection.cpp \
    ProjectData/Route.cpp \
    ProjectData/RouteBusstopItem.cpp \
    Widgets/DlgPreferencesPages/WdgPreferencesPage.cpp \
    Widgets/DlgPreferencesPages/WdgPreferencesPageAppearance.cpp \
    Widgets/DlgPreferencesPages/WdgPreferencesPageDebug.cpp \
    Widgets/DlgPreferencesPages/WdgPreferencesPageGeneral.cpp \
    Widgets/DlgPreferencesPages/WdgPreferencesPageHome.cpp \
    Widgets/DlgPreferencesPages/WdgPreferencesPageKeyboardShortcuts.cpp \
    Widgets/DlgPreferencesPages/WdgPreferencesPageLocations.cpp \
    Widgets/DlgPreferencesPages/WdgPreferencesPagePlugins.cpp \
    Widgets/DlgPreferencesPages/WdgPreferencesPageUpdates.cpp \
    Widgets/WdgAbstract.cpp \
    Widgets/WdgAccentColorSelector.cpp \
    Widgets/WdgColorSchemeSelector.cpp \
    Widgets/WdgVariantEditor.cpp \
    Widgets/WdgWelcome.cpp \
    Widgets/WdgWelcomeRecentProjectEntry.cpp

HEADERS += \
    Dialogs/DlgConfigEditor.h \
    Dialogs/DlgPreferences.h \
    Global/ActionShortcutMapper.h \
    Global/AppInfo.h \
    Global/Global.h \
    Global/GlobalConfig.h \
    Global/LocalConfig.h \
    Global/Logger.h \
    Global/StyleHandler.h \
    Global/VariantConverter.h \
    Global/Workspace.h \
    Global/WorkspaceHandler.h \
    ItemModels/KeyboardShortcutsModel.h \
    ItemModels/LanguagesModel.h \
    ItemModels/LocalConfigModel.h \
    MainWindow.h \
    ProjectData/ProjectData.h \
    ProjectData/ProjectDataItem.h \
    ProjectData/ProjectDataItemSignals.h \
    ProjectData/ProjectDataItemData.h \
    ProjectData/ProjectDataItemContainer.h \
    ProjectData/ProjectDataItemList.h \
    ProjectData/ProjectDataItemSet.h \
    ProjectData/Busstop.h \
    ProjectData/BusstopPlatform.h \
    ProjectData/Line.h \
    ProjectData/LineDirection.h \
    ProjectData/Route.h \
    ProjectData/RouteBusstopItem.h \
    Widgets/DlgPreferencesPages/WdgPreferencesPage.h \
    Widgets/DlgPreferencesPages/WdgPreferencesPageAppearance.h \
    Widgets/DlgPreferencesPages/WdgPreferencesPageDebug.h \
    Widgets/DlgPreferencesPages/WdgPreferencesPageGeneral.h \
    Widgets/DlgPreferencesPages/WdgPreferencesPageHome.h \
    Widgets/DlgPreferencesPages/WdgPreferencesPageKeyboardShortcuts.h \
    Widgets/DlgPreferencesPages/WdgPreferencesPageLocations.h \
    Widgets/DlgPreferencesPages/WdgPreferencesPagePlugins.h \
    Widgets/DlgPreferencesPages/WdgPreferencesPageUpdates.h \
    Widgets/WdgAbstract.h \
    Widgets/WdgAccentColorSelector.h \
    Widgets/WdgColorSchemeSelector.h \
    Widgets/WdgVariantEditor.h \
    Widgets/WdgWelcome.h \
    Widgets/WdgWelcomeRecentProjectEntry.h

FORMS += \
    Dialogs/DlgConfigEditor.ui \
    Dialogs/DlgPreferences.ui \
    MainWindow.ui \
    Widgets/DlgPreferencesPages/WdgPreferencesPageAppearance.ui \
    Widgets/DlgPreferencesPages/WdgPreferencesPageDebug.ui \
    Widgets/DlgPreferencesPages/WdgPreferencesPageGeneral.ui \
    Widgets/DlgPreferencesPages/WdgPreferencesPageHome.ui \
    Widgets/DlgPreferencesPages/WdgPreferencesPageKeyboardShortcuts.ui \
    Widgets/DlgPreferencesPages/WdgPreferencesPageLocations.ui \
    Widgets/DlgPreferencesPages/WdgPreferencesPagePlugins.ui \
    Widgets/DlgPreferencesPages/WdgPreferencesPageUpdates.ui \
    Widgets/WdgAccentColorSelector.ui \
    Widgets/WdgColorSchemeSelector.ui \
    Widgets/WdgVariantEditor.ui \
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
