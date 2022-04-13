/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "dbmigrations.h"
#include <QSqlDatabase>

#define DBCONNAME "dbmigrations"

DbMigrations::DbMigrations(const QString &iniPath, bool quiet)
    : Configuration(iniPath, quiet)
{

}

DbMigrations::~DbMigrations() = default;

int DbMigrations::openDb()
{
    int rc = loadConfig();
    if (rc != 0) {
        return rc;
    }

    //% "Establishing database connection"
    printStatus(qtTrId("gikctl-status-open-db"));

    const QString dbType = value(QStringLiteral("Database"), QStringLiteral("type"), QStringLiteral("QMYSQL")).toString().toUpper();

    if (dbType == QLatin1String("QMYSQL")) {

        const QString dbHost = value(QStringLiteral("Database"), QStringLiteral("host"), QStringLiteral("localhost")).toString();
        const QString dbUser = value(QStringLiteral("Database"), QStringLiteral("user"), QStringLiteral("gikwimi")).toString();
        const QString dbPass = value(QStringLiteral("Database"), QStringLiteral("password")).toString();
        const QString dbName = value(QStringLiteral("Database"), QStringLiteral("name"), QStringLiteral("gikwimidb")).toString();
        const int     dbPort = value(QStringLiteral("Database"), QStringLiteral("port"), 3306).toInt();

        auto db = QSqlDatabase::addDatabase(dbType, QStringLiteral(DBCONNAME));
        if (Q_UNLIKELY(!db.isValid())) {
            printFailed();
            //: Error message
            //% "Failed to obtain database object."
            rc = dbError(qtTrId("gikctl-error-db-invalid-object"));
            return rc;
        }

        db.setDatabaseName(dbName);
        db.setUserName(dbUser);
        db.setPassword(dbPass);

        if (dbHost[0] == QLatin1Char('/')) {
            db.setConnectOptions(QStringLiteral("UNIX_SOCKET=%1").arg(dbHost));
        } else {
            db.setHostName(dbHost);
            db.setPort(dbPort);
        }

        if (!db.open()) {
            printFailed();
            rc = dbError(db);
            return rc;
        }

    } else {
        printFailed();
        //: Error message, %1 will be the invalid DB type
        //% "Can not open database connection for invalid database type “%1”."
        rc = configError(qtTrId("gikctl-error-config-invalid-dbtype").arg(dbType));
        return rc;
    }

    printDone();

    return rc;
}

int DbMigrations::runMigrations()
{
    int rc = openDb();
    if (rc != 0) {
        return rc;
    }

    return rc;
}

int DbMigrations::runRollbacks(uint steps)
{
    int rc = openDb();
    if (rc != 0) {
        return rc;
    }

    return rc;
}

int DbMigrations::runRollbacks(const QString &steps)
{
    bool ok = false;
    const uint _steps = steps.toUInt(&ok);
    if (!ok) {
        //: Error message, %1 will be replaced by the invalid value string
        //% "“%1” is not a valid unsigned integer value."
        return inputError(qtTrId("gikctl-error-input-invalid-uint"));
    }

    return runRollbacks(_steps);
}

int DbMigrations::runReset()
{
    int rc = openDb();
    if (rc != 0) {
        return rc;
    }

    return rc;
}

int DbMigrations::runRefresh(uint steps)
{
    int rc = openDb();
    if (rc != 0) {
        return rc;
    }

    return rc;
}

int DbMigrations::runRefresh(const QString &steps)
{
    bool ok = false;
    const uint _steps = steps.toUInt(&ok);
    if (!ok) {
        //: Error message, %1 will be replaced by the invalid value string
        //% "“%1” is not a valid unsigned integer value."
        return inputError(qtTrId("gikctl-error-input-invalid-uint"));
    }

    return runRefresh(_steps);
}
