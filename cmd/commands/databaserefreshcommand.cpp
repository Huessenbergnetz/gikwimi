/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "databaserefreshcommand.h"

#include <Firfuorida/Migrator>

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

DatabaseRefreshCommand::DatabaseRefreshCommand(QObject *parent)
    : DatabaseCommand{parent}
{
    setObjectName(QStringLiteral("refresh"));
}

DatabaseRefreshCommand::~DatabaseRefreshCommand() = default;

void DatabaseRefreshCommand::init()
{
    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("s"), QStringLiteral("steps")}),
                                           qtTrId("gikctl-opt-database-refresh-steps-desc"),
                                           qtTrId("gikctl-opt-database-refresh-steps-value"),
                                           QStringLiteral("0")));
}

int DatabaseRefreshCommand::exec(QCommandLineParser *parser)
{
    int rc = 0;

    parser->addOptions(m_cliOptions);
    parser->parse(QCoreApplication::arguments());

    if (checkShowHelp(parser)) {
        return 0;
    }

    const QString stepsString = parser->value(QStringLiteral("steps"));
    bool ok = false;
    uint steps = stepsString.toUInt(&ok);

    if (!ok) {
        //% "Can not perform database migration refresh. “%1“ is not a valid integer value for steps."
        rc = inputError(qtTrId("gikctl-error-invalid-refresh-steps-integer").arg(stepsString));
        return rc;
    }

    setGlobalOptions(parser);

    rc = openDb(dbConName());
    if (rc != 0) {
        return rc;
    }

    initMigrations();

    if (!m_migrator->refresh(steps)) {
        return dbError(m_migrator->lastError().text());
    }

    return rc;
}

QString DatabaseRefreshCommand::summary() const
{
    return qtTrId("gikctl-cli-command-database-refresh-summary");
}

QString DatabaseRefreshCommand::description() const
{
    return qtTrId("gikctl-cli-command-database-refresh-description");
}

#include "moc_databaserefreshcommand.cpp"
