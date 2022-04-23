# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

add_library(gikwimi SHARED)

target_sources(gikwimi
    PRIVATE
        gikwimi.h
        gikwimi.cpp
        root.h
        root.cpp
        login.h
        login.cpp
        logging.h
        gikwimiconfig.h
        gikwimiconfig.cpp
        userauthstoresql.h
        userauthstoresql.cpp
        controlcenter.h
        controlcenter.cpp
)

add_subdirectory(../common common)

target_link_libraries(gikwimi
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    Cutelyst::Core
    Cutelyst::Session
    Cutelyst::Authentication
    Cutelyst::StatusMessage
    Cutelyst::View::Cutelee
    Cutelyst::StaticSimple
    Cutelyst::Utils::Validator
    Cutelyst::Utils::Sql
    Cutelyst::Utils::Pagination
    Cutelyst::Utils::LangSelect
    Cutelyst::Memcached
    Cutelyst::MemcachedSessionStore
    Cutelyst::CSRFProtection
    Cutelee::Templates
)

target_compile_features(gikwimi PUBLIC cxx_std_14)

target_compile_definitions(gikwimi
    PRIVATE
        QT_NO_CAST_TO_ASCII
        QT_NO_CAST_FROM_ASCII
        QT_STRICT_ITERATORS
        QT_NO_URL_CAST_FROM_STRING
        QT_NO_CAST_FROM_BYTEARRAY
        QT_USE_QSTRINGBUILDER
        GIKWIMI_VERSION="${PROJECT_VERSION}"
)

target_compile_options(gikwimi
    PRIVATE
        -Wall
        -Wcast-align
        -Wno-uninitialized
        -Wempty-body
        -Wformat-security
        -Wformat
        -Winit-self
)

install(TARGETS gikwimi DESTINATION ${CMAKE_INSTALL_LIBDIR}/cutelyst${Cutelyst3Qt5_VERSION_MAJOR}-qt5-apps)
