# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

add_custom_target(supplementary
    SOURCES
        conf.ini.in
        gikwimi.service.in
        gikwimi-restart.service
        gikwimi-restart.timer
        gikwimi-apparmor-systemd.conf
        gikwimi-apparmor.in
        gikwimi-apparmor-local
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

install(FILES gikwimi-restart.service gikwimi-restart.timer
        DESTINATION ${SYSTEMD_UNIT_DIR}
)

if (INSTALL_APPARMOR)

    configure_file(gikwimi-apparmor.in apparmor)

    install(FILES gikwimi-apparmor-systemd.conf
            DESTINATION ${SYSTEMD_UNIT_DIR}/gikwimi.service.d
            RENAME apparmor.conf
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/apparmor
            DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/apparmor.d
            RENAME gikwimi
    )

    install(FILES gikwimi-apparmor-local
            DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/apparmor.d/local
            RENAME gikwimi
    )

endif (INSTALL_APPARMOR)
