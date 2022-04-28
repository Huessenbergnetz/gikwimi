/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "usercommand.h"
#include "useraddcommand.h"

#include <QCoreApplication>
#include <QCommandLineParser>

UserCommand::UserCommand(QObject *parent)
    : Command{parent}
{
    setObjectName(QStringLiteral("user"));
}

UserCommand::~UserCommand() = default;

void UserCommand::init()
{
    new UserAddCommand(this);
}

int UserCommand::exec(QCommandLineParser *parser)
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

QString UserCommand::summary() const
{
    //: CLI command summary
    //% "Manage users"
    return qtTrId("gikctl-command-user-summary");
}

QString UserCommand::description() const
{
    //: CLI command description
    //% "Add, edit, delete and list users."
    return qtTrId("gikctl-command-user-description");
}

#include "moc_usercommand.cpp"
