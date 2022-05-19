# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

set(SITE_FILES
    base.tmpl
    login.tmpl
    wrapper.tmpl
    invitation.tmpl
)

target_sources(defaulttemplate
    PRIVATE
        ${SITE_FILES}
)

add_subdirectory(controlcenter)
add_subdirectory(inc)

install(FILES ${SITE_FILES}
        DESTINATION ${GIKWIMI_TEMPLATESDIR}/default/site
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
