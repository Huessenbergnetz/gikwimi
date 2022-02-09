# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

# Create the application
add_library(gikwimi SHARED
    gikwimi.h
    gikwimi.cpp
    root.h
    root.cpp)

# Link to Cutelyst
target_link_libraries(gikwimi
    Cutelyst::Core
    Qt5::Core
    Qt5::Network
    Qt5::Sql
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
        QT_SHA3_KECCAK_COMPAT
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
