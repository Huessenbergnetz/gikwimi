# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

add_executable(gikwimictl
    main.cpp)

target_compile_features(gikwimictl PUBLIC cxx_std_14)

target_link_libraries(gikwimictl
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    FirfuoridaQt5
)

target_compile_definitions(gikwimictl
    PRIVATE
        QT_NO_CAST_TO_ASCII
        QT_NO_CAST_FROM_ASCII
        QT_STRICT_ITERATORS
        QT_NO_URL_CAST_FROM_STRING
        QT_NO_CAST_FROM_BYTEARRAY
        QT_USE_QSTRINGBUILDER
        QT_SHA3_KECCAK_COMPAT
        GIKWIMI_VERSION="${PROJECT_VERSION}"
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
