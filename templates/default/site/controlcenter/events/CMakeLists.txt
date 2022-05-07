# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

target_sources(defaulttemplate
    PRIVATE
        add.tmpl
        edit.tmpl
        index.tmpl
)

add_subdirectory(guests)
