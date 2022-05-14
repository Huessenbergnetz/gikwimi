# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

add_custom_target(supplementary
    SOURCES
        conf.ini.in
        gikwimi.service.in
)

configure_file(conf.ini.in conf.ini)
configure_file(gikwimi.service.in gikwimi.service @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/conf.ini
        DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}
        RENAME gikwimi.ini
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gikwimi.service
        DESTINATION ${SYSTEMD_UNIT_DIR}
)
