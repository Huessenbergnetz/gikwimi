/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include "dbmigrations/dbmigrations.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("Huessenbergnetz"));
    app.setOrganizationDomain(QStringLiteral("huessenbergnetz.de"));
    app.setApplicationName(QStringLiteral("gikwimictl"));
    app.setApplicationVersion(QStringLiteral(GIKWIMI_VERSION));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    const QString iniFileDefVal  = QStringLiteral(GIKWIMI_CONFFILE);
    QCommandLineOption iniFile(QStringList({QStringLiteral("i"), QStringLiteral("ini")}),
                               //: CLI option description, %1 will be the default value
                               //% "Path to configuration ini file. Default: %1"
                               qtTrId("gikctl-cliopt-inifile-desc").arg(iniFileDefVal),
                               //: CLI option value name
                               //% "ini file"
                               qtTrId("gikctl-cliopt-inifile-val"),
                               iniFileDefVal);
    parser.addOption(iniFile);

    const QString distIniFileDefVal = QStringLiteral(GIKWIMI_DISTCONFFILE);
    QCommandLineOption distIniFile(QStringList({QStringLiteral("dist-ini")}),
                                   //: CLI option description, %1 will be the default value
                                   //% "Path to distribution configuration ini file. Default: %1"
                                   qtTrId("gikctl-cliopt-distini-desc").arg(distIniFileDefVal),
                                   qtTrId("gikctl-cliopt-inifile-val"),
                                   distIniFileDefVal);
    parser.addOption(distIniFile);

    //: CLI option description
    //% "Be quiet and print less output."
    QCommandLineOption quiet(QStringList({QStringLiteral("q"), QStringLiteral("quiet")}), qtTrId("gikctl-cliopt-quiet-desc"));
    parser.addOption(quiet);

    //: CLI option description
    //% "Runs all database migrations not already applied"
    QCommandLineOption dbMigrate(QStringLiteral("db-migrate"), qtTrId("gikctl-cliopt-dbmigrate-desc"));
    parser.addOption(dbMigrate);

    QCommandLineOption dbRollback(QStringLiteral("db-rollback"),
                                  //: CLI option description
                                  //% "Rolls back given steps of already applied database migrations."
                                  qtTrId("gikctl-cliopt-dbrollback-desc"),
                                  //: CLI option value name
                                  //% "steps"
                                  qtTrId("gikctl-cliopt-dbrollback-val"));
    parser.addOption(dbRollback);

    QCommandLineOption dbRefresh(QStringLiteral("db-refresh"),
                                 //: CLI option description
                                 //% Rolls back and reapplies the given number of database migrations. If steps is 0, all migrations will be rolled back and reapplied."
                                 qtTrId("gikctl-cliopt-dbrefresh-desc"),
                                 //: CLI option value name
                                 //% "steps"
                                 qtTrId("gikctl-cliopt-dbrefresh-val"));
    parser.addOption(dbRefresh);

    //: CLI option description
    //% "Resets the database by rolling back all migrations."
    QCommandLineOption dbReset(QStringLiteral("db-reset"), qtTrId("gikctl-cliopt-dbreset-desc"));
    parser.addOption(dbReset);

    parser.process(app);

    if (parser.isSet(dbMigrate)) {
        DbMigrations migs(parser.value(distIniFile), parser.value(iniFile), parser.isSet(quiet));
        return migs.runMigrations();
    }

    if (parser.isSet(dbRollback)) {
        DbMigrations migs(parser.value(distIniFile), parser.value(iniFile), parser.isSet(quiet));
        return migs.runRollbacks(parser.value(dbRollback));
    }

    if (parser.isSet(dbRefresh)) {
        DbMigrations migs(parser.value(distIniFile), parser.value(iniFile), parser.isSet(quiet));
        return migs.runRefresh(parser.value(dbRefresh));
    }

    if (parser.isSet(dbReset)) {
        DbMigrations migs(parser.value(distIniFile), parser.value(iniFile), parser.isSet(quiet));
        return migs.runReset();
    }

    return 0;
}
