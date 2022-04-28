# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

add_executable(gikwimictl)

target_sources(gikwimictl
    PRIVATE
        main.cpp
        cli.cpp
        cli.h
        configuration.cpp
        configuration.h
        controller.cpp
        controller.h
        command.cpp
        command.h
        database.cpp
        database.h
)

target_compile_features(gikwimictl PUBLIC cxx_std_14)

add_subdirectory(dbmigrations)
add_subdirectory(../common common)
add_subdirectory(commands)

target_link_libraries(gikwimictl
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    Cutelyst::Authentication
    Cutelyst::Utils::Validator
    KF5::Contacts
    FirfuoridaQt5::Core
    gikwimi
)

GNUInstallDirs_get_absolute_install_dir(GIKWIMI_FULL_TRANSLATIONSDIR GIKWIMI_TRANSLATIONSDIR)

target_compile_definitions(gikwimictl
    PRIVATE
        QT_NO_CAST_TO_ASCII
        QT_NO_CAST_FROM_ASCII
        QT_STRICT_ITERATORS
        QT_NO_URL_CAST_FROM_STRING
        QT_NO_CAST_FROM_BYTEARRAY
        QT_USE_QSTRINGBUILDER
        GIKWIMI_VERSION="${PROJECT_VERSION}"
        GIKWIMI_CONFFILE="${CMAKE_INSTALL_FULL_SYSCONFDIR}/gikwimi.ini"
        $<BUILD_INTERFACE:GIKWIMI_TRANSLATIONSDIR="${CMAKE_BINARY_DIR}/translations">
        $<INSTALL_INTERFACE:GIKWIMI_TRANSLATIONSDIR="${GIKWIMI_FULL_TRANSLATIONSDIR}">
)

target_compile_options(gikwimictl
    PRIVATE
        -Wall
        -Wcast-align
        -Wno-uninitialized
        -Wempty-body
        -Wformat-security
        -Wformat
        -Winit-self
)

install(TARGETS gikwimictl DESTINATION ${CMAKE_INSTALL_BINDIR})
