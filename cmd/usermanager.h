/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_USERMANAGER_H
#define GIKWIMICTL_USERMANAGER_H

#include "database.h"

class UserManager : public Database
{
public:
    explicit UserManager(const QString &iniPath, bool quiet = false);

    ~UserManager();

    int add(const QString &name, const QString &email, const QString &password, const QString &type);
};

#endif // GIKWIMICTL_USERMANAGER_H
