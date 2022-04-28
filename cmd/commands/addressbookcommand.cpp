/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "addressbookcommand.h"
#include "addressbookaddcommand.h"

#include <QCoreApplication>
#include <QCommandLineParser>

AddressBookCommand::AddressBookCommand(QObject *parent)
    : Command{parent}
{
    setObjectName(QStringLiteral("addressbook"));
}

AddressBookCommand::~AddressBookCommand() = default;

void AddressBookCommand::init()
{
    new AddressBookAddCommand(this);
}

int AddressBookCommand::exec(QCommandLineParser *parser)
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

QString AddressBookCommand::summary() const
{
    return qtTrId("gikctl-cli-command-addressbook-summary");
}

QString AddressBookCommand::description() const
{
    return qtTrId("gikctl-cli-command-addressbook-description");
}

#include "moc_addressbookcommand.cpp"
