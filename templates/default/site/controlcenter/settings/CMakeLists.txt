# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

set(CONTROLCENTER_SETTINGS_FILES
    index.tmpl
)

target_sources(defaulttemplate
    PRIVATE
        ${CONTROLCENTER_SETTINGS_FILES}
)

install(FILES ${CONTROLCENTER_SETTINGS_FILES}
        DESTINATION ${GIKWIMI_TEMPLATESDIR}/default/site/controlcenter/settings
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
