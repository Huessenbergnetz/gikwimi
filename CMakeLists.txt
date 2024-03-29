# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(gikwimi
    VERSION 0.0.1
    DESCRIPTION "Event invitation organizer"
    HOMEPAGE_URL https://github.com/Huessenbergnetz/gikwimi
    LANGUAGES CXX)

if (POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()

find_package(Qt5 COMPONENTS Core Network Sql LinguistTools REQUIRED)
find_package(Cutelyst3Qt5 REQUIRED)
find_package(Cutelee6Qt5 5.2.0 REQUIRED)
find_package(FirfuoridaQt5 REQUIRED)
find_package(KF5Contacts REQUIRED)
find_package(SimpleMail2Qt5 REQUIRED)

# Auto generate moc files
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GNUInstallDirs)

set(GIKWIMI_APPDIR ${CMAKE_INSTALL_DATADIR}/gikwimi CACHE PATH "Directory to install gikwimi data files into")
set(GIKWIMI_TEMPLATESDIR ${GIKWIMI_APPDIR}/templates CACHE PATH "Directory to install gikwimi template files into")
set(GIKWIMI_TRANSLATIONSDIR ${GIKWIMI_APPDIR}/translations CACHE PATH "Directory to install gikwimi translation files into")
set(SYSTEMD_UNIT_DIR lib/systemd/system CACHE PATH "Systemd unit directory")
set(GIKWIMI_USER "gikwimi" CACHE STRING "User under which the server will be running")
set(GIKWIMI_GROUP "gikwimi" CACHE STRING "Group under which the server will be running")

option(INSTALL_APPARMOR "Install AppArmor profile and systemd settings" ON)

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
set(GIKWIMI_CONF_GIK_SITENAME "sitename")
set(GIKWIMI_CONF_GIK_SITENAME_DEFVAL "Gikwimi")
set(GIKWIMI_CONF_GIK_USEMEMCACHED "usememcached")
set(GIKWIMI_CONF_GIK_USEMEMCACHED_DEFVAL false)
set(GIKWIMI_CONF_GIK_USEMEMCACHEDSESSION "usememcachedsession")
set(GIKWIMI_CONF_GIK_USEMEMCACHEDSESSION_DEFVAL false)

set(GIKWIMI_CONF_MAIL "email")
SET(GIKWIMI_CONF_MAIL_HOST "host")
SET(GIKWIMI_CONF_MAIL_PORT "port")
SET(GIKWIMI_CONF_MAIL_PORT_DEFVAL 465)
SET(GIKWIMI_CONF_MAIL_FROM_MAIL "frommail")
SET(GIKWIMI_CONF_MAIL_FROM_NAME "fromname")
SET(GIKWIMI_CONF_MAIL_FROM_NAME_DEFVAL "Gikwimi")
SET(GIKWIMI_CONF_MAIL_USER "user")
SET(GIKWIMI_CONF_MAIL_PASS "password")
SET(GIKWIMI_CONF_MAIL_ENC "encryption")
SET(GIKWIMI_CONF_MAIL_ENC_DEFVAL "ssl")



add_subdirectory(app)
add_subdirectory(cmd)
add_subdirectory(supplementary)
add_subdirectory(templates)
add_subdirectory(translations)
