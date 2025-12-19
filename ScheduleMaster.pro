QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/ui/dialogs/DlgBusstopEditor.cpp \
    src/ui/dialogs/DlgConfigEditor.cpp \
    src/ui/dialogs/DlgGlobalSearch.cpp \
    src/ui/dialogs/DlgLineEditor.cpp \
    src/ui/dialogs/DlgPreferences.cpp \
    src/ui/dialogs/DlgRouteEditor.cpp \
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
    src/projectdata/TimeProfile.cpp \
    src/projectdata/TimeProfileItem.cpp \
    ProjectDataModels/BusstopPlatformTableModel.cpp \
    ProjectDataModels/BusstopTableModel.cpp \
    ProjectDataModels/BusstopTableProxyModel.cpp \
    ProjectDataModels/LineDirectionTableModel.cpp \
    ProjectDataModels/LineSelectionListModel.cpp \
    ProjectDataModels/LineSelectionListProxyModel.cpp \
    ProjectDataModels/LineTableModel.cpp \
    ProjectDataModels/LineTableProxyModel.cpp \
    ProjectDataModels/RouteBusstopTableModel.cpp \
    ProjectDataModels/RouteTableModel.cpp \
    ProjectDataModels/RouteTableProxyModel.cpp \
    ProjectDataModels/TimeProfileTableModel.cpp \
    src/ui/widgets/Docks/DockBusstops.cpp \
    src/ui/widgets/Docks/DockLines.cpp \
    src/ui/widgets/Docks/DockNews.cpp \
    src/ui/widgets/Docks/DockRoutes.cpp \
    src/ui/widgets/Docks/DockWelcome.cpp \
    src/ui/widgets/FilterPopups/WdgBusstopsFilterPopup.cpp \
    src/ui/widgets/FilterPopups/WdgFilterPopup.cpp \
    src/ui/widgets/FilterPopups/WdgFilterPopupContent.cpp \
    src/ui/widgets/FilterPopups/WdgLineFilterPopup.cpp \
    src/ui/widgets/FilterPopups/WdgRouteFilterPopup.cpp \
    src/ui/widgets/WdgBusstopFlagEditor.cpp \
    src/ui/widgets/WdgBusstopPlatformEditor.cpp \
    src/ui/widgets/WdgColorSelector.cpp \
    src/ui/widgets/WdgFilterBanner.cpp \
    src/ui/widgets/WdgGlobalSearchItem.cpp \
    src/ui/widgets/WdgProjectDataTableQuickSearchLineEdit.cpp \
    src/ui/widgets/WdgTableColumnVisibilitySelector.cpp \
    src/ui/widgets/WdgTimeProfileEditor.cpp \
    src/ui/widgets/WdgTimeProfileEditorTable.cpp \
    main.cpp \
    MainWindow.cpp \
    src/projectdata/ProjectData.cpp \
    src/projectdata/Busstop.cpp \
    src/projectdata/BusstopPlatform.cpp \
    src/projectdata/Line.cpp \
    src/projectdata/LineDirection.cpp \
    src/projectdata/Route.cpp \
    src/projectdata/RouteBusstopItem.cpp \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPage.cpp \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageAppearance.cpp \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageDebug.cpp \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageGeneral.cpp \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageHome.cpp \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageKeyboardShortcuts.cpp \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageLocations.cpp \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPagePlugins.cpp \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageUpdates.cpp \
    src/ui/widgets/WdgAccentColorSelector.cpp \
    src/ui/widgets/WdgColorSchemeSelector.cpp \
    src/ui/widgets/WdgVariantEditor.cpp \
    src/ui/widgets/WdgWelcomeRecentProjectEntry.cpp

HEADERS += \
    ApplicationInterface.h \
    src/commands/CmdBusstops.h \
    src/commands/CmdGeneral.h \
    src/commands/CmdLines.h \
    src/commands/CmdRoutes.h \
    src/ui/dialogs/DlgBusstopEditor.h \
    src/ui/dialogs/DlgConfigEditor.h \
    src/ui/dialogs/DlgGlobalSearch.h \
    src/ui/dialogs/DlgLineEditor.h \
    src/ui/dialogs/DlgPreferences.h \
    src/ui/dialogs/DlgRouteEditor.h \
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
    ItemModels/IconSetsModel.h \
    ItemModels/KeyboardShortcutsModel.h \
    ItemModels/LanguagesModel.h \
    ItemModels/LocalConfigModel.h \
    ItemModels/StylesModel.h \
    MainWindow.h \
    src/projectdata/ProjectData.h \
    src/projectdata/ProjectDataItem.h \
    src/projectdata/ProjectDataItemSignals.h \
    src/projectdata/ProjectDataItemData.h \
    src/projectdata/ProjectDataItemContainer.h \
    src/projectdata/ProjectDataItemList.h \
    src/projectdata/ProjectDataItemSet.h \
    src/projectdata/Busstop.h \
    src/projectdata/BusstopPlatform.h \
    src/projectdata/Line.h \
    src/projectdata/LineDirection.h \
    src/projectdata/Route.h \
    src/projectdata/RouteBusstopItem.h \
    src/projectdata/TimeProfile.h \
    src/projectdata/TimeProfileItem.h \
    ProjectDataModels/BusstopPlatformTableModel.h \
    ProjectDataModels/BusstopTableModel.h \
    ProjectDataModels/BusstopTableProxyModel.h \
    ProjectDataModels/LineDirectionTableModel.h \
    ProjectDataModels/LineSelectionListModel.h \
    ProjectDataModels/LineSelectionListProxyModel.h \
    ProjectDataModels/LineTableModel.h \
    ProjectDataModels/LineTableProxyModel.h \
    ProjectDataModels/OrderedProjectDataRowModel.h \
    ProjectDataModels/RouteBusstopTableModel.h \
    ProjectDataModels/RouteTableModel.h \
    ProjectDataModels/RouteTableProxyModel.h \
    ProjectDataModels/SortFilterProxyModel.h \
    ProjectDataModels/TimeProfileTableModel.h \
    ProjectDataModels/UnorderedProjectDataRowModel.h \
    ProjectDataModels/UnorderedProjectDataRowModelSignals.h \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPage.h \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageAppearance.h \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageDebug.h \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageGeneral.h \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageHome.h \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageKeyboardShortcuts.h \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageLocations.h \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPagePlugins.h \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageUpdates.h \
    src/ui/widgets/Docks/DockAbstract.h \
    src/ui/widgets/Docks/DockBusstops.h \
    src/ui/widgets/Docks/DockLines.h \
    src/ui/widgets/Docks/DockNews.h \
    src/ui/widgets/Docks/DockRoutes.h \
    src/ui/widgets/Docks/DockWelcome.h \
    src/ui/widgets/FilterPopups/WdgBusstopsFilterPopup.h \
    src/ui/widgets/FilterPopups/WdgFilterPopup.h \
    src/ui/widgets/FilterPopups/WdgFilterPopupContent.h \
    src/ui/widgets/FilterPopups/WdgLineFilterPopup.h \
    src/ui/widgets/FilterPopups/WdgRouteFilterPopup.h \
    src/ui/widgets/WdgAccentColorSelector.h \
    src/ui/widgets/WdgBusstopFlagEditor.h \
    src/ui/widgets/WdgBusstopPlatformEditor.h \
    src/ui/widgets/WdgColorSchemeSelector.h \
    src/ui/widgets/WdgColorSelector.h \
    src/ui/widgets/WdgFilterBanner.h \
    src/ui/widgets/WdgGlobalSearchItem.h \
    src/ui/widgets/WdgProjectDataTableQuickSearchLineEdit.h \
    src/ui/widgets/WdgProjectDataTableView.h \
    src/ui/widgets/WdgTableColumnVisibilitySelector.h \
    src/ui/widgets/WdgTimeProfileEditor.h \
    src/ui/widgets/WdgTimeProfileEditorTable.h \
    src/ui/widgets/WdgVariantEditor.h \
    src/ui/widgets/WdgWelcomeRecentProjectEntry.h

FORMS += \
    src/ui/dialogs/DlgBusstopEditor.ui \
    src/ui/dialogs/DlgConfigEditor.ui \
    src/ui/dialogs/DlgGlobalSearch.ui \
    src/ui/dialogs/DlgLineEditor.ui \
    src/ui/dialogs/DlgPreferences.ui \
    src/ui/dialogs/DlgRouteEditor.ui \
    MainWindow.ui \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageAppearance.ui \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageDebug.ui \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageGeneral.ui \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageHome.ui \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageKeyboardShortcuts.ui \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageLocations.ui \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPagePlugins.ui \
    src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageUpdates.ui \
    src/ui/widgets/Docks/DockBusstops.ui \
    src/ui/widgets/Docks/DockLines.ui \
    src/ui/widgets/Docks/DockNews.ui \
    src/ui/widgets/Docks/DockRoutes.ui \
    src/ui/widgets/Docks/DockWelcome.ui \
    src/ui/widgets/FilterPopups/WdgBusstopsFilterPopup.ui \
    src/ui/widgets/FilterPopups/WdgFilterPopup.ui \
    src/ui/widgets/FilterPopups/WdgLineFilterPopup.ui \
    src/ui/widgets/FilterPopups/WdgRouteFilterPopup.ui \
    src/ui/widgets/WdgAccentColorSelector.ui \
    src/ui/widgets/WdgBusstopFlagEditor.ui \
    src/ui/widgets/WdgBusstopPlatformEditor.ui \
    src/ui/widgets/WdgColorSchemeSelector.ui \
    src/ui/widgets/WdgColorSelector.ui \
    src/ui/widgets/WdgFilterBanner.ui \
    src/ui/widgets/WdgGlobalSearchItem.ui \
    src/ui/widgets/WdgTableColumnVisibilitySelector.ui \
    src/ui/widgets/WdgTimeProfileEditor.ui \
    src/ui/widgets/WdgVariantEditor.ui \
    src/ui/widgets/WdgWelcomeRecentProjectEntry.ui

RESOURCES = Resources.qrc

RC_ICONS = Icons/ScheduleMaster_64px.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
