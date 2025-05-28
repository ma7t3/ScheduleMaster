QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Dialogs/DlgConfigEditor.cpp \
    Dialogs/DlgPreferences.cpp \
    Global/ActionController.cpp \
    Global/ActionManager.cpp \
    Global/AppInfo.cpp \
    Global/CrashDetector.cpp \
    Global/DockController.cpp \
    Global/DockManager.cpp \
    Global/FolderLocationManager.cpp \
    Global/Global.cpp \
    Global/IconController.cpp \
    Global/IconSetManager.cpp \
    Global/LanguageManager.cpp \
    Global/LastUsedFilesManager.cpp \
    Global/Logger.cpp \
    Global/ProjectFileHandler.cpp \
    Global/SettingsManager.cpp \
    Global/StyleHandler.cpp \
    Global/StyleManager.cpp \
    Global/VariantConverter.cpp \
    Global/Workspace.cpp \
    Global/WorkspaceHandler.cpp \
    Global/WorkspaceManager.cpp \
    ItemModels/IconSetsModel.cpp \
    ItemModels/KeyboardShortcutsModel.cpp \
    ItemModels/LanguagesModel.cpp \
    ItemModels/LocalConfigModel.cpp \
    ItemModels/StylesModel.cpp \
    Widgets/Docks/DockAbstract.cpp \
    Widgets/Docks/DockWelcome.cpp \
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
    Widgets/WdgAccentColorSelector.cpp \
    Widgets/WdgColorSchemeSelector.cpp \
    Widgets/WdgVariantEditor.cpp \
    Widgets/WdgWelcomeRecentProjectEntry.cpp

HEADERS += \
    Commands/CmdBusstops.h \
    Commands/CmdGeneral.h \
    Dialogs/DlgConfigEditor.h \
    Dialogs/DlgPreferences.h \
    Global/ActionController.h \
    Global/ActionManager.h \
    Global/AppInfo.h \
    Global/CrashDetector.h \
    Global/DockController.h \
    Global/DockManager.h \
    Global/FolderLocationManager.h \
    Global/Global.h \
    Global/GlobalConfigManager.h \
    Global/GlobalConfigManagerSignals.h \
    Global/IconController.h \
    Global/IconSetManager.h \
    Global/LanguageManager.h \
    Global/LastUsedFilesManager.h \
    Global/Logger.h \
    Global/ProjectFileHandler.h \
    Global/SettingsManager.h \
    Global/StyleHandler.h \
    Global/StyleManager.h \
    Global/VariantConverter.h \
    Global/Workspace.h \
    Global/WorkspaceHandler.h \
    Global/WorkspaceManager.h \
    ItemModels/IconSetsModel.h \
    ItemModels/KeyboardShortcutsModel.h \
    ItemModels/LanguagesModel.h \
    ItemModels/LocalConfigModel.h \
    ItemModels/StylesModel.h \
    MainWindow.h \
    MainWindowInterface.h \
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
    Widgets/Docks/DockAbstract.h \
    Widgets/Docks/DockWelcome.h \
    Widgets/WdgAccentColorSelector.h \
    Widgets/WdgColorSchemeSelector.h \
    Widgets/WdgVariantEditor.h \
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
    Widgets/Docks/DockWelcome.ui \
    Widgets/WdgAccentColorSelector.ui \
    Widgets/WdgColorSchemeSelector.ui \
    Widgets/WdgVariantEditor.ui \
    Widgets/WdgWelcomeRecentProjectEntry.ui

RESOURCES = Resources.qrc

RC_ICONS = Icons/ScheduleMaster_64px.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
