/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controller.h"

#include "commands/databasecommand.h"
#include "commands/usercommand.h"
#include "commands/addressbookcommand.h"
#include "commands/contactcommand.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QTextStream>

Controller::Controller(QObject *parent)
    : QObject{parent}
{
    setObjectName(QStringLiteral("controller"));
}

Controller::~Controller() = default;

void Controller::init()
{
    m_globalOptions.append(QCommandLineOption(QStringList({QStringLiteral("h"), QStringLiteral("help")}),
                                              //: CLI option description
                                              //% "Displays help on commandline options and commands."
                                              qtTrId("gikctl-opt-global-help-desc")));

    const QString iniFileDefVal = QStringLiteral(GIKWIMI_CONFFILE);
    m_globalOptions.append(QCommandLineOption(QStringList({QStringLiteral("i"), QStringLiteral("ini")}),
                                              //: CLI option description, %1 will be the default value
                                              //% "Path to configuration ini file. Default: %1"
                                              qtTrId("gikctl-opt-global-inifile-desc").arg(iniFileDefVal),
                                              //: CLI option value name
                                              //% "ini file"
                                              qtTrId("gikctl-opt-global-inifile-val"),
                                              iniFileDefVal));
    m_globalOptions.append(QCommandLineOption(QStringList({QStringLiteral("q"), QStringLiteral("quiet")}),
                                              //: CLI option description
                                              //% "Be quiet and print less output."
                                              qtTrId("gikctl-opt-global-quiet-desc")));

    new DatabaseCommand(this);
    new UserCommand(this);
    new AddressBookCommand(this);
    new ContactCommand(this);
}

int Controller::exec(QCommandLineParser *parser)
{
    parser->addOptions(m_globalOptions);
    parser->addPositionalArgument(QStringLiteral("command"), QString());

    parser->parse(QCoreApplication::arguments());

    const QStringList args = parser->positionalArguments();
    const QString command = args.empty() ? QLatin1String("") : args.first();

    auto com = findChild<Command *>(command, Qt::FindDirectChildrenOnly);

    if (com) {
        com->init();
        return com->exec(parser);
    } else {
        showHelp();
    }

    return 0;
}


void Controller::showHelp() const
{
    const QStringList args = QCoreApplication::arguments();
    const QString exeName = args.first();

    QTextStream out(stdout, QIODevice::WriteOnly);

    out << qtTrId("gikctl-help-usage") << ' ' << exeName << ' ';
    out << qtTrId("gikctl-help-usage-global-options") << ' ';
    out << qtTrId("gikctl-help-usage-command") << '\n';
    out << '\n';

    //: General description for gikwimictl
    //% "Gikwimi is a online event guest management system. This command line client helps to manage several aspects of Gikwimi from the command line."
    out << qtTrId("gikctl-help-description") << '\n';
    out << '\n';

    out << qtTrId("gikctl-help-header-global-options") << '\n';
    int longestOptionNameAndValue = 0;
    for (const QCommandLineOption &opt : qAsConst(m_globalOptions)) {
        const QStringList names = opt.names();
        QString longName;
        for (const QString &name : names) {
            if (name.length() > 1) {
                longName = name;
            }
        }
        longestOptionNameAndValue = std::max(longestOptionNameAndValue, longName.length() + opt.valueName().length());
    }
    for (const QCommandLineOption &opt : qAsConst(m_globalOptions)) {
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
        out << "  " << shortName << qSetFieldWidth(longestOptionNameAndValue + 10) << Qt::left << longNameAndValue << qSetFieldWidth(0) << opt.description() << '\n';
#else
        out << "  " << shortName << qSetFieldWidth(longestOptionNameAndValue + 10) << left << longNameAndValue << qSetFieldWidth(0) << opt.description() << '\n';
#endif
    }
    out << '\n';

    out << qtTrId("gikctl-help-header-commands") << '\n';

    const QList<Command*> coms = findChildren<Command *>(QString(), Qt::FindDirectChildrenOnly);
    int maxCommandNameLength = 0;
    for (const auto com : coms) {
        maxCommandNameLength = std::max(maxCommandNameLength, com->objectName().length());
    }
    for (const auto com : coms) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
            out << "  " << qSetFieldWidth(maxCommandNameLength + 4) << Qt::left << com->objectName() << qSetFieldWidth(0) << " " << com->summary() << '\n';
#else
            out << "  " << qSetFieldWidth(maxCommandNameLength + 4) << left << com->objectName() << qSetFieldWidth(0) << " " << com->summary() << '\n';
#endif
    }
}

QList<QCommandLineOption> Controller::globalOptions() const
{
    return m_globalOptions;
}

#include "moc_controller.cpp"
