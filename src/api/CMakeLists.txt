add_library(ScheduleMasterInterface SHARED
    GlobalConfigItem.cpp GlobalConfigItem.h
    helpers.cpp helpers.h
    IActionService.cpp IActionService.h
    IAppearanceService.cpp IAppearanceService.h
    ICrashDetector.h
    IFolderLocationService.cpp IFolderLocationService.h
    IIconService.cpp IIconService.h
    ILanguageService.cpp ILanguageService.h
    ILastUsedFilesService.h
    ILogger.h
    ISettingsService.cpp ISettingsService.h
    ScheduleMaster_global.h
    ScheduleMaster.h
    IDockService.h
    IDockService.cpp
    IMainWindow.h
    IWorkspaceService.h IWorkspaceService.cpp
    ScheduleMaster.cpp
)

target_compile_definitions(ScheduleMasterInterface PRIVATE SCHEDULEMASTER_API)
target_compile_definitions(ScheduleMasterInterface PRIVATE QT_DISABLE_DEPRECATED_BEFORE=0x060000)

target_link_libraries(ScheduleMasterInterface PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_include_directories(ScheduleMasterInterface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
