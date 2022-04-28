/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "command.h"
#include "controller.h"
#include <QCoreApplication>
#include <QTextStream>
#include <QCommandLineParser>
#include <algorithm>

Command::Command(QObject *parent)
    : Database{parent}
{

}

Command::~Command() = default;

void Command::showHelp() const
{
    QTextStream out(stdout, QIODevice::WriteOnly);

    showUsage(&out);

    out << description() << '\n';
    out << '\n';

    showGlobalOptions(&out);

    showSubCommands(&out);

    showOptions(&out);
}

int Command::invalidSubCommand(const QString &subCommand) const
{
    //: CLI error message, %1 will be replaced by the invalid subcommand, %2 by the current command and %3 by the application name
    //% "“%1“ is not a valid subcommand of “%2”. Run '%3 --help %2' for more information on the command."
    return inputError(qtTrId("gikctl-cli-command-error-invalid-subcommand").arg(subCommand, objectName(), QCoreApplication::applicationName()));
}

void Command::showGlobalOptions(QTextStream *out) const
{
    Controller *c = nullptr;
    auto p = parent();
    while (p) {
        if (p->objectName() == QLatin1String("controller")) {
            c = qobject_cast<Controller*>(p);
            break;
        } else {
            p = p->parent();
        }
    }

    const QList<QCommandLineOption> options = c ? c->globalOptions() : QList<QCommandLineOption>();

    if (!options.empty()) {
        //% "Global options:"
        *out << qtTrId("gikctl-clihelp-header-global-options") << '\n';
        showOptions(out, options);
        *out << '\n';
    }
}


void Command::showUsage(QTextStream *out) const
{
    const QStringList args = QCoreApplication::arguments();
    const QString exeName = args.first();

    //% "Usage:"
    *out << qtTrId("gikctl-clihelp-usage") << ' ' << exeName << ' ';
    //% "[global options]"
    *out << qtTrId("gikctl-clihelp-usage-global-options");
    QStringList commands;
    QObject *p = parent();
    while (p->inherits("Command")) {
        commands << p->objectName();
        p = p->parent();
    }

    std::reverse(commands.begin(), commands.end());
    commands << objectName();
    *out << ' ' << commands.join(QChar(QChar::Space));

    if (!findChildren<Command*>(QString(), Qt::FindDirectChildrenOnly).empty()) {
        //% "<command>"
        *out << ' ' << qtTrId("gikctl-clihelp-usage-command");
    }

    if (!m_cliOptions.empty()) {
        //% [options]
        *out << ' ' << qtTrId("gikctl-clihelp-usage-options");
    }

    *out << '\n';
    *out << '\n';
}

void Command::showSubCommands(QTextStream *out) const
{
    const QList<Command *> coms = findChildren<Command *>(QString(), Qt::FindDirectChildrenOnly);
    if (!coms.empty()) {
        // CLI help header
        //% "Commands:"
        *out << qtTrId("gikctl-clihelp-header-commands") << '\n';

        const QList<Command*> coms = findChildren<Command *>(QString(), Qt::FindDirectChildrenOnly);
        int maxCommandNameLength = 0;
        for (const auto com : coms) {
            maxCommandNameLength = std::max(maxCommandNameLength, com->objectName().length());
        }
        for (const auto com : coms) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
            *out << "  " << qSetFieldWidth(maxCommandNameLength + 4) << Qt::left << com->objectName() << qSetFieldWidth(0) << " " << com->summary() << '\n';
#else
            *out << "  " << qSetFieldWidth(maxCommandNameLength + 4) << left << com->objectName() << qSetFieldWidth(0) << " " << com->summary() << '\n';
#endif
        }
        *out << '\n';
    }
}

void Command::showOptions(QTextStream *out) const
{
    if (!m_cliOptions.empty()) {
        //: CLI help header
        //% "Options:"
        *out << qtTrId("gikctl-clihelp-header-options") << '\n';
        showOptions(out, m_cliOptions);
        *out << '\n';
    }
}

void Command::setGlobalOptions(QCommandLineParser *parser)
{
    setIniPath(parser->value(QStringLiteral("ini")));
    setQuiet(parser->isSet(QStringLiteral("quiet")));
}

bool Command::checkShowHelp(QCommandLineParser *parser)
{
    if (parser->isSet(QStringLiteral("help"))) {
        showHelp();
        return true;
    } else {
        return false;
    }
}

int Command::getLongestOptionName(const QList<QCommandLineOption> &options) const
{
    int longest = 0;

    for (const QCommandLineOption &opt : options) {
        const QStringList names = opt.names();
        QString longName;
        for (const QString &name : names) {
            if (name.length() > 1) {
                longName = name;
            }
        }
        longest = std::max(longest, longName.length() + opt.valueName().length());
    }

    return longest;
}

void Command::showOptions(QTextStream *out, const QList<QCommandLineOption> &options) const
{
    int longestLongNameAndValue = getLongestOptionName(options);

    for (const QCommandLineOption &opt : options) {
        QString shortName;
        QString longName;
        const QStringList names = opt.names();
        for (const QString &name : names) {
            if (name.length() == 1) {
                shortName = name;
            } else {
                longName = name;
            }
        }
        if (shortName.isEmpty()) {
            shortName = QLatin1String("    ");
        } else {
            shortName.append(QLatin1String(", ")).prepend(QLatin1Char('-'));
        }

        QString longNameAndValue;
        if (opt.valueName().isEmpty()) {
            longNameAndValue = QLatin1String("--") + longName;
        } else {
            longNameAndValue = QLatin1String("--") + longName + QChar(QChar::Space) + QLatin1Char('<') + opt.valueName() + QLatin1Char('>');
        }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        *out << "  " << shortName << qSetFieldWidth(longestLongNameAndValue + 10) << Qt::left << longNameAndValue << qSetFieldWidth(0) << opt.description() << '\n';
#else
        *out << "  " << shortName << qSetFieldWidth(longestLongNameAndValue + 10) << left << longNameAndValue << qSetFieldWidth(0) << opt.description() << '\n';
#endif
    }
}

int Command::runSubCommand(const QString &command, QCommandLineParser *parser)
{
    auto com = findChild<Command *>(command, Qt::FindDirectChildrenOnly);

    if (com) {
        com->init();
        return com->exec(parser);
    } else {
        showHelp();
        return 0;
    }
}

#include "moc_command.cpp"
