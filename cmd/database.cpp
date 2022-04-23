/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "database.h"
#include "confignames.h"

#include <QSqlDatabase>

#define DBCONNAME "dbmigrations"

Database::Database(const QString &iniPath, bool quiet)
    : Configuration(iniPath, quiet)
{

}

Database::~Database() = default;

int Database::openDb(const QString &connectionName)
{
    int rc = loadConfig();
    if (rc != 0) {
        return rc;
    }

    //% "Establishing database connection"
    printStatus(qtTrId("gikctl-status-open-db"));

    const QString dbConfSection = QStringLiteral(GIKWIMI_CONF_DB);
    const QString dbType = value(dbConfSection, QStringLiteral(GIKWIMI_CONF_DB_TYPE), QStringLiteral(GIKWIMI_CONF_DB_TYPE_DEFVAL)).toString().toUpper();

    if (dbType == QLatin1String("QMYSQL")) {

        const QString dbHost = value(dbConfSection, QStringLiteral(GIKWIMI_CONF_DB_HOST), QStringLiteral(GIKWIMI_CONF_DB_HOST_DEFVAL)).toString();
        const QString dbUser = value(dbConfSection, QStringLiteral(GIKWIMI_CONF_DB_USER), QStringLiteral(GIKWIMI_CONF_DB_USER_DEFVAL)).toString();
        const QString dbPass = value(dbConfSection, QStringLiteral(GIKWIMI_CONF_DB_PASS)).toString();
        const QString dbName = value(dbConfSection, QStringLiteral(GIKWIMI_CONF_DB_NAME), QStringLiteral(GIKWIMI_CONF_DB_NAME_DEFVAL)).toString();
        const int     dbPort = value(dbConfSection, QStringLiteral(GIKWIMI_CONF_DB_PORT), GIKWIMI_CONF_DB_PORT_DEFVAL).toInt();

        auto db = QSqlDatabase::addDatabase(dbType, connectionName);
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
