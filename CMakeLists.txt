cmake_minimum_required(VERSION 3.16)

project(hdfguide VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(HDF5 COMPONENTS CXX HL REQUIRED)

add_subdirectory(lib)

include_directories(src)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.ui
    src/GroupBrowserWidget.cpp
    src/DatasetPreviewWidget.cpp
    src/AttributeTableWidget.cpp
    src/TerminalWidget.cpp
    src/TerminalWidget.ui
    src/AnalysisViewWidget.cpp
    src/HDF5Model.cpp
    src/DatasetSearchWidget.cpp

    src/fa/QtAwesome/QtAwesome.cpp
    src/fa/QtAwesome/QtAwesomeAnim.cpp
    src/fa/QtAwesome/QtAwesome.qrc

    src/AboutDialog.cpp
    src/AboutDialog.ui

    qrc/main.qrc
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    qtadvanceddocking
    HighFive
    HDF5::HDF5
)
