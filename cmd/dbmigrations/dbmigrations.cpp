/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "confignames.h"
#include "dbmigrations.h"
#include "m0001_create_users_table.h"

#include <Firfuorida/Migrator>

#include <QSqlDatabase>

#define DBCONNAME "dbmigrations"

DbMigrations::DbMigrations(const QString &iniPath, bool quiet)
    : Database(iniPath, quiet)
{

}

DbMigrations::~DbMigrations() = default;

void DbMigrations::initMigrations()
{
    m_migrator = std::make_unique<Firfuorida::Migrator>(QStringLiteral(DBCONNAME), QStringLiteral("migrations"));
    new M0001_Create_Users_Table(m_migrator.get());
}

int DbMigrations::runMigrations()
{
    int rc = openDb(QStringLiteral(DBCONNAME));
    if (rc != 0) {
        return rc;
    }

    initMigrations();

    if (!m_migrator->migrate()) {
        rc = dbError(m_migrator->lastError().text());
    }

    return rc;
}

int DbMigrations::runRollbacks(uint steps)
{
    int rc = openDb(QStringLiteral(DBCONNAME));
    if (rc != 0) {
        return rc;
    }

    initMigrations();

    if (!m_migrator->rollback(steps)) {
        rc = dbError(m_migrator->lastError().text());
    }

    return rc;
}

int DbMigrations::runRollbacks(const QString &steps)
{
    bool ok = false;
    const uint _steps = steps.toUInt(&ok);
    if (!ok) {
        //: Error message, %1 will be replaced by the invalid value string
        //% "“%1” is not a valid unsigned integer value."
        return inputError(qtTrId("gikctl-error-input-invalid-uint"));
    }

    return runRollbacks(_steps);
}

int DbMigrations::runReset()
{
    int rc = openDb(QStringLiteral(DBCONNAME));
    if (rc != 0) {
        return rc;
    }

    initMigrations();

    if (!m_migrator->reset()) {
        rc = dbError(m_migrator->lastError().text());
    }

    return rc;
}

int DbMigrations::runRefresh(uint steps)
{
    int rc = openDb(QStringLiteral(DBCONNAME));
    if (rc != 0) {
        return rc;
    }

    initMigrations();

    if (!m_migrator->refresh(steps)) {
        rc = dbError(m_migrator->lastError().text());
    }

    return rc;
}

int DbMigrations::runRefresh(const QString &steps)
{
    bool ok = false;
    const uint _steps = steps.toUInt(&ok);
    if (!ok) {
        //: Error message, %1 will be replaced by the invalid value string
        //% "“%1” is not a valid unsigned integer value."
        return inputError(qtTrId("gikctl-error-input-invalid-uint"));
    }

    return runRefresh(_steps);
}
