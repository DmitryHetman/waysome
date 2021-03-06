#
# Waysome main CMake file
#
cmake_minimum_required(VERSION 2.8)

project(waysome C)


#
# Where to look for modules
#
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

#
# Project options
#
option(HARD_MODE "Enables extra checks for use during development" OFF)

#
# Dependencies
#
find_package(DRM REQUIRED)
find_package(EGL REQUIRED)
find_package(EV REQUIRED)
find_package(GBM REQUIRED)
find_package(GLES2 REQUIRED)
find_package(IcuIO REQUIRED)
find_package(IcuUC REQUIRED)
find_package(LATEX)
find_package(Latex2man)
find_package(Libevdev REQUIRED)
find_package(Libudev)
find_package(MakeInfo)
find_package(PNG REQUIRED 1.6)
find_package(UUID REQUIRED)
find_package(WaylandCursor REQUIRED)
find_package(WaylandEgl REQUIRED)
find_package(WaylandScanner REQUIRED)
find_package(WaylandServer REQUIRED)
find_package(XKBCommon REQUIRED)
find_package(Yajl REQUIRED)
find_package(libreset REQUIRED)

#
# Enable testing
#
enable_testing()

#
# Add subdirectories for the project
#
add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(tests)
