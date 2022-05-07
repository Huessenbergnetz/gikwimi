# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

target_sources(defaulttemplate
    PRIVATE
        index.tmpl
)

add_subdirectory(addressbooks)
add_subdirectory(events)
add_subdirectory(settings)
add_subdirectory(users)
add_subdirectory(usersettings)
