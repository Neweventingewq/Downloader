cmake_minimum_required(VERSION 3.21)
project(VolchayDownloader VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ----------------------------------------------------------------------------
# Qt 6 dependencies
# ----------------------------------------------------------------------------
find_package(Qt6 6.2 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Network
    WebEngineQuick
    WebEngineCore
)

# ----------------------------------------------------------------------------
# Sources
# ----------------------------------------------------------------------------
set(VD_SOURCES
    src/main.cpp
    src/CookiesPreparer.cpp
    src/CookieJar.cpp
    src/DownloadManager.cpp
    src/DownloadQueueModel.cpp
    src/DownloadJob.cpp
    src/Settings.cpp
    src/Theme.cpp
    src/ToolsLocator.cpp
    src/AppLocale.cpp
)

set(VD_HEADERS
    src/CookiesPreparer.h
    src/CookieJar.h
    src/DownloadManager.h
    src/DownloadQueueModel.h
    src/DownloadJob.h
    src/Settings.h
    src/Theme.h
    src/ToolsLocator.h
    src/AppLocale.h
)

set(VD_WIN_RESOURCE "")
if(WIN32)
    set(VD_WIN_RESOURCE resources/app.rc)
endif()

qt_add_executable(VolchayDownloader WIN32 MACOSX_BUNDLE
    ${VD_SOURCES}
    ${VD_HEADERS}
    resources/qml.qrc
    ${VD_WIN_RESOURCE}
)

target_include_directories(VolchayDownloader PRIVATE src)

target_link_libraries(VolchayDownloader PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
    Qt6::WebEngineQuick
    Qt6::WebEngineCore
)

set_target_properties(VolchayDownloader PROPERTIES
    OUTPUT_NAME "Volchay-Downloader"
)

if(WIN32)
    set_target_properties(VolchayDownloader PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()

install(TARGETS VolchayDownloader
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION bin
)
