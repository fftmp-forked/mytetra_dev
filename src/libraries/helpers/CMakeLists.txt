add_library(actionhelper OBJECT "ActionHelper.cpp")
target_link_libraries(actionhelper PRIVATE Qt6::Widgets)

add_library(csshelper OBJECT "CssHelper.cpp")
target_link_libraries(csshelper PRIVATE Qt6::Core Qt6::Widgets)

add_library(multilineinputdialog OBJECT "MultiLineInputDialog.cpp")
target_link_libraries(multilineinputdialog PRIVATE Qt6::Core Qt6::Widgets)
target_include_directories(multilineinputdialog PRIVATE "${CMAKE_SOURCE_DIR}/src")

add_library(configeditorhelper OBJECT "ConfigEditorHelper.cpp")
target_link_libraries(configeditorhelper PRIVATE Qt6::Core Qt6::Widgets multilineinputdialog)
target_include_directories(configeditorhelper PRIVATE "${CMAKE_SOURCE_DIR}/src")

add_library(debughelper OBJECT "DebugHelper.cpp")
target_link_libraries(debughelper PRIVATE Qt6::Core Qt6::Widgets)
target_include_directories(debughelper PRIVATE "${CMAKE_SOURCE_DIR}/src")

add_library(diskhelper OBJECT "DiskHelper.cpp")
target_link_libraries(diskhelper PRIVATE Qt6::Core uniqueidhelper)

add_library(uniqueidhelper OBJECT "UniqueIdHelper.cpp")
target_link_libraries(uniqueidhelper PRIVATE Qt6::Core)

