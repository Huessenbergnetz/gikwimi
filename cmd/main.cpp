/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <QCoreApplication>
#include <QTranslator>
#include <QLocale>
#include <QCommandLineOption>
#include <QCommandLineParser>

#ifdef QT_DEBUG
#include "dbmigrations/dbmigrations.h"
#endif
#include "setup.h"
#include "addressbookmanager.h"
#include "usermanager.h"

#include "../app/objects/addressbook.h"
#include "../app/objects/user.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("Huessenbergnetz"));
    app.setOrganizationDomain(QStringLiteral("huessenbergnetz.de"));
    app.setApplicationName(QStringLiteral("gikwimictl"));
    app.setApplicationVersion(QStringLiteral(GIKWIMI_VERSION));

    const QLocale locale;
    {
        auto trans = new QTranslator(&app);
        if (Q_LIKELY(trans->load(locale, QStringLiteral("gikwimictl"), QStringLiteral("_"), QStringLiteral(GIKWIMI_TRANSLATIONSDIR)))) {
            if (Q_UNLIKELY(!QCoreApplication::installTranslator(trans))) {
                qWarning("Can not install translator for locale %s", qUtf8Printable(locale.name()));
            }
        } else {
            qWarning("Can not load translations for locale %s from %s", qUtf8Printable(locale.name()), GIKWIMI_TRANSLATIONSDIR);
        }
    }

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

    QCommandLineOption quiet(QStringList({QStringLiteral("q"), QStringLiteral("quiet")}),
                             //: CLI option description
                             //% "Be quiet and print less output."
                             qtTrId("gikctl-cliopt-quiet-desc"));
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

    QCommandLineOption addUser(QStringLiteral("add-user"),
                               //: CLI option description
                               //% "Add a new user with the given username. The user name must not be assigned yet."
                               qtTrId("gikctl-cliopt-adduser-desc"),
                               //: CLI option value name
                               //% "username"
                               qtTrId("gikctl-cliopt-adduser-value"));
    parser.addOption(addUser);

    const QString userTypeDefault = QStringLiteral("registered");
    QCommandLineOption userType(QStringLiteral("user-type"),
                                //: CLI option description, %1 will be replaced by a list of supported types, %2 by the default value
                                //% "Type for the user to create or edit. Currently supported types: %1. Default: %2"
                                qtTrId("gikctl-cliopt-usertype-desc").arg(locale.createSeparatedList(User::supportedTypes()), userTypeDefault),
                                //: CLI option value name
                                //% "type"
                                qtTrId("gikctl-cliopt-usertype-value"),
                                userTypeDefault);
    parser.addOption(userType);

    QCommandLineOption userEmail(QStringLiteral("user-email"),
                                 //: CLI option description
                                 //% "Email address for the user to create or edit."
                                 qtTrId("gikctl-cliopt-useremail-desc"),
                                 //: CLI option value name
                                 //% "email"
                                 qtTrId("gikctl-cliopt-useremail-value"));
    parser.addOption(userEmail);

    QCommandLineOption userPassword(QStringLiteral("user-password"),
                                    //: CLI option description
                                    //% "Password for the user to create. If empty, a random password will be generated."
                                    qtTrId("gikctl-cliopt-userpass-desc"),
                                    //: CLI option value name
                                    //% "password"
                                    qtTrId("gikctl-cliopt-userpass-value"));
    parser.addOption(userPassword);

    QCommandLineOption userId(QStringLiteral("user"),
                              //: CLI option description
                              //% "User ID or username used to create addressbooks and events."
                              qtTrId("gikctl-cliopt-userid-desc"),
                              //: CLI option value
                              //% "user"
                              qtTrId("gikctl-cliopt-userid-value"));
    parser.addOption(userId);

    QCommandLineOption addAddressBook(QStringLiteral("add-addressbook"),
                                      //: CLI option description
                                      //% "Add a new addressbook with the given name."
                                      qtTrId("gikctl-cliopt-addaddressbook-desc"),
                                      //: CLI option value for addressbook name
                                      //% "name"
                                      qtTrId("gikctl-cliopt-addaddressbook-value"));
    parser.addOption(addAddressBook);

    const QString addressBookTypeDefault = QStringLiteral("local");
    QCommandLineOption addressBookType(QStringLiteral("addressbook-type"),
                                       //: CLI option description, %1 will be replaced by a list of supported types, %2 by the default value
                                       //% "Type of the addressbook to create. Currently supported types: %1. Default: %2"
                                       qtTrId("gikctl-cliopt-addressbooktype-desc").arg(locale.createSeparatedList(AddressBook::supportedTypes()), addressBookTypeDefault),
                                       //: CLI option value for addressbook type
                                       //% "type"
                                       qtTrId("gikctl-cliopt-addressbooktype-value"),
                                       addressBookTypeDefault);
    parser.addOption(addressBookType);

    QCommandLineOption runSetup(QStringLiteral("setup"),
                                //: CLI option description
                                //% "Run setup after installing or upgrading Gikwimi."
                                qtTrId("gikctl-cliopt-runsetup-desc"));
    parser.addOption(runSetup);

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

    if (parser.isSet(addUser)) {
        Setup setup(parser.value(iniFile), parser.isSet(quiet));
        return setup.addUser(parser.value(addUser).trimmed(), parser.value(userEmail).trimmed(), parser.value(userPassword).trimmed(), parser.value(userType).trimmed());
    }

    if (parser.isSet(addAddressBook)) {
        AddressBookManager abm(parser.value(iniFile), parser.isSet(quiet));
        return abm.add(parser.value(userId), parser.value(addAddressBook), parser.value(addressBookType), QString());
    }

    if (parser.isSet(runSetup)) {
        Setup setup(parser.value(iniFile), parser.isSet(quiet));
        return setup.run();
    }

    return 0;
}
