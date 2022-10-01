add_subdirectory("libraries")

file(GLOB_RECURSE SRC
    "controllers/*.cpp"
    "models/*.cpp"
    "views/*.cpp"
)

add_executable(mytetra "main.cpp" "../resource/mytetra.qrc" "../resource/icons.qrc" ${SRC})
target_include_directories(mytetra PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(mytetra PRIVATE
    actionhelper configeditorhelper csshelper clipboardbranch clipboardrecords debughelper diskhelper downloader
    fixedparameters globalparameters iconselectdialog multilineinputdialog periodiccheckbase periodicsynchro shortcutmanager timermonitoring uniqueidhelper
    qtsingleapplication walkhistory wyedit
    ${QT_LIBS})

set_target_properties(mytetra PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE # rpmlint wants PIE
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/mytetra/" # for search qtSingleApplication
)

target_compile_definitions(mytetra PRIVATE APPLICATION_VERSION="${APP_VERSION}")

# require Qt 6.2
qt_add_translations(mytetra
    TS_FILES "../resource/translations/mytetra_fr.ts" "../resource/translations/mytetra_ru.ts"
    RESOURCE_PREFIX "resource/translations"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(TARGETS mytetra DESTINATION "${CMAKE_INSTALL_BINDIR}")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    install(TARGETS mytetra DESTINATION "${WIN_INSTALL_DIR}")
endif()
