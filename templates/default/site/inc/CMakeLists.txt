# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

set(INC_FILES
    editGuestModal.tmpl
    inputWithLabel.tmpl
    modal.tmpl
    page_header.tmpl
    rangeInputWithLabel.tmpl
    textInputWithLabel.tmpl
)

target_sources(defaulttemplate
    PRIVATE
        ${INC_FILES}
)

install(FILES ${INC_FILES}
        DESTINATION ${GIKWIMI_TEMPLATESDIR}/default/site/inc
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
