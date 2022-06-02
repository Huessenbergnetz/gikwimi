# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

set(CONTROLCENTER_EVENTS_GUESTS_FILES
    add.tmpl
    addtogroup.tmpl
    edit.tmpl
    editGuestModel.tmpl
    index.tmpl
    invite.tmpl
)

target_sources(defaulttemplate
    PRIVATE
        ${CONTROLCENTER_EVENTS_GUESTS_FILES}
)

install(FILES ${CONTROLCENTER_EVENTS_GUESTS_FILES}
        DESTINATION ${GIKWIMI_TEMPLATESDIR}/default/site/controlcenter/events/guests
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
