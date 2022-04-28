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

    if (!m_migrator->reset()) {
        rc = dbError(m_migrator->lastError().text());
    }

    return rc;
}

QString DatabaseResetCommand::summary() const
{
    return qtTrId("gikctl-cli-command-database-reset-summary");
}

QString DatabaseResetCommand::description() const
{
    return qtTrId("gikctl-cli-command-database-reset-description");
}

#include "moc_databaseresetcommand.cpp"
