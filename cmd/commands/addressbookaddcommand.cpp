/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "addressbookaddcommand.h"

#include "../../app/objects/addressbook.h"

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QLocale>

#define DBCONNAME "addressbookmanagement"

AddressBookAddCommand::AddressBookAddCommand(QObject *parent)
    : Command{parent}
{
    setObjectName(QStringLiteral("add"));
}

AddressBookAddCommand::~AddressBookAddCommand() = default;

void AddressBookAddCommand::init()
{
    QLocale locale;

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("n"), QStringLiteral("name")}),
                                           qtTrId("gikctl-opt-addressbook-add-name-desc"),
                                           qtTrId("gikctl-opt-addressbook-add-name-value")));

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("u"), QStringLiteral("user")}),
                                           qtTrId("gikctl-opt-addressbook-add-user-desc"),
                                           qtTrId("gikctl-opt-addressbook-add-user-value")));

    const QString typeDefVal = AddressBook::typeEnumToString(AddressBook::Local);
    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("t"), QStringLiteral("type")}),
                                           qtTrId("gikctl-opt-addressbook-add-user-desc").arg(locale.createSeparatedList(AddressBook::supportedTypes()), typeDefVal),
                                           qtTrId("gikctl-opt-addressbook-add-user-value"),
                                           typeDefVal));

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("d"), QStringLiteral("data")}),
                                           qtTrId("gikctl-opt-addressbook-add-data-desc"),
                                           qtTrId("gikctl-opt-addressbook-add-data-value")));
}

int AddressBookAddCommand::exec(QCommandLineParser *parser)
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

    const QString name              = parser->value(QStringLiteral("name")).trimmed();
    const QString user              = parser->value(QStringLiteral("user")).trimmed();
    const AddressBook::Type type    = AddressBook::typeStringToEnum(parser->value(QStringLiteral("type")).trimmed());
    const QString data              = parser->value(QStringLiteral("data")).trimmed();

    //% "Adding new addressbook “%1“"
    printStatus(qtTrId("gikctl-status-add-addressbook").arg(name));

    if (name.isEmpty()) {
        printFailed();
        //: CLI error message
        //% "Can not add a new addresbook with an empty name"
        return inputError(qtTrId("gikctl-addressbook-add-err-empty-name"));
    }

    if (user.isEmpty()) {
        printFailed();
        //: CLI error message
        //% "Can not add a new addressbook without a valid user"
        return inputError(qtTrId("gikctl-addressbook-add-err-empty-user"));
    }

    if (type == AddressBook::Invalid) {
        printFailed();
        QLocale locale;
        //: CLI error message
        //% "Can not add a new addressbook of not supported type “%1”. Currently supported types are: %2"
        return inputError(qtTrId("gikctl-addrebook-add-err-invalid-type").arg(parser->value(QStringLiteral("type")).trimmed(), locale.createSeparatedList(AddressBook::supportedTypes())));
    }

    QSqlQuery q(QSqlDatabase::database(QStringLiteral(DBCONNAME)));

    bool userIsId = false;
    uint userId = user.toUInt(&userIsId);

    if (!userIsId) {
        q.prepare(QStringLiteral("SELECT id FROM users WHERE username = :user OR email = :user"));
        q.bindValue(QStringLiteral(":user"), user);
        if (Q_UNLIKELY(!q.exec())) {
            printFailed();
            return dbError(q);
        }
        if (Q_UNLIKELY(!q.next())) {
            printFailed();
            //: CLI error message
            //% "Can not find user"
            return inputError(qtTrId("gikctl-err-add-addressbook-user-not-found"));
        }
        userId = q.value(0).toUInt();
    }

    if (Q_UNLIKELY(!q.prepare(QStringLiteral("INSERT INTO addressbooks (user_id, type, name, data) VALUES (:user_id, :type, :name, :data)")))) {
        printFailed();
        return dbError(q);
    }

    q.bindValue(QStringLiteral(":user_id"), userId);
    q.bindValue(QStringLiteral(":type"), static_cast<quint8>(type));
    q.bindValue(QStringLiteral(":name"), name);
    q.bindValue(QStringLiteral(":data"), data);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        return dbError(q);
    }

    printDone();

    return rc;
}

QString AddressBookAddCommand::summary() const
{
    return qtTrId("gikctl-cli-command-addressbook-add-summary");
}

QString AddressBookAddCommand::description() const
{
    return qtTrId("gikctl-cli-command-addressbook-add-description");
}

#include "moc_addressbookaddcommand.cpp"
