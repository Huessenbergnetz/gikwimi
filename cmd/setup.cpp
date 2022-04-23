/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "setup.h"

#include <Cutelyst/Plugins/Utils/validatoremail.h>
#include <Cutelyst/Plugins/Authentication/credentialpassword.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QRandomGenerator>

#define DBCONNAME "dbsetup"

Setup::Setup(const QString &iniPath, bool quiet)
    : Database(iniPath, quiet)
{

}

Setup::~Setup() = default;

int Setup::addUser(const QString &name, const QString &email, const QString &password, const QString &type)
{
    int rc = openDb(QStringLiteral(DBCONNAME));
    if (rc != 0) {
        return rc;
    }

    //% "Adding new user"
    printStatus(qtTrId("gikctl-status-add-user"));

    if (name.isEmpty()) {
        printFailed();
        //: CLI error message
        //% "Can not add a new user without a valid username"
        return inputError(qtTrId("gikctl-cli-adduser-empty-name"));
    }

    if (email.isEmpty()) {
        printFailed();
        //: CLI error message
        //% "Can not add a new user without a valid email address"
        return inputError(qtTrId("gikctl-cli-adduser-invalid-email"));
    }

    if (!Cutelyst::ValidatorEmail::validate(email, Cutelyst::ValidatorEmail::RFC5321, Cutelyst::ValidatorEmail::AllowIDN)) {
        printFailed();
        return inputError(qtTrId("gikctl-cli-adduser-invalid-email"));
    }

    QString _password;
    bool passwordGenerated = false;
    if (password.isEmpty()) {
        const int passwordLength = 14;
        auto rand = QRandomGenerator::global();
        _password.reserve(passwordLength);
        for (int i = 0; i < passwordLength; ++i) {
            _password.append(QChar(rand->bounded(33, 126)));
        }
        passwordGenerated = true;
    } else {
        _password = password;
    }

    bool ok = false;
    ushort _t = type.toUShort(&ok);
    if (!ok || _t > 255) {
        printFailed();
        //: CLI error message
        //% "Cannot add a new user with invalid type that is not in the range between 0 and 255."
        return inputError(qtTrId("gikctl-cli-adduser-invalid-type"));
    }
    const quint8 _type = static_cast<quint8>(_t);

    const QString passwordEnc = Cutelyst::CredentialPassword::createPassword(_password);
    if (passwordEnc.isEmpty()) {
        //: CLI error message
        //% "Failed to encrypt password"
        return internalError(qtTrId("gikctl-cli-err-failed-pass-enc"));
    }

    const QDateTime now = QDateTime::currentDateTimeUtc();

    QSqlQuery q(QSqlDatabase::database(QStringLiteral(DBCONNAME)));

    if (Q_UNLIKELY(!q.prepare(QStringLiteral("SELECT username, email FROM users WHERE username = :username OR email = :email")))) {
        printFailed();
        return dbError(q);
    }
    q.bindValue(QStringLiteral(":username"), name);
    q.bindValue(QStringLiteral(":email"), email);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        return dbError(q);
    }

    if (q.next()) {
        if (q.value(0).toString() == name) {
            printFailed();
            //: CLI error message, %1 will be replaced by the username
            //% "The username “%1“ is already in use"
            return inputError(qtTrId("gikctl-cli-err-username-in-use").arg(name));
        }
        if (q.value(1).toString() == email) {
            printFailed();
            //: CLI error message, %1 will be replaced by the email address, %2 by the username
            //% "The email address “%1” is already in use by “%2“"
            return inputError(qtTrId("gikctl-cli-err-email-in-use").arg(email, name));
        }
    }


    if (Q_UNLIKELY(!q.prepare(QStringLiteral("INSERT INTO users (type, username, password, email, created_at, updated_at) VALUES (:type, :username, :password, :email, :created_at, :updated_at)")))) {
        printFailed();
        return dbError(q);
    }
    q.bindValue(QStringLiteral(":type"), _type);
    q.bindValue(QStringLiteral(":username"), name);
    q.bindValue(QStringLiteral(":password"), passwordEnc);
    q.bindValue(QStringLiteral(":email"), email);
    q.bindValue(QStringLiteral(":created_at"), now);
    q.bindValue(QStringLiteral(":updated_at"), now);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        return dbError(q);
    }

    printDone();

    if (passwordGenerated) {
        //% "Generated password for user “%1“: %2"
        printMessage(qtTrId("gikctl-cli-status-generated-password").arg(name, _password));
    }

    return rc;
}
