# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

set(CONTROLCENTER_FILES
    index.tmpl
)

target_sources(defaulttemplate
    PRIVATE
        ${CONTROLCENTER_FILES}
)

add_subdirectory(addressbooks)
add_subdirectory(events)
add_subdirectory(settings)
add_subdirectory(users)
add_subdirectory(usersettings)

install(FILES ${CONTROLCENTER_FILES}
        DESTINATION ${GIKWIMI_TEMPLATESDIR}/default/site/controlcenter
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
