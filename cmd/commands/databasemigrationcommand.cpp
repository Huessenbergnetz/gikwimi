/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "databasemigrationcommand.h"

#include <Firfuorida/Migrator>

#include <QCoreApplication>
#include <QCommandLineParser>

DatabaseMigrationCommand::DatabaseMigrationCommand(QObject *parent)
    : DatabaseCommand{parent}
{
    setObjectName(QStringLiteral("migrate"));
}

DatabaseMigrationCommand::~DatabaseMigrationCommand() = default;

void DatabaseMigrationCommand::init()
{

}

int DatabaseMigrationCommand::exec(QCommandLineParser *parser)
{
    int rc = 0;

    parser->parse(QCoreApplication::arguments());

    if (checkShowHelp(parser)) {
        return rc;
    }

    setGlobalOptions(parser);

    rc = openDb(dbConName());
    if (rc != 0) {
        return rc;
    }

    initMigrations();

    if (!m_migrator->migrate()) {
        rc = dbError(m_migrator->lastError().text());
    }

    return rc;
}

QString DatabaseMigrationCommand::summary() const
{
    //: CLI command summary
    //% "Run database migrations"
    return qtTrId("gikctl-command-database-migration-summary");
}

QString DatabaseMigrationCommand::description() const
{
    //: CLI command description
    //% "Performs all database migrations that have not already be performed before. You should run this command after upgrading Gikwimi."
    return qtTrId("gikctl-command-database-migration-description");
}

#include "moc_databasemigrationcommand.cpp"
