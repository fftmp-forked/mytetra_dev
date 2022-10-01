add_subdirectory("qtSingleApplication")
add_subdirectory("helpers")
add_subdirectory("wyedit")

# @TODO: some of these libraries does not use Qt6::Widgets Qt6::Xml directly,
# but depend on class Record, which expose such dependency. Looks like abstraction leak, need to fix.

add_library(shortcutmanager OBJECT "ShortcutManager/ShortcutManager.cpp")
target_include_directories(shortcutmanager PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(shortcutmanager PRIVATE Qt6::Core Qt6::Widgets debughelper)

add_library(clipboardbranch OBJECT "ClipboardBranch.cpp")
target_include_directories(clipboardbranch PRIVATE Qt6::Core Qt6::Widgets Qt6::Xml)
target_link_libraries(clipboardbranch PRIVATE Qt6::Core Qt6::Widgets Qt6::Xml)

add_library(clipboardrecords OBJECT "ClipboardRecords.cpp")
target_include_directories(clipboardrecords PRIVATE Qt6::Core Qt6::Widgets Qt6::Xml)
target_link_libraries(clipboardrecords PRIVATE Qt6::Core Qt6::Widgets Qt6::Xml)

add_library(downloader OBJECT "Downloader.cpp")
target_link_libraries(downloader PRIVATE Qt6::Core Qt6::Network Qt6::Widgets)

add_library(fixedparameters OBJECT "FixedParameters.cpp")
target_link_libraries(fixedparameters PRIVATE Qt6::Core)

add_library(globalparameters OBJECT "GlobalParameters.cpp")
target_include_directories(globalparameters PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(globalparameters PRIVATE Qt6::Core Qt6::Widgets)

add_library(iconselectdialog OBJECT "IconSelectDialog.cpp")
target_link_libraries(iconselectdialog PRIVATE Qt6::Core Qt6::Widgets)

add_library(periodiccheckbase OBJECT "PeriodicCheckBase.cpp")
target_include_directories(periodiccheckbase PRIVATE Qt6::Core Qt6::Widgets Qt6::Xml)
target_link_libraries(periodiccheckbase PRIVATE Qt6::Core Qt6::Widgets Qt6::Xml)

add_library(periodicsynchro OBJECT "PeriodicSynchro.cpp")
target_include_directories(periodicsynchro PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(periodicsynchro PRIVATE Qt6::Core Qt6::Widgets)

add_library(timermonitoring OBJECT "TimerMonitoring.cpp")
target_link_libraries(timermonitoring PRIVATE Qt6::Core Qt6::Widgets Qt6::Xml)
target_include_directories(timermonitoring PRIVATE Qt6::Core Qt6::Widgets Qt6::Xml)

add_library(walkhistory OBJECT "WalkHistory.cpp")
target_link_libraries(walkhistory PRIVATE Qt6::Core)
target_include_directories(walkhistory PRIVATE Qt6::Core)
