# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

set(CONTROLCENTER_USERS_FILES
    add.tmpl
    index.tmpl
)

target_sources(defaulttemplate
    PRIVATE
        ${CONTROLCENTER_USERS_FILES}
)

install(FILES ${CONTROLCENTER_USERS_FILES}
        DESTINATION ${GIKWIMI_TEMPLATESDIR}/site/controlcenter/users
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
