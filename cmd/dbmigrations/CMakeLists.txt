# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

target_sources(gikwimictl
    PRIVATE
        m0001_create_users_table.cpp
        m0001_create_users_table.h
        m0002_createusersettings.cpp
        m0002_createusersettings.h
        m0003_createoptionstable.cpp
        m0003_createoptionstable.h
        m0004_createaddressbookstable.cpp
        m0004_createaddressbookstable.h
        m0005_createcontactstable.cpp
        m0005_createcontactstable.h
        m0006_createeventstable.cpp
        m0006_createeventstable.h
        m0007_createguestgroupstable.cpp
        m0007_createguestgroupstable.h
        m0008_creategueststable.cpp
        m0008_creategueststable.h
        m0009_createtemplatestable.cpp
        m0009_createtemplatestable.h
)
