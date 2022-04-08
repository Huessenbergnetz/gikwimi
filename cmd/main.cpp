/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

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

    {
        const QString iniFileDefVal  = QStringLiteral(GIKWIMI_CONFFILE);
        QCommandLineOption iniFile(QStringList({QStringLiteral("ini"), QStringLiteral("i")}),
                                   //: CLI option description, %1 will be the default value
                                   //% "Path to configuration ini file. Default: %1"
                                   qtTrId("gikctl-cliopt-inifile-desc").arg(iniFileDefVal),
                                   //: CLI option value name
                                   //% "ini file"
                                   qtTrId("gikctl-cliopt-inifile-val"),
                                   iniFileDefVal);
        parser.addOption(iniFile);

        //: CLI option description
        //% "Runs all database migrations not already applied"
        QCommandLineOption dbMigrate(QStringLiteral("db-migrate"), qtTrId("gikctl-cliopt-dbmigrate-desc"));
        parser.addOption(dbMigrate);

        const QString dbRollbackDefVal = QStringLiteral("1");
        QCommandLineOption dbRollback(QStringLiteral("db-rollback"),
                                      //: CLI option description, %1 will be the default value
                                      //% "Rolls back given steps of already applied database migrations. Default: %1"
                                      qtTrId("gikctl-cliopt-dbrollback-desc").arg(dbRollbackDefVal),
                                      //: CLI option value name
                                      //% "steps"
                                      qtTrId("gikctl-cliopt-dbrollback-val"),
                                      dbRollbackDefVal);
        parser.addOption(dbRollback);

        const QString dbRefreshDefVal = QStringLiteral("0");
        QCommandLineOption dbRefresh(QStringLiteral("db-refresh"),
                                     //: CLI option description, %1 will be the default value
                                     //% Rolls back and reapplies the given number of database migrations. If steps is 0, all migrations will be rolled back and reapplied. Default: %1"
                                     qtTrId("gikctl-cliopt-dbrefresh-desc").arg(dbRefreshDefVal),
                                     //: CLI option value name
                                     //% "steps"
                                     qtTrId("gikctl-cliopt-dbrefresh-val"),
                                     dbRefreshDefVal);
        parser.addOption(dbRefresh);

        //: CLI option description
        //% "Resets the database by rolling back all migrations."
        QCommandLineOption dbReset(QStringLiteral("db-reset"), qtTrId("gikctl-cliopt-dbreset-desc"));
        parser.addOption(dbReset);
    }

    parser.process(app);

    return 0;
}
