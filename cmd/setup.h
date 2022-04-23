/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_SETUP_H
#define GIKWIMICTL_SETUP_H

#include "database.h"

class Setup : public Database
{
public:
    explicit Setup(const QString &iniPath, bool quiet = false);
    ~Setup();

    int addUser(const QString &name, const QString &email, const QString &password, const QString &type);
};

#endif // GIKWIMICTL_SETUP_H
