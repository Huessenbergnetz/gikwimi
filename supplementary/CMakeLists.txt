# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

add_custom_target(supplementary
    SOURCES
        distconf.ini.in
        conf.ini.in
)

configure_file(distconf.ini.in distconf.ini)
configure_file(conf.ini.in conf.ini)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/distconf.ini
        DESTINATION ${GIKWIMI_DISTCONFDIR}
        RENAME gikwimi.ini
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/conf.ini
        DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}
        RENAME gikwimi.ini
)
