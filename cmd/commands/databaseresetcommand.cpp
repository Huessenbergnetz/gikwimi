/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "databaseresetcommand.h"

#include <Firfuorida/Migrator>

#include <QCommandLineParser>

DatabaseResetCommand::DatabaseResetCommand(QObject *parent)
    : DatabaseCommand{parent}
{
    setObjectName(QStringLiteral("reset"));
}

DatabaseResetCommand::~DatabaseResetCommand() = default;

void DatabaseResetCommand::init()
{

}

int DatabaseResetCommand::exec(QCommandLineParser *parser)
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

    if (!m_migrator->reset()) {
        rc = dbError(m_migrator->lastError().text());
    }

    return rc;
}

QString DatabaseResetCommand::summary() const
{
    //: CLI command summary
    //% "Reset database migrations"
    return qtTrId("gikctl-command-database-reset-summary");
}

QString DatabaseResetCommand::description() const
{
    //: CLI command description
    //% "Resets all database migrations. A reset will roll back all migrations. Note that this will result in data loss. This command is for development purposes only."
    return qtTrId("gikctl-command-database-reset-description");
}

#include "moc_databaseresetcommand.cpp"
