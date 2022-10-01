file(GLOB_RECURSE WYEDIT_SRC "*.cpp")

add_library(wyedit OBJECT "${WYEDIT_SRC}")
target_link_libraries(wyedit PRIVATE Qt6::Core Qt6::Widgets Qt6::Xml Qt6::Network debughelper downloader diskhelper multilineinputdialog)
