# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

set(CONTROLCENTER_EVENTS_TEMPLATES_FILES
    add.tmpl
    edit.tmpl
    index.tmpl
)

target_sources(defaulttemplate
    PRIVATE
        ${CONTROLCENTER_EVENTS_TEMPLATES_FILES}
)

install(FILES ${CONTROLCENTER_EVENTS_TEMPLATES_FILES}
        DESTINATION ${GIKWIMI_TEMPLATESDIR}/default/site/controlcenter/events/templates
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
