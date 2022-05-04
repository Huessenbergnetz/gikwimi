/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "contactimportcommand.h"

#include <KContacts/Addressee>
#include <KContacts/VCardConverter>

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QFile>
#include <QMetaEnum>
#include <QLocale>

#define DBCONNAME "contactimport"

ContactImportCommand::ContactImportCommand(QObject *parent)
    : Command{parent}
{
    setObjectName(QStringLiteral("import"));
}

ContactImportCommand::~ContactImportCommand() = default;

void ContactImportCommand::init()
{
    QLocale locale;

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("f"), QStringLiteral("file")}),
                                           //: CLI option description
                                           //% "Path to a vCard file that contains the addresses to import."
                                           qtTrId("gikctl-opt-contact-import-filepath-desc"),
                                           //: CLI option value name
                                           //% "filepath"
                                           qtTrId("gikctl-opt-contact-import-filepath-value")));

    const QString mDefVal = strategyEnumToString(Strategy::Add);
    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("m"), QStringLiteral("merge-strategy")}),
                                           //: CLI option description, %1 will be replaced by the supported values, %2 by the default value
                                           //% "Set the strategy how to handle conflicting contact entries. Currently supported values: %1. Default value: %2."
                                           qtTrId("gikctl-opt-contact-import-merge-desc").arg(locale.createSeparatedList(supportedStrategies()), mDefVal),
                                           //: CLI option value name
                                           //% "strategy"
                                           qtTrId("gikctl-opt-contact-import-merge-value"),
                                           mDefVal));

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("b"), QStringLiteral("addressbook")}),
                                           //: CLI option description
                                           //% "Database ID of the addressbook the contacts should be added to."
                                           qtTrId("gikctl-opt-contact-import-addressbook-desc"),
                                           //: CLI option value name
                                           //% "database id"
                                           qtTrId("gikctl-opt-contact-import-addressbook-value")));
}

int ContactImportCommand::exec(QCommandLineParser *parser)
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

    //% "Importing contacts"
    printStatus(qtTrId("gikctl-status-import-contacts"));

    const QString strategyString = parser->value(QStringLiteral("merge-strategy"));
    Strategy strategy = strategyStringToEnum(strategyString);

    if (strategy == Strategy::Invalid) {
        QLocale locale;
        printFailed();
        //: CLI error message, %1 will be replaced by the invalid value, %2 by the list of allowed values.
        //% "“%1“ is not a valid merge strategy. Currently supported stragies are: %2"
        return inputError(qtTrId("gikctl-contact-import-err-invalid-merge-strategy").arg(strategyString, locale.createSeparatedList(supportedStrategies())));
    }

    const QString addressBookIdStr = parser->value(QStringLiteral("addressbook"));

    if (addressBookIdStr.isEmpty()) {
        printFailed();
        //: CLI error message
        //% "Please use -b to set a database ID of an addressbook the contacts should be imported to"
        return inputError(qtTrId("gikctl-contact-import-err-empty-abid"));
    }

    bool abIdOk = false;
    const uint addressBookId = addressBookIdStr.toUInt(&abIdOk);

    if (!abIdOk) {
        printFailed();
        //: CLI error message, %1 will be replaced by the invalid input value
        //% "“%1“ ist not a valid addressbook database ID"
        return inputError(qtTrId("gikctl-contact-import-err-invalid-abid"));
    }

    const QString filePath = parser->value(QStringLiteral("file")).trimmed();

    if (filePath.isEmpty()) {
        printFailed();
        //: CLI error message
        //% "Please use the -f option to specify a file to import from."
        return inputError(qtTrId("gikctl-contact-import-err-empty-filepath"));
    }

    QFile file(filePath);
    if (!file.exists()) {
        printFailed();
        //: CLI error message, %1 will be replaced by the file path
        //% "Can not find vCard file at %1"
        return fileError(qtTrId("gikctl-contact-import-err-file-not-exists").arg(filePath));
    }

    if (!file.open(QIODevice::ReadOnly)) {
        printFailed();
        //: CLI error message, %1 will be replaced by the file path
        //% "Can not open vCard file at %1 for reading"
        return fileError(qtTrId("gikctl-contact-import-err-open-file").arg(filePath));
    }

    const QByteArray data = file.readAll();
    if (data.isEmpty()) {
        printFailed();
        //: CLI error message, %1 will be replaced by the file path
        //% "Can not import contacts from empty vCard file at %1"
        return fileError(qtTrId("gikctl-contact-import-err-empty-file").arg(filePath));
    }

    KContacts::VCardConverter converter;
    const KContacts::Addressee::List contactList = converter.parseVCards(data);

    if (contactList.empty()) {
        printFailed();
        //: CLI error message, %1 will be replaced by the file path
        //% "Can not find any vCards in file at %1"
        return fileError(qtTrId("gikctl-contact-import-err-empty-vcards").arg(filePath));
    }


    QSqlDatabase db = QSqlDatabase::database(QStringLiteral(DBCONNAME));
    QSqlQuery q(db);

    if (Q_UNLIKELY(!q.prepare(QStringLiteral("SELECT a.name, u.username FROM addressbooks a JOIN users u ON u.id = a.user_id WHERE a.id = :id")))) {
        printFailed();
        return dbError(q);
    }
    q.bindValue(QStringLiteral(":id"), addressBookId);
    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        return dbError(q);
    }

    if (Q_UNLIKELY(!q.next())) {
        printFailed();
        //: CLI error message, %1 will be replaced by the database id
        //% "Can not find addressbook with ID %1 in the database"
        return inputError(qtTrId("gikctl-contact-import-err-ab-not-found").arg(addressBookId));
    }

    const QString abName = q.value(0).toString();
    const QString username = q.value(1).toString();
    const QDateTime now = QDateTime::currentDateTimeUtc();

    if (strategy == Strategy::Replace || strategy == Strategy::Add) {

        db.transaction();
        if (strategy == Strategy::Replace) {
            if (Q_UNLIKELY(!q.prepare(QStringLiteral("DELETE FROM contacts WHERE addressbook_id = :addressbook_id")))) {
                printFailed();
                return dbError(q);
            }
            q.bindValue(QStringLiteral(":addressbook_id"), addressBookId);
            if (Q_UNLIKELY(!q.exec())) {
                printFailed();
                return dbError(q);
            }
        }

        for (const KContacts::Addressee &a : contactList) {
            QString gname, fname;
            if (a.givenName().isEmpty() || a.familyName().isEmpty()) {
                const QStringList realNameParts = a.realName().split(QChar(QChar::Space));
                if (realNameParts.size() > 1) {
                    gname = realNameParts.first();
                    fname = realNameParts.last();
                } else {
                    const QStringList formattedNameParts = a.formattedName().split(QChar(QChar::Space));
                    if (formattedNameParts.size() > 1) {
                        gname = formattedNameParts.first();
                        fname = formattedNameParts.last();
                    }
                }
            } else {
                gname = a.givenName();
                fname = a.familyName();
            }

            if (gname.isEmpty() || fname.isEmpty()) {
                continue;
            }

            if (Q_UNLIKELY(!q.prepare(QStringLiteral("INSERT INTO contacts (addressbook_id, given_name, family_name, vcard, created_at, updated_at) "
                                                     "VALUES (:addressbook_id, :given_name, :family_name, :vcard, :created_at, :updated_at)")))) {
                printFailed();
                return dbError(q);
            }
            q.bindValue(QStringLiteral(":addressbook_id"), addressBookId);
            q.bindValue(QStringLiteral(":given_name"), gname);
            q.bindValue(QStringLiteral(":family_name"), fname);
            q.bindValue(QStringLiteral(":vcard"), QString::fromUtf8(converter.createVCard(a, KContacts::VCardConverter::v4_0)));
            q.bindValue(QStringLiteral(":created_at"), now);
            q.bindValue(QStringLiteral(":updated_at"), now);
            if (Q_UNLIKELY(!q.exec())) {
                printFailed();
                return dbError(q);
            }
        }
        db.commit();

    } else if (strategy == Strategy::KeepNew) {
        printFailed();
        return internalError(QStringLiteral("Not yet implemented"));
    } else if (strategy == Strategy::KeepOld) {
        printFailed();
        return internalError(QStringLiteral("Not yet implemented"));
    }

    printDone();

    return rc;
}

QString ContactImportCommand::summary() const
{
    //: CLI command summary
    //% "Import contacts"
    return qtTrId("gikctl-command-contact-import-summary");
}

QString ContactImportCommand::description() const
{
    //: CLI command description
    //% "Import one or more contacts from a vCard file into a specific addressbook."
    return qtTrId("gikctl-command-contact-import-description");
}

ContactImportCommand::Strategy ContactImportCommand::strategyStringToEnum(const QString &str) const
{
    if (str.compare(QLatin1String("replace"), Qt::CaseInsensitive) == 0) {
        return Strategy::Replace;
    } else if (str.compare(QLatin1String("keepold"), Qt::CaseInsensitive) == 0) {
        return Strategy::KeepOld;
    } else if (str.compare(QLatin1String("keepnew"), Qt::CaseInsensitive) == 0) {
        return Strategy::KeepNew;
    } else if (str.compare(QLatin1String("add"), Qt::CaseInsensitive) == 0) {
        return Strategy::Add;
    } else {
        return Strategy::Invalid;
    }
}

QString ContactImportCommand::strategyEnumToString(Strategy strategy) const
{
    QString str;

    if (strategy != Strategy::Invalid) {
        const auto mo = ContactImportCommand::metaObject();
        const auto me = mo->enumerator(mo->indexOfEnumerator("Strategy"));

        str = QString::fromLatin1(me.valueToKey(static_cast<int>(strategy))).toLower();
    }

    return str;
}

QStringList ContactImportCommand::supportedStrategies() const
{
    QStringList lst;

    const auto mo = ContactImportCommand::metaObject();
    const auto me = mo->enumerator(mo->indexOfEnumerator("Strategy"));

    lst.reserve(me.keyCount() - 1);

    for (int i = 1; i < me.keyCount(); ++i) {
        lst.append(QString::fromLatin1(me.key(i)).toLower());
    }

    return lst;
}

#include "moc_contactimportcommand.cpp"
