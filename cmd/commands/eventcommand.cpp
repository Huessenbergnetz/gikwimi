/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "eventcommand.h"
#include "eventaddcommand.h"

#include <QCoreApplication>
#include <QCommandLineParser>

EventCommand::EventCommand(QObject *parent)
    : Command{parent}
{
    setObjectName(QStringLiteral("event"));
}

EventCommand::~EventCommand() = default;

void EventCommand::init()
{
    new EventAddCommand(this);
}

int EventCommand::exec(QCommandLineParser *parser)
{
    parser->addPositionalArgument(QStringLiteral("subcommand"), QString());
    parser->parse(QCoreApplication::arguments());

    setGlobalOptions(parser);

    QStringList args = parser->positionalArguments();
    if (args.takeFirst().toLower() != objectName()) {
        qFatal("%s", "Invalid command name! Aborting...");
    }

    const QString command = args.empty() ? QLatin1String("") : args.takeFirst().toLower();

    return runSubCommand(command, parser);
}

QString EventCommand::summary() const
{
    //: CLI command summary
    //% "Manage events"
    return qtTrId("gikctl-command-event-summary");
}

QString EventCommand::description() const
{
    //: CLI command description
    //% "Add, edit, delete and list events."
    return qtTrId("gikctl-command-event-description");
}

#include "moc_eventcommand.cpp"
