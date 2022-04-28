/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "useraddcommand.h"

#include "../../app/objects/user.h"

#include <Cutelyst/Plugins/Utils/validatoremail.h>
#include <Cutelyst/Plugins/Authentication/credentialpassword.h>

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QRandomGenerator>
#include <QLocale>

#define DBCONNAME "usermanagement"

UserAddCommand::UserAddCommand(QObject *parent)
    : Command{parent}
{
    setObjectName(QStringLiteral("add"));
}

UserAddCommand::~UserAddCommand() = default;

void UserAddCommand::init()
{
    QLocale locale;

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("u"), QStringLiteral("username")}),
                                           //: CLI option description
                                           //% "The user name for the user to create."
                                           qtTrId("gikctl-opt-user-add-username-desc"),
                                           //: CLI option value name
                                           //% "name"
                                           qtTrId("gikctl-opt-user-add-username-value")));

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("e"), QStringLiteral("email")}),
                                           //: CLI option description
                                           //% "The email address of the user to create."
                                           qtTrId("gikctl-opt-user-add-email-desc"),
                                           //: CLI option value name
                                           //% "email"
                                           qtTrId("gikctl-opt-user-add-email-value")));

    const QString userTypeDefVal = User::typeEnumToString(User::Registered);
    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("t"), QStringLiteral("type")}),
                                           //: CLI option description, %1 will be replaced by a list of supported types, %2 by the default value
                                           //% "The type of the new user. Available types: %1. Default value: %2."
                                           qtTrId("gikctl-opt-user-add-type-desc").arg(locale.createSeparatedList(User::supportedTypes()), userTypeDefVal),
                                           //: CLI option value name
                                           //% "type"
                                           qtTrId("gikctl-opt-user-add-type-value"),
                                           userTypeDefVal));

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("p"), QStringLiteral("password")}),
                                           //: CLI option description
                                           //% "The password for the new user. If empty, a random password will be generated that will be displayed after the user has been created successfully."
                                           qtTrId("gikctl-opt-user-add-password-desc"),
                                           //: CLI option value name
                                           //% "password"
                                           qtTrId("gikctl-opt-user-add-password-value")));
}

int UserAddCommand::exec(QCommandLineParser *parser)
{
    parser->addOptions(m_cliOptions);
    parser->parse(QCoreApplication::arguments());

    setGlobalOptions(parser);

    int rc = openDb(QStringLiteral(DBCONNAME));
    if (rc != 0) {
        return rc;
    }

    if (checkShowHelp(parser)) {
        return rc;
    }

    //% "Adding new user"
    printStatus(qtTrId("gikctl-status-add-user"));

    const QString username  = parser->value(QStringLiteral("username")).trimmed();
    const QString email     = parser->value(QStringLiteral("email")).trimmed();
    const User::Type type   = User::typeStringToEnum(parser->value(QStringLiteral("type")).trimmed());
    const QString password  = parser->value(QStringLiteral("password").trimmed());

    if (username.isEmpty()) {
        printFailed();
        //: CLI error message
        //% "Can not add a new user without a valid username"
        return inputError(qtTrId("gikctl-cli-user-add-err-empty-name"));
    }

    if (email.isEmpty()) {
        printFailed();
        //: CLI error message
        //% "Can not add a new user without a valid email address"
        return inputError(qtTrId("gikctl-cli-user-add-err-invalid-email"));
    }

    if (!Cutelyst::ValidatorEmail::validate(email, Cutelyst::ValidatorEmail::RFC5321, Cutelyst::ValidatorEmail::AllowIDN)) {
        printFailed();
        return inputError(qtTrId("gikctl-cli-user-add-err-invalid-email"));
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

    const QString passwordEnc = Cutelyst::CredentialPassword::createPassword(_password);
    if (passwordEnc.isEmpty()) {
        //: CLI error message
        //% "Failed to encrypt password"
        return internalError(qtTrId("gikctl-cli-user-add-err-failed-pwenc"));
    }

    const QDateTime now = QDateTime::currentDateTimeUtc();

    QSqlQuery q(QSqlDatabase::database(QStringLiteral(DBCONNAME)));

    if (Q_UNLIKELY(!q.prepare(QStringLiteral("SELECT username, email FROM users WHERE username = :username OR email = :email")))) {
        printFailed();
        return dbError(q);
    }
    q.bindValue(QStringLiteral(":username"), username);
    q.bindValue(QStringLiteral(":email"), email);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        return dbError(q);
    }

    if (q.next()) {
        if (q.value(0).toString() == username) {
            printFailed();
            //: CLI error message, %1 will be replaced by the username
            //% "The username “%1“ is already in use"
            return inputError(qtTrId("gikctl-cli-user-add-err-username-in-use").arg(username));
        }
        if (q.value(1).toString() == email) {
            printFailed();
            //: CLI error message, %1 will be replaced by the email address, %2 by the username
            //% "The email address “%1” is already in use by “%2“"
            return inputError(qtTrId("gikctl-cli-user-add-err-email-in-use").arg(email, q.value(0).toString()));
        }
    }

    if (Q_UNLIKELY(!q.prepare(QStringLiteral("INSERT INTO users (type, username, password, email, created_at, updated_at) VALUES (:type, :username, :password, :email, :created_at, :updated_at)")))) {
        printFailed();
        return dbError(q);
    }
    q.bindValue(QStringLiteral(":type"), static_cast<qint8>(type));
    q.bindValue(QStringLiteral(":username"), username);
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
        printMessage(qtTrId("gikctl-cli-status-generated-password").arg(username, _password));
    }

    return rc;
}

QString UserAddCommand::summary() const
{
    //: CLI command summary
    //% "Add a new user"
    return qtTrId("gikctl-command-user-add-summary");
}

QString UserAddCommand::description() const
{
    //: CLI command description
    //% "Add a new user with the given option parameters."
    return qtTrId("gikctl-command-user-add-description");
}

#include "moc_useraddcommand.cpp"
