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
    parser->parse(QCoreApplication::arguments());

    if (checkShowHelp(parser)) {
        return 0;
    }

    setGlobalOptions(parser);

    int rc = openDb(dbConName());
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
    return qtTrId("gikctl-cli-command-database-migration-summary");
}

QString DatabaseMigrationCommand::description() const
{
    return qtTrId("gikctl-cli-command-database-migration-description");
}

#include "moc_databasemigrationcommand.cpp"
