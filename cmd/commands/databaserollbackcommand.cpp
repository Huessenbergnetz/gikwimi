/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "databaserollbackcommand.h"

#include <Firfuorida/Migrator>

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

DatabaseRollbackCommand::DatabaseRollbackCommand(QObject *parent)
    : DatabaseCommand{parent}
{
    setObjectName(QStringLiteral("rollback"));
}

DatabaseRollbackCommand::~DatabaseRollbackCommand() = default;

void DatabaseRollbackCommand::init()
{
    m_cliOptions.append(QCommandLineOption({QStringLiteral("s"), QStringLiteral("steps")},
                                           qtTrId("gikctl-opt-database-rollback-steps-desc"),
                                           qtTrId("gikctl-opt-database-rollback-steps-value"),
                                           QStringLiteral("1")));
}

int DatabaseRollbackCommand::exec(QCommandLineParser *parser)
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
        //% "Can not perform database migration rollback. “%1“ is not a valid integer value for steps."
        rc = inputError(qtTrId("gikctl-error-invalid-rollback-steps-integer").arg(stepsString));
        return rc;
    }

    setGlobalOptions(parser);

    rc = openDb(dbConName());
    if (rc != 0) {
        return rc;
    }

    initMigrations();

    if (!m_migrator->rollback(steps)) {
        return dbError(m_migrator->lastError().text());
    }

    return rc;
}

QString DatabaseRollbackCommand::summary() const
{
    return qtTrId("gikctl-cli-command-database-rollback-summary");
}

QString DatabaseRollbackCommand::description() const
{
    return qtTrId("gikctl-cli-command-database-rollback-description");
}

#include "moc_databaserollbackcommand.cpp"
