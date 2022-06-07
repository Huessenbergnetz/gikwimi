# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

set(CMD_TS_FILES gikwimictl.ts
                 gikwimictl_de.ts
)

qt5_add_translation(CMD_QM_FILES ${CMD_TS_FILES} OPTIONS -idbased)

add_custom_target(gikwimictltranslations ALL
                  DEPENDS ${CMD_QM_FILES}
                  COMMENT "Compiles gikwimictl translation messages"
                  SOURCES ${CMD_TS_FILES}
)

install(FILES ${CMD_QM_FILES}
        DESTINATION ${GIKWIMI_TRANSLATIONSDIR}
        COMPONENT runtime
)

set(APP_TS_FILES gikwimiapp_en.ts
                 gikwimiapp_de.ts
)

qt5_add_translation(APP_QM_FILES ${APP_TS_FILES})

add_custom_target(gikwimiapptranslations ALL
                  DEPENDS ${APP_QM_FILES}
                  COMMENT "Compiles gikwimi app translation messages"
                  SOURCES ${APP_TS_FILES}
)

install(FILES ${APP_QM_FILES}
        DESTINATION ${GIKWIMI_TRANSLATIONSDIR}
        COMPONENT runtime
)
