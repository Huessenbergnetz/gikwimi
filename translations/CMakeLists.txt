# SPDX-FileCopyrightText: (C) 2022, 2025 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

set(CMD_TS_FILES gikwimictl.ts
                 gikwimictl_de.ts
)

set(APP_TS_FILES gikwimiapp_en.ts
                 gikwimiapp_de.ts
)

if (Qt6_VERSION VERSION_GREATER_EQUAL 6.7.0)
        qt_add_lupdate(TS_FILES ${CMD_TS_FILES}
                       SOURCE_TARGETS gikwimictl
                       OPTIONS -no-obsolete -locations none -source-language en)

        qt_add_lrelease(TS_FILES ${CMD_TS_FILES}
                        QM_FILES_OUTPUT_VARIABLE CMD_QM_FILES
                        OPTIONS -idbased)


        qt_add_lupdate(TS_FILES ${APP_TS_FILES}
                       SOURCE_TARGETS gikwimi
                       OPTIONS -no-obsolete -locations none -source-language en)

        qt_add_lrelease(TS_FILES ${APP_TS_FILES}
                        QM_FILES_OUTPUT_VARIABLE APP_QM_FILES)
else (Qt6_VERSION VERSION_GREATER_EQUAL 6.7.0)
        qt_add_lupdate(gikwimictl
                       TS_FILES ${CMD_TS_FILES}
                       OPTIONS -no-obsolete -locations none -source-language en)

        qt_add_lrelease(gikwimictl
                        TS_FILES ${CMD_TS_FILES}
                        QM_FILES_OUTPUT_VARIABLE CMD_QM_FILES
                        OPTIONS -idbased)


        qt_add_lupdate(gikwimi
                       TS_FILES ${APP_TS_FILES}
                       OPTIONS -no-obsolete -locations none -source-language en)

        qt_add_lrelease(gikwimi
                        TS_FILES ${APP_TS_FILES}
                        QM_FILES_OUTPUT_VARIABLE APP_QM_FILES
                        OPTIONS -idbased)
endif (Qt6_VERSION VERSION_GREATER_EQUAL 6.7.0)

install(FILES ${CMD_QML_FILES}
        DESTINATION ${GIKWIMI_TRANSLATIONSDIR})

install(FILES ${APP_QML_FILES}
        DESTINATION ${GIKWIMI_TRANSLATIONSDIR})
