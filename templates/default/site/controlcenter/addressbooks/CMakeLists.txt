# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

set(CONTROLCENTER_ADDRESSBOOKS_FILES
    add.tmpl
    edit.tmpl
    index.tmpl
)

target_sources(defaulttemplate
    PRIVATE
        ${CONTROLCENTER_ADDRESSBOOKS_FILES}
)

add_subdirectory(contacts)

install(FILES ${CONTROLCENTER_ADDRESSBOOKS_FILES}
        DESTINATION ${GIKWIMI_TEMPLATESDIR}/default/site/controlcenter/addressbooks
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
