/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_DATABASE_H
#define GIKWIMICTL_DATABASE_H

#include "configuration.h"

class Database : public Configuration
{
public:
    explicit Database(const QString &iniPath, bool quiet = false);
    ~Database();

protected:
    int openDb(const QString &connectionName);
};

#endif // GIKWIMICTL_DATABASE_H
