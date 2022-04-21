/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_DBMIGRATIONS_H
#define GIKWIMICTL_DBMIGRATIONS_H

#include "configuration.h"

class DbMigrations : public Configuration
{
public:
    explicit DbMigrations(const QString &distIniPath, const QString &iniPath, bool quiet = false);

    ~DbMigrations();

    int runMigrations();
    int runRollbacks(uint steps = 1);
    int runRollbacks(const QString &steps);
    int runReset();
    int runRefresh(uint steps = 0);
    int runRefresh(const QString &steps);

private:
    int openDb();
};

#endif // GIKWIMICTL_DBMIGRATIONS_H
