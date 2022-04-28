/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "databasecommand.h"
#include "databasemigrationcommand.h"
#include "databaserollbackcommand.h"
#include "databaserefreshcommand.h"
#include "databaseresetcommand.h"

#include "../dbmigrations/m0001_create_users_table.h"
#include "../dbmigrations/m0002_createusersettings.h"
#include "../dbmigrations/m0003_createoptionstable.h"
#include "../dbmigrations/m0004_createaddressbookstable.h"

#include <Firfuorida/Migrator>

#include <QCoreApplication>
#include <QCommandLineParser>

DatabaseCommand::DatabaseCommand(QObject *parent)
    : Command(parent)
{
    setObjectName(QStringLiteral("database"));
}

DatabaseCommand::~DatabaseCommand() = default;

void DatabaseCommand::init()
{
    new DatabaseMigrationCommand(this);
    new DatabaseRollbackCommand(this);
    new DatabaseRefreshCommand(this);
    new DatabaseResetCommand(this);
}

int DatabaseCommand::exec(QCommandLineParser *parser)
{
    parser->addPositionalArgument(QStringLiteral("subcommand"), QString());
    parser->parse(QCoreApplication::arguments());

    setGlobalOptions(parser);

    QStringList args = parser->positionalArguments();
    if (args.takeFirst() != objectName()) {
        qFatal("%s", "Invalid command name! Aborting...");
    }

    const QString command = args.empty() ? QLatin1String("") : args.takeFirst().toLower();

    auto com = findChild<Command *>(command, Qt::FindDirectChildrenOnly);

    if (com) {
        com->init();
        return com->exec(parser);
    } else {
        showHelp();
    }

    return 0;
}

QString DatabaseCommand::summary() const
{
    //: CLI command summary
    //% "Manage database migrations"
    return qtTrId("gikctl-command-database-summary");
}

QString DatabaseCommand::description() const
{
    //: CLI command description
    //% "Run, roll back, reset and refresh database migrations. Be careful, these commands can easily lead to data loss! Make sure that you have made a backup of your database beforehand."
    return qtTrId("gikctl-command-database-description");
}

QString DatabaseCommand::dbConName() const
{
    return QStringLiteral("dbmigrations");
}

void DatabaseCommand::initMigrations()
{
    m_migrator = std::make_unique<Firfuorida::Migrator>(dbConName(), QStringLiteral("migrations"));
    new M0001_Create_Users_Table(m_migrator.get());
    new M0002_CreateUserSettings(m_migrator.get());
    new M0003_CreateOptionsTable(m_migrator.get());
    new M0004_CreateAddressBooksTable(m_migrator.get());
}

#include "moc_databasecommand.cpp"
