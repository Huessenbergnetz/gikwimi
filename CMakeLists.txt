# SPDX-FileCopyrightText: (C) 2021 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

cmake_minimum_required(VERSION 3.12.4 FATAL_ERROR)

project(gikwimi
    VERSION 0.0.1
    DESCRIPTION "Event invitation organizer"
    HOMEPAGE_URL https://github.com/Huessenbergnetz/gikwimi
    LANGUAGES CXX)

if (POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()

find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)
find_package(Cutelyst3Qt5 REQUIRED)

# Auto generate moc files
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GNUInstallDirs)

add_subdirectory(src)
