QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Dialogs/DlgBusstopEditor.cpp \
    Dialogs/DlgConfigEditor.cpp \
    Dialogs/DlgGlobalSearch.cpp \
    Dialogs/DlgLineEditor.cpp \
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
    InterfaceImpl/AppInterfaceImpl.cpp \
    InterfaceImpl/ProjectManagerImpl.cpp \
    ItemModels/IconSetsModel.cpp \
    ItemModels/KeyboardShortcutsModel.cpp \
    ItemModels/LanguagesModel.cpp \
    ItemModels/LocalConfigModel.cpp \
    ItemModels/StylesModel.cpp \
    ProjectDataModels/BusstopPlatformTableModel.cpp \
    ProjectDataModels/BusstopTableModel.cpp \
    ProjectDataModels/LineDirectionTableModel.cpp \
    ProjectDataModels/LineTableModel.cpp \
    ProjectDataModels/LineTableProxyModel.cpp \
    ProjectDataModels/RouteTableModel.cpp \
    ProjectDataModels/RouteTableProxyModel.cpp \
    Widgets/Docks/DockBusstops.cpp \
    Widgets/Docks/DockLines.cpp \
    Widgets/Docks/DockNews.cpp \
    Widgets/Docks/DockRoutes.cpp \
    Widgets/Docks/DockWelcome.cpp \
    Widgets/WdgBusstopFlagEditor.cpp \
    Widgets/WdgBusstopPlatformEditor.cpp \
    Widgets/WdgColorSelector.cpp \
    Widgets/WdgGlobalSearchItem.cpp \
    Widgets/WdgTableColumnVisibilitySelector.cpp \
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
    Commands/CmdLines.h \
    Dialogs/DlgBusstopEditor.h \
    Dialogs/DlgConfigEditor.h \
    Dialogs/DlgGlobalSearch.h \
    Dialogs/DlgLineEditor.h \
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
    Global/Singleton.h \
    Global/StyleHandler.h \
    Global/StyleManager.h \
    Global/VariantConverter.h \
    Global/Workspace.h \
    Global/WorkspaceHandler.h \
    Global/WorkspaceManager.h \
	InterfaceImpl/AppInterfaceImpl.h \
    InterfaceImpl/ProjectManagerImpl.h \
    ItemModels/IconSetsModel.h \
    ItemModels/KeyboardShortcutsModel.h \
    ItemModels/LanguagesModel.h \
    ItemModels/LocalConfigModel.h \
    ItemModels/StylesModel.h \
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
    ProjectDataModels/BusstopPlatformTableModel.h \
    ProjectDataModels/BusstopTableModel.h \
    ProjectDataModels/LineDirectionTableModel.h \
    ProjectDataModels/LineTableModel.h \
    ProjectDataModels/LineTableProxyModel.h \
    ProjectDataModels/OrderedProjectDataRowModel.h \
    ProjectDataModels/RouteTableModel.h \
    ProjectDataModels/RouteTableProxyModel.h \
    ProjectDataModels/UnorderedProjectDataRowModel.h \
    ProjectDataModels/UnorderedProjectDataRowModelSignals.h \
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
    Widgets/Docks/DockBusstops.h \
    Widgets/Docks/DockLines.h \
    Widgets/Docks/DockNews.h \
    Widgets/Docks/DockRoutes.h \
    Widgets/Docks/DockWelcome.h \
    Widgets/WdgAccentColorSelector.h \
    Widgets/WdgBusstopFlagEditor.h \
    Widgets/WdgBusstopPlatformEditor.h \
    Widgets/WdgColorSchemeSelector.h \
    Widgets/WdgColorSelector.h \
    Widgets/WdgGlobalSearchItem.h \
    Widgets/WdgTableColumnVisibilitySelector.h \
    Widgets/WdgVariantEditor.h \
    Widgets/WdgWelcomeRecentProjectEntry.h

FORMS += \
    Dialogs/DlgBusstopEditor.ui \
    Dialogs/DlgConfigEditor.ui \
    Dialogs/DlgGlobalSearch.ui \
    Dialogs/DlgLineEditor.ui \
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
    Widgets/Docks/DockBusstops.ui \
    Widgets/Docks/DockLines.ui \
    Widgets/Docks/DockNews.ui \
    Widgets/Docks/DockRoutes.ui \
    Widgets/Docks/DockWelcome.ui \
    Widgets/WdgAccentColorSelector.ui \
    Widgets/WdgBusstopFlagEditor.ui \
    Widgets/WdgBusstopPlatformEditor.ui \
    Widgets/WdgColorSchemeSelector.ui \
    Widgets/WdgColorSelector.ui \
    Widgets/WdgGlobalSearchItem.ui \
    Widgets/WdgTableColumnVisibilitySelector.ui \
    Widgets/WdgVariantEditor.ui \
    Widgets/WdgWelcomeRecentProjectEntry.ui

INCLUDEPATH += $$PWD/ScheduleMasterInterface

LIBS += -L$$PWD/lib \
    -lScheduleMasterInterface

RESOURCES = Resources.qrc

RC_ICONS = Icons/ScheduleMaster_64px.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
