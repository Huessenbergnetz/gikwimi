# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

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
find_package(Cutelee6Qt5 5.2.0 REQUIRED)
find_package(FirfuoridaQt5 REQUIRED)

# Auto generate moc files
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GNUInstallDirs)

set(GIKWIMI_DISTCONFDIR "/usr/etc" CACHE PATH "Directory containing distribution provided read only configuration files")

set(GIKWIMI_CONF_DB "database")
set(GIKWIMI_CONF_DB_TYPE "type")
set(GIKWIMI_CONF_DB_TYPE_DEFVAL "QMYSQL")
set(GIKWIMI_CONF_DB_HOST "host")
set(GIKWIMI_CONF_DB_HOST_DEFVAL "localhost")
set(GIKWIMI_CONF_DB_USER "user")
set(GIKWIMI_CONF_DB_USER_DEFVAL "gikwimi")
set(GIKWIMI_CONF_DB_PASS "password")
set(GIKWIMI_CONF_DB_NAME "name")
set(GIKWIMI_CONF_DB_NAME_DEFVAL "gikwimidb")
set(GIKWIMI_CONF_DB_PORT "port")
set(GIKWIMI_CONF_DB_PORT_DEFVAL 3306)

set(GIKWIMI_CONF_GIK "gikwimi")
set(GIKWIMI_CONF_GIK_TEMPLATE "template")
set(GIKWIMI_CONF_GIK_TEMPLATE_DEFVAL "default")


add_subdirectory(app)
add_subdirectory(cmd)
add_subdirectory(supplementary)
