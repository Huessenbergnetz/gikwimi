/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "setup.h"
#include "dbmigrations/m0001_create_users_table.h"

#include <Firfuorida/Migrator>

#include <memory>

#define DBCONNAME "dbsetup"

Setup::Setup(const QString &iniPath, bool quiet)
    : Database(iniPath, quiet)
{

}

Setup::~Setup() = default;

int Setup::run()
{
    int rc = openDb(QStringLiteral(DBCONNAME));
    if (rc != 0) {
        return rc;
    }

    auto migrator = std::make_unique<Firfuorida::Migrator>(QStringLiteral(DBCONNAME), QStringLiteral("migrations"));
    new M0001_Create_Users_Table(migrator.get());

    if (!migrator->migrate()) {
        rc = dbError(migrator->lastError().text());
    }

    return rc;
}
