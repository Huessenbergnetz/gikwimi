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

    //: CLI option description
    //% "Be quiet and print less output."
    QCommandLineOption quiet(QStringList({QStringLiteral("q"), QStringLiteral("quiet")}), qtTrId("gikctl-cliopt-quiet-desc"));
    parser.addOption(quiet);

#ifdef QT_DEBUG
    QCommandLineOption dbMigrate(QStringLiteral("db-migrate"), QStringLiteral("Runs all database migrations not already applied"));
    parser.addOption(dbMigrate);

    QCommandLineOption dbRollback(QStringLiteral("db-rollback"),
                                  QStringLiteral("Rolls back given steps of already applied database migrations."),
                                  QStringLiteral("steps"));
    parser.addOption(dbRollback);

    QCommandLineOption dbRefresh(QStringLiteral("db-refresh"),
                                 QStringLiteral("Rolls back and reapplies the given number of database migrations. If steps is 0, all migrations will be rolled back and reapplied."),
                                 QStringLiteral("steps"));
    parser.addOption(dbRefresh);

    QCommandLineOption dbReset(QStringLiteral("db-reset"), QStringLiteral("Resets the database by rolling back all migrations."));
    parser.addOption(dbReset);
#endif

    //: CLI option description
    //% "Run setup process."
    QCommandLineOption setup(QStringLiteral("setup"), qtTrId("gikctl-cliopt-setup-desc"));
    parser.addOption(setup);

    parser.process(app);

#ifdef QT_DEBUG
    if (parser.isSet(dbMigrate)) {
        DbMigrations migs(parser.value(iniFile), parser.isSet(quiet));
        return migs.runMigrations();
    }

    if (parser.isSet(dbRollback)) {
        DbMigrations migs(parser.value(iniFile), parser.isSet(quiet));
        return migs.runRollbacks(parser.value(dbRollback));
    }

    if (parser.isSet(dbRefresh)) {
        DbMigrations migs(parser.value(iniFile), parser.isSet(quiet));
        return migs.runRefresh(parser.value(dbRefresh));
    }

    if (parser.isSet(dbReset)) {
        DbMigrations migs(parser.value(iniFile), parser.isSet(quiet));
        return migs.runReset();
    }
#endif

    if (parser.isSet(setup)) {

    }

    return 0;
}
