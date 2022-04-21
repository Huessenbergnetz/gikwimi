# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

target_sources(gikwimictl
    PRIVATE
        confignames.h.in
)

configure_file(confignames.h.in confignames.h)

target_include_directories(gikwimictl
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)
