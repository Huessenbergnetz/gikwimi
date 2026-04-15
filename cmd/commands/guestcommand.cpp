/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guestcommand.h"
#include "guestexportcommand.h"

#include <QCoreApplication>
#include <QCommandLineParser>

using namespace Qt::StringLiterals;

GuestCommand::GuestCommand(QObject *parent)
    : Command{parent}
{
    setObjectName(u"guest"_s);
}

GuestCommand::~GuestCommand() = default;

void GuestCommand::init()
{
    new GuestExportCommand(this);
}

int GuestCommand::exec(QCommandLineParser *parser)
{
    parser->addPositionalArgument(u"subcommand"_s, {});
    parser->parse(QCoreApplication::arguments());

    setGlobalOptions(parser);

    QStringList args = parser->positionalArguments();
    if (args.takeFirst().toLower() != objectName()) {
        qFatal("%s", "Invalid command name! Aborting...");
    }

    const QString command = args.empty() ? ""_L1 : args.takeFirst().toLower();

    return runSubCommand(command, parser);
}

QString GuestCommand::summary() const
{
    //: CLI command summary
    //% "Manage guests"
    return qtTrId("gikctl-command-guest-summary");
}

QString GuestCommand::description() const
{
    //: CLI command description
    //% "Add, edit, delete, list and export guests of an event."
    return qtTrId("gikctl-comman-guest-description");
}

#include "moc_guestcommand.cpp"
