/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "contactcommand.h"
#include "contactimportcommand.h"

#include <QCoreApplication>
#include <QCommandLineParser>

ContactCommand::ContactCommand(QObject *parent)
    : Command{parent}
{
    setObjectName(QStringLiteral("contact"));
}

ContactCommand::~ContactCommand() = default;

void ContactCommand::init()
{
    new ContactImportCommand(this);
}

int ContactCommand::exec(QCommandLineParser *parser)
{
    parser->addPositionalArgument(QStringLiteral("subcommand"), QString());
    parser->parse(QCoreApplication::arguments());

    setGlobalOptions(parser);

    QStringList args = parser->positionalArguments();
    if (args.takeFirst() != objectName()) {
        qFatal("%s", "Invalid command name! Aborting...");
    }

    const QString command = args.empty() ? QLatin1String("") : args.takeFirst().toLower();

    return runSubCommand(command, parser);
}

QString ContactCommand::summary() const
{
    //: CLI command summary
    //% "Manage contacts"
    return qtTrId("gikctl-command-contact-summary");
}

QString ContactCommand::description() const
{
    //: CLI command description
    //% "Add, edit, delete and list contacts in a user’s addressbook."
    return qtTrId("gikctl-command-contact-description");
}

#include "moc_contactcommand.cpp"
