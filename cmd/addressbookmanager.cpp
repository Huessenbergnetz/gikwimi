/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "addressbookmanager.h"
#include "../app/objects/addressbook.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QLocale>

#define DBCONNAME "addressbookmanagement"

AddressBookManager::AddressBookManager(const QString &iniPath, bool quiet)
    : Database(iniPath, quiet)
{

}

AddressBookManager::~AddressBookManager() = default;

int AddressBookManager::add(const QString &user, const QString &name, const QString &type, const QString &data)
{
    int rc = openDb(QStringLiteral(DBCONNAME));
    if (rc != 0) {
        return rc;
    }

    //% "Adding new addressbook “%1“"
    printStatus(qtTrId("gikctl-status-add-addressbook").arg(name));

    if (user.isEmpty()) {
        printFailed();
        //: CLI error message
        //% "Can not add a new addressbook without a valid user"
        rc = inputError(qtTrId("gikctl-err-add-addressbook-empty-user"));
        return rc;
    }

    if (name.isEmpty()) {
        printFailed();
        //: CLI error message
        //% "Can not add a new addresbook with an empty name"
        rc = inputError(qtTrId("gikctl-err-add-addressebook-empty-name"));
        return rc;
    }

    const QStringList supportedTypes = AddressBook::supportedTypes();
    if (!supportedTypes.contains(type.toLower())) {
        QLocale locale;
        printFailed();
        //: CLI error message
        //% "Can not add a new addressbo of a not supported type. Currently supported types are: %1"
        rc = inputError(qtTrId("gikctl-err-add-addressbook-invalid-type").arg(locale.createSeparatedList(supportedTypes)));
        return rc;
    }

    QSqlQuery q(QSqlDatabase::database(QStringLiteral(DBCONNAME)));

    bool userIsId = false;
    uint userId = user.toUInt(&userIsId);

    if (!userIsId) {
        q.prepare(QStringLiteral("SELECT id FROM users WHERE username = :user OR email = :user"));
        q.bindValue(QStringLiteral(":user"), user);
        if (Q_UNLIKELY(!q.exec())) {
            printFailed();
            rc = dbError(q);
            return rc;
        }
        if (Q_UNLIKELY(!q.next())) {
            printFailed();
            //: CLI error message
            //% "Can not find user"
            rc = inputError(qtTrId("gikctl-err-add-addressbook-user-not-found"));
            return rc;
        }
        userId = q.value(0).toUInt();
    }

    if (Q_UNLIKELY(!q.prepare(QStringLiteral("INSERT INTO addressbooks (user_id, type, name, data) VALUES (:user_id, :type, :name, :data)")))) {
        printFailed();
        rc = dbError(q);
        return rc;
    }

    q.bindValue(QStringLiteral(":user_id"), userId);
    q.bindValue(QStringLiteral(":type"), static_cast<quint8>(AddressBook::typeStringToEnum(type)));
    q.bindValue(QStringLiteral(":name"), name);
    q.bindValue(QStringLiteral(":data"), data);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        rc = dbError(q);
        return rc;
    }

    printDone();

    return rc;
}

