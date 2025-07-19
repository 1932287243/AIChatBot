cmake_minimum_required(VERSION 3.16)
project(AIChatBot LANGUAGES CXX)

# 设置 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置自动处理 moc / rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# needed for windows
if(WIN32)
    set(INTL_LDFLAGS -lintl)
endif(WIN32)

set(QMarkdownTextEditPath 3rdparty/qmarkdowntextedit)
# QMarkdownTextEdit library
set(RC_FILES
    ${QMarkdownTextEditPath}/media.qrc
)

# Translations arent loaded so don't include them
set(TS_FILES
    ${QMarkdownTextEditPath}/trans/qmarkdowntextedit_de.ts
    ${QMarkdownTextEditPath}/trans/qmarkdowntextedit_ur.ts
    ${QMarkdownTextEditPath}/trans/qmarkdowntextedit_zh_CN.ts
)

set(QMARKDOWNTEXTEDIT_SOURCES
    ${RC_FILES}
    ${QMarkdownTextEditPath}/linenumberarea.h # We need to keep this here, otherwise the build fails
    ${QMarkdownTextEditPath}/markdownhighlighter.cpp
    ${QMarkdownTextEditPath}/qmarkdowntextedit.cpp
    ${QMarkdownTextEditPath}/qownlanguagedata.cpp
    ${QMarkdownTextEditPath}/qownlanguagedata.h
    ${QMarkdownTextEditPath}/qplaintexteditsearchwidget.cpp
    ${QMarkdownTextEditPath}/qplaintexteditsearchwidget.ui
)
set(QMARKDOWNTEXTEDIT_HEADERS
    ${QMarkdownTextEditPath}/markdownhighlighter.h
    ${QMarkdownTextEditPath}/qmarkdowntextedit.h
    ${QMarkdownTextEditPath}/qplaintexteditsearchwidget.h
)

add_library(qmarkdowntextedit ${QMARKDOWNTEXTEDIT_SOURCES})
set_target_properties(qmarkdowntextedit PROPERTIES
    PUBLIC_HEADER "${QMARKDOWNTEXTEDIT_HEADERS}"
)

target_link_libraries(qmarkdowntextedit PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    ${INTL_LDFLAGS}
)

if (Qt${QT_VERSION_MAJOR}Quick_FOUND)
    target_link_libraries(qmarkdowntextedit PUBLIC Qt${QT_VERSION_MAJOR}::Quick)
    # add_executable(QtQuickExample examples/qml/example.cpp examples/qml/ressources.qrc)
    # target_link_libraries(QtQuickExample PRIVATE Qt${QT_VERSION_MAJOR}::Quick qmarkdowntextedit)
endif()

# 查找所有头文件和源文件
FILE(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR}/include/*.h)
FILE(GLOB_RECURSE COMPONENT_HEADER_FILES ${PROJECT_SOURCE_DIR}/include/Component/*.h)
FILE(GLOB_RECURSE DELEGATE_HEADER_FILES ${PROJECT_SOURCE_DIR}/include/StyledItemDelegate/*.h)
FILE(GLOB_RECURSE CHATPAGE_HEADER_FILES ${PROJECT_SOURCE_DIR}/include/ChatPage/*.h)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SOURCE_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/Component COMPONENT_SOURCE_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/StyledItemDelegate DELEGATE_SOURCE_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/ChatPage CHATPAGE_SOURCE_FILES)

# Qt5模块查找
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network)

# 所有项目源码
set(PROJECT_SOURCES
    main.cpp
    Resource/Resource.qrc
    ${app_icon_resource_windows}
    ${HEADER_FILES}
    ${SOURCE_FILES}
    ${COMPONENT_HEADER_FILES}
    ${COMPONENT_SOURCE_FILES}
    ${DELEGATE_HEADER_FILES}
    ${DELEGATE_SOURCE_FILES}
    ${CHATPAGE_HEADER_FILES}
    ${CHATPAGE_SOURCE_FILES}
)

# 添加可执行程序
add_executable(AIChatBot ${PROJECT_SOURCES})

target_include_directories(AIChatBot PRIVATE ${PROJECT_SOURCE_DIR}/include
                                             ${PROJECT_SOURCE_DIR}/3rdparty/qmarkdowntextedit)

# 设置输出文件名
set_target_properties(AIChatBot PROPERTIES
    OUTPUT_NAME "AIChatBot"
)

# 链接 Qt5 库
target_link_libraries(AIChatBot
    PUBLIC
        Qt5::Core
        Qt5::Gui
        # Qt5::Widgets
        Qt5::Network
        qmarkdowntextedit
)
