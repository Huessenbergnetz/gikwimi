/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guestexportcommand.h"

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QTextStream>
#include <QUrl>

#define DBCONNAME "guestmanagement"

using namespace Qt::StringLiterals;

GuestExportCommand::GuestExportCommand(QObject *parent)
    : Command{parent}
{
    setObjectName("export");
}

GuestExportCommand::~GuestExportCommand() = default;

void GuestExportCommand::init()
{
    m_cliOptions.append(QCommandLineOption({u"e"_s, u"event"_s},
                                           //: CLI option description
                                           //% "The DB id of the event the guests to export belong to."
                                           qtTrId("gikctl-opt-guest-export-event-desc"),
                                           //: CLI option name value
                                           //% "db id"
                                           qtTrId("gikctl-opt-guest-export-event-value")));

    m_cliOptions.append(QCommandLineOption({u"f"_s, u"file"_s},
                                           u"Path to the file the exported data should be written to."_s,
                                           u"file"_s));

    m_cliOptions.append(QCommandLineOption({u"u"_s, u"url"_s},
                                           u"URL of the website website gikwimi runs on."_s,
                                           u"url"_s));
}

int GuestExportCommand::exec(QCommandLineParser *parser)
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

    printStatus(qtTrId("Exporting guest list"));

    const QString eventIdStr = parser->value(u"event"_s);
    if (Q_UNLIKELY(eventIdStr.isEmpty())) {
        printFailed();
        return inputError(u"Can not export guest list without event ID."_s);
    }

    bool eventIdOk = false;
    const int eventId = parser->value(u"event"_s).toInt(&eventIdOk);

    if (Q_UNLIKELY(!eventIdOk)) {
        printFailed();
        return inputError(u"Invalid event ID"_s);
    }

    const QString urlStr = parser->value(u"url"_s);
    if (Q_UNLIKELY(urlStr.isEmpty())) {
        printFailed();
        return inputError(u"Please set the URL of the website gikwimi runs on."_s);
    }
    const QUrl url(urlStr);
    if (Q_UNLIKELY(!url.isValid())) {
        printFailed();
        return inputError(u"Invalid website URL."_s);
    }

    const QString filename = parser->value(u"file"_s);
    if (Q_UNLIKELY(filename.isEmpty())) {
        printFailed();
        return inputError(u"Please set a path to a file to export to using the -f switch."_s);
    }

    auto file = new QFile(filename, this);

    if (!file->open(QIODeviceBase::WriteOnly|QIODeviceBase::Truncate|QIODeviceBase::Text, QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ReadGroup|QFileDevice::ReadOther)) {
        printFailed();
        return fileError(u"Failed to open file for writing: %1"_s.arg(file->errorString()));
    }

    QSqlDatabase db = QSqlDatabase::database(QStringLiteral(DBCONNAME));

    QSqlQuery q(db);
    q.setForwardOnly(true);

    if (Q_UNLIKELY(!q.prepare(u"SELECT g.uid, c.given_name, c.family_name, g.partner_given_name, g.partner_family_name, g.adults, g.children "
                              "FROM guestgroups gg JOIN guests g ON gg.id = g.group_id JOIN contacts c ON c.id = g.contact_id WHERE gg.event_id = :eventId"_s))) {
        printFailed();
        return dbError(q);
    }

    q.bindValue(u":eventId"_s, eventId);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        return dbError(q);
    }

    if (q.size() == 0) {
        printDone();
        printWarning(u"Query returned zero results."_s);
        return rc;
    }

    QTextStream out(file);
    const QStringList header({u"Anrede"_s, u"Plural"_s, u"URL"_s, u"UID"_s});
    out << header.join(';'_L1) << Qt::endl;

    while(q.next()) {
        const auto uid = q.value(0).toString();
        QUrl curUrl;
        curUrl.setScheme(url.scheme());
        curUrl.setHost(url.host());
        curUrl.setPath("/i/"_L1 + uid);

        QString salutation = u"Hallo "_s + q.value(1).toString();
        const QString partnerName = q.value(3).toString();
        if (!partnerName.isEmpty()) {
            salutation += u" und "_s + partnerName;
        }

        const bool plural = q.value(5).toInt() > 1;

        const QStringList line({salutation,
                                plural ? u"Ja"_s : u"Nein"_s,
                                curUrl.toString(),
                                uid});
        out << line.join(';'_L1) << Qt::endl;
    }

    printDone();

    return rc;
}

QString GuestExportCommand::summary() const
{
    //: CLI command summary
    //% "Export guest list"
    return qtTrId("gikctl-command-guest-export-summary");
}

QString GuestExportCommand::description() const
{
    //: CLI command description
    //% "Exports the guest list into different formats"
    return qtTrId("gikctl-command-guest-export-description");
}

#include "moc_guestexportcommand.cpp"
