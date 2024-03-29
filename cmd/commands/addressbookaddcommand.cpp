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
#include <QJsonDocument>
#include <QJsonObject>

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
                                           //: CLI option description
                                           //% "The name for the addressbook to create."
                                           qtTrId("gikctl-opt-addressbook-add-name-desc"),
                                           //: CLI option value name
                                           //% "name"
                                           qtTrId("gikctl-opt-addressbook-add-name-value")));

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("u"), QStringLiteral("user")}),
                                           //: CLI option description
                                           //% "The user name, email or ID of the user that will be the owner of the addressbook."
                                           qtTrId("gikctl-opt-addressbook-add-user-desc"),
                                           //: CLI option value name
                                           //% "user"
                                           qtTrId("gikctl-opt-addressbook-add-user-value")));

    const QString typeDefVal = AddressBook::typeEnumToString(AddressBook::Local);
    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("t"), QStringLiteral("type")}),
                                           //: CLI option description, %1 will be replaced by a list of available types, %2 by the default value
                                           //% "The type of the addressbook. Currently supported values: %1. Default value: %2."
                                           qtTrId("gikctl-opt-addressbook-add-type-desc").arg(locale.createSeparatedList(AddressBook::supportedTypes()), typeDefVal),
                                           //: CLI option value name
                                           //% "type"
                                           qtTrId("gikctl-opt-addressbook-add-type-value"),
                                           typeDefVal));

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("s"), QStringLiteral("settings")}),
                                           //: CLI option description
                                           //% "Optional settings required by some addressbook types."
                                           qtTrId("gikctl-opt-addressbook-add-settings-desc"),
                                           //: CLI option value name
                                           //% "key=value;..."
                                           qtTrId("gikctl-opt-addressbook-add-settings-value")));
}

int AddressBookAddCommand::exec(QCommandLineParser *parser)
{
    int rc = 0;

    parser->addOptions(m_cliOptions);
    parser->parse(QCoreApplication::arguments());

    if (checkShowHelp(parser)) {
        return rc;
    }

    setGlobalOptions(parser);

    rc = openDb(QStringLiteral(DBCONNAME));
    if (rc != 0) {
        return rc;
    }

    const QString name              = parser->value(QStringLiteral("name")).trimmed();
    const QString user              = parser->value(QStringLiteral("user")).trimmed();
    const AddressBook::Type type    = AddressBook::typeStringToEnum(parser->value(QStringLiteral("type")).trimmed());
    const QString settings          = parser->value(QStringLiteral("settings")).trimmed();

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

    QString settingsJsonString;
    if (!settings.isEmpty()) {
        QVariantHash settingsHash;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        const QStringList settingsList = settings.split(QLatin1Char(';'), Qt::SkipEmptyParts);
#else
        const QStringList settingsList = settings.split(QLatin1Char(';'), QString::SkipEmptyParts);
#endif
        for (const QString &keyValPair : settingsList) {
            const int idx = keyValPair.indexOf(QLatin1Char('='));
            if (idx > 0) {
                const QString key = keyValPair.left(idx);
                const QString val = keyValPair.mid(idx + 1);
                settingsHash.insert(key, val);
            }
        }
        const QJsonObject settingsJsonObject = QJsonObject::fromVariantHash(settingsHash);
        const QJsonDocument settingsJsonDoc = QJsonDocument(settingsJsonObject);
        settingsJsonString = QString::fromUtf8(settingsJsonDoc.toJson(QJsonDocument::Compact));
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

    if (Q_UNLIKELY(!q.prepare(QStringLiteral("INSERT INTO addressbooks (user_id, type, name, settings, created_at, updated_at) VALUES (:user_id, :type, :name, :settings, :created_at, :updated_at)")))) {
        printFailed();
        return dbError(q);
    }

    const QDateTime now = QDateTime::currentDateTimeUtc();

    q.bindValue(QStringLiteral(":user_id"), userId);
    q.bindValue(QStringLiteral(":type"), static_cast<quint8>(type));
    q.bindValue(QStringLiteral(":name"), name);
    q.bindValue(QStringLiteral(":settings"), settingsJsonString);
    q.bindValue(QStringLiteral(":created_at"), now);
    q.bindValue(QStringLiteral(":updated_at"), now);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        return dbError(q);
    }

    printDone();

    return rc;
}

QString AddressBookAddCommand::summary() const
{
    //: CLI command summary
    //% "Add a new addressbook"
    return qtTrId("gikctl-command-addressbook-add-summary");
}

QString AddressBookAddCommand::description() const
{
    //: CLI command description
    //% "Add a new addressbook for a specific user."
    return qtTrId("gikctl-command-addressbook-add-description");
}

#include "moc_addressbookaddcommand.cpp"
