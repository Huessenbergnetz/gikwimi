# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

if (TARGET gikwimictl)
    target_sources(gikwimictl
        PRIVATE
            confignames.h.in
            global.h
    )
endif (TARGET gikwimictl)

if (TARGET gikwimi)
    target_sources(gikwimi
        PRIVATE
            confignames.h.in
            global.h
    )
endif (TARGET gikwimi)

configure_file(confignames.h.in confignames.h)

if (TARGET gikwimictl)
    target_include_directories(gikwimictl
        PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif (TARGET gikwimictl)

if (TARGET gikwimi)
    target_include_directories(gikwimi
        PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif (TARGET gikwimi)
