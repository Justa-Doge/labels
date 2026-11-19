cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(Labels VERSION 0.2.0)

option(GEODE_DONT_INSTALL_MODS "Do not install the mod into a game during development builds" ON)

add_library(Labels SHARED
    src/main.cpp
    src/LabelStore.cpp
    src/LabelHud.cpp
    src/LabelEditor.cpp
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK. Set GEODE_SDK to your Geode SDK folder.")
endif()

add_subdirectory($ENV{GEODE_SDK} build/geode)

CPMAddPackage(
    NAME rift
    GIT_REPOSITORY https://git.eclipse.menu/EclipseMenu/RIFT
    GIT_TAG e065e9a439ed26e05d34ffc3a5827eb5406c8a94
)

target_link_libraries(Labels rift)
setup_geode_mod(Labels)


