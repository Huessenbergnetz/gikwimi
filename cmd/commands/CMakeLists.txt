# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

target_sources(gikwimictl
    PRIVATE
        addressbookcommand.cpp
        addressbookcommand.h
        addressbookaddcommand.cpp
        addressbookaddcommand.h
        contactcommand.cpp
        contactcommand.h
        contactimportcommand.cpp
        contactimportcommand.h
        databasecommand.cpp
        databasecommand.h
        databasemigrationcommand.cpp
        databasemigrationcommand.h
        databaserefreshcommand.cpp
        databaserefreshcommand.h
        databaseresetcommand.cpp
        databaseresetcommand.h
        databaserollbackcommand.cpp
        databaserollbackcommand.h
        eventcommand.cpp
        eventcommand.h
        usercommand.cpp
        usercommand.h
        useraddcommand.cpp
        useraddcommand.h
)
