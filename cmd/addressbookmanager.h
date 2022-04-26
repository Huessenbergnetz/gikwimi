/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_ADDRESSBOOKMANAGER_H
#define GIKWIMICTL_ADDRESSBOOKMANAGER_H

#include "database.h"

class AddressBookManager : public Database
{
public:
    explicit AddressBookManager(const QString &iniPath, bool quiet = false);
    ~AddressBookManager();

    int add(const QString &user, const QString &name, const QString &type, const QString &data);
};

#endif // GIKWIMICTL_ADDRESSBOOKMANAGER_H
