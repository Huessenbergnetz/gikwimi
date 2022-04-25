# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

target_sources(gikwimictl
    PRIVATE
        dbmigrations.cpp
        dbmigrations.h
        m0001_create_users_table.cpp
        m0001_create_users_table.h
        m0002_createusersettings.cpp
        m0002_createusersettings.h
)
