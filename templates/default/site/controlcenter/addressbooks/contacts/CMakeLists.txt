# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

set(CONTROLCENTER_ADDRESSBOOKS_CONTACTS_FILES
    add.tmpl
    edit.tmpl
    index.tmpl
)

target_sources(defaulttemplate
    PRIVATE
        ${CONTROLCENTER_ADDRESSBOOKS_CONTACTS_FILES}
)

install(FILES ${CONTROLCENTER_ADDRESSBOOKS_CONTACTS_FILES}
        DESTINATION ${GIKWIMI_TEMPLATESDIR}/site/controlcenter/addressbooks/contacts
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
