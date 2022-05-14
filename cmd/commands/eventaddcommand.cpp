/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "eventaddcommand.h"

#include "../../app/objects/event.h"
#include "../../app/objects/guestgroup.h"
#include "../../app/utils.h"

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QLocale>
#include <QTimeZone>

#define DBCONNAME "eventmanagement"

EventAddCommand::EventAddCommand(QObject *parent)
    : Command{parent}
{
    setObjectName(QStringLiteral("add"));
}

EventAddCommand::~EventAddCommand() = default;

void EventAddCommand::init()
{
    QLocale locale;

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("t"), QStringLiteral("title")}),
                                               //: CLI option description
                                               //% "The title for the event to create."
                                               qtTrId("gikctl-opt-event-add-title-desc"),
                                               //: CLI option value name
                                               //% "name"
                                               qtTrId("gikctl-opt-event-add-title-value")));

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("u"), QStringLiteral("user")}),
                                           //: CLI option description
                                           //% "The user name, email or ID of the user that will be the owner of the event."
                                           qtTrId("gikctl-opt-event-add-user-desc"),
                                           //: CLI option value name
                                           //% "user"
                                           qtTrId("gikctl-opt-event-add-user-value")));

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("s"), QStringLiteral("slug")}),
                                           //: CLI option description
                                           //% "The slug used in the event’s URL."
                                           qtTrId("gikctl-opt-event-add-slug-desc"),
                                           //: CLI option value name
                                           //% "slug"
                                           qtTrId("gikctl-opt-event-add-slug-value")));

    m_cliOptions.append(QCommandLineOption(QStringLiteral("start"),
                                           //: CLI option description
                                           //% "The date and time the event starts."
                                           qtTrId("gikctl-opt-event-add-start-desc"),
                                           //: CLI option value name
                                           //% "yyyy-mm-ddThh:mm:ss"
                                           qtTrId("gikctl-opt-event-add-time-value")));

    m_cliOptions.append(QCommandLineOption(QStringLiteral("end"),
                                           //: CLI option description
                                           //% "The date and time the event ends. If omitted or if it is the same as start, no end time is given."
                                           qtTrId("gikctl-opt-event-add-end-desc"),
                                           qtTrId("gikctl-opt-event-add-time-value")));

    const QString tzDefVal = QString::fromLatin1(QTimeZone::systemTimeZoneId());
    m_cliOptions.append(QCommandLineOption(QStringLiteral("tz"),
                                           //: CLI option description
                                           //% "The time zone in which the event takes place. Use --tz-list for a list of available time zones. Default: %1."
                                           qtTrId("gikctl-opt-event-add-tz-desc").arg(tzDefVal),
                                           QStringLiteral("IANA ID"),
                                           tzDefVal));

    m_cliOptions.append(QCommandLineOption(QStringLiteral("tz-list"),
                                           //: CLI option description
                                           //% "Show a list of all available IANA time zone IDs."
                                           qtTrId("gikctl-opt-event-tz-list-desc")));

    const QString audienceDefVal = Event::audienceEnumToString(Event::Private);
    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("a"), QStringLiteral("audience")}),
                                           //: CLI option description, %1 will be replaced by list of available values, %2 by the default value
                                           //% "The audience the event is intendet for. Currently supported values: %1. Default value: %2."
                                           qtTrId("gikctl-opt-event-add-audience-desc").arg(locale.createSeparatedList(Event::supportedAudiences()), audienceDefVal),
                                           //: CLI option value name
                                           //% "audience"
                                           qtTrId("gikctl-opt-event-add-audience-value"),
                                           audienceDefVal));

    const QString partiDefVal = Event::participationEnumToString(Event::Refusal);
    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("p"), QStringLiteral("participation")}),
                                           //: CLI option description, %1 will be replaced by list of available values, %2 by the default value
                                           //% "The participation strategy used by the event. Currently supported values: %1. Default value: %2."
                                           qtTrId("gikctl-opt-event-add-participation-desc").arg(locale.createSeparatedList(Event::supportedParticipations()), partiDefVal),
                                           //: CLI option value name
                                           //% "participation"
                                           qtTrId("gikctl-opt-event-add-participation-value"),
                                           partiDefVal));

    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("d"), QStringLiteral("description")}),
                                           //: CLI option description
                                           //% "The description for ths event."
                                           qtTrId("gikctl-opt-event-add-description-desc"),
                                           //: CLI option value name
                                           //% "text"
                                           qtTrId("gikctl-opt-event-add-description-value")));

    m_cliOptions.append(QCommandLineOption(QStringLiteral("all-day"),
                                           //: CLI option description
                                           //% "The event is all day."
                                           qtTrId("gikctl-opt-event-add-all-day-desc")));

    //: default value for standard event guest group that will be created together with the event
    //% "default"
    const QString groupDefVal = qtTrId("gikctl-opt-event-add-group-defval");
    m_cliOptions.append(QCommandLineOption(QStringList({QStringLiteral("g"), QStringLiteral("group")}),
                                           //: CLI option description
                                           //% "Default group to create for the event. Default value: %1"
                                           qtTrId("gikctl-opt-event-add-group-desc").arg(groupDefVal),
                                           //: CLI option value name
                                           //% "group name"
                                           qtTrId("gikctl-opt-event-add-group-value"),
                                           groupDefVal));

    const QString salutDefVal = GuestGroup::salutationEnumToString(GuestGroup::SalutationNeutral);
    m_cliOptions.append(QCommandLineOption(QStringLiteral("salutation"),
                                           //: CLI option description
                                           //% "Salutation used for guests in the default group. Can be changed later. Available forms of salutation: %1. Default value: %2"
                                           qtTrId("gikctl-opt-event-add-salutation-desc").arg(locale.createSeparatedList(GuestGroup::salutationKeys(false)), salutDefVal),
                                           //: CLI option value name
                                           //% "salutation"
                                           qtTrId("gikctl-opt-event-add-salutation-value"),
                                           salutDefVal));
}

int EventAddCommand::exec(QCommandLineParser *parser)
{
    int rc = 0;

    parser->addOptions(m_cliOptions);
    parser->parse(QCoreApplication::arguments());

    if (checkShowHelp(parser)) {
        return rc;
    }

    setGlobalOptions(parser);

    if (parser->isSet(QStringLiteral("tz-list"))) {
        //% "Available IANA time zone IDs:"
        printMessage(qtTrId("gikctl-event-show-timezones"));

        const auto tzList = QTimeZone::availableTimeZoneIds();
        for (const QByteArray &tz : tzList) {
            printMessage(tz);
        }

        return rc;
    }

    rc = openDb(QStringLiteral(DBCONNAME));
    if (rc != 0) {
        return rc;
    }

    const QString title                         = parser->value(QStringLiteral("title")).trimmed();
    const QString user                          = parser->value(QStringLiteral("user")).trimmed();
    QString slug                                = parser->value(QStringLiteral("slug")).trimmed();
    const QString start                         = parser->value(QStringLiteral("start")).trimmed();
    const QString end                           = parser->value(QStringLiteral("end")).trimmed();
    const QString tz                            = parser->value(QStringLiteral("tz")).trimmed();
    const Event::Audience audience              = Event::audienceStringToEnum(parser->value(QStringLiteral("audience")).trimmed());
    const Event::Participation participation    = Event::participationStringToEnum(parser->value(QStringLiteral("participation")).trimmed());
    const QString description                   = parser->value(QStringLiteral("description")).trimmed();
    const bool allDay                           = parser->isSet(QStringLiteral("all-day"));
    const QString group                         = parser->value(QStringLiteral("group")).trimmed();
    const GuestGroup::Salutation salutation     = GuestGroup::salutationStringToEnum(parser->value(QStringLiteral("salutation")).trimmed());

    //% "Adding new event “%1”"
    printStatus(qtTrId("gikctl-status-add-event").arg(title));

    if (title.isEmpty()) {
        printFailed();
        //: CLI error message
        //% "Can not add a new event with empty title"
        return inputError(qtTrId("gikctl-event-add-err-empty-title"));
    }

    if (user.isEmpty()) {
        printFailed();
        //: CLI error message
        //% "Can not add a new event without valid user"
        return inputError(qtTrId("gikctl-event-add-err-empty-user"));
    }

    if (slug.isEmpty()) {
        slug = Utils::createSlug(title);
    }

    if (start.isEmpty()) {
        printFailed();
        //: CLI error message
        //% "Can not add a new event without valid start time"
        return inputError(qtTrId("gikctl-event-add-err-empty-start"));
    }

    const QTimeZone timezone(tz.toLatin1());
    if (!timezone.isValid()) {
        printFailed();
        //% "Can not add a new event with invalid time zone"
        return inputError(qtTrId("gikctl-event-add-err-invalid-tz"));
    }

    auto _startTime = QDateTime::fromString(start, Qt::ISODate);
    _startTime.setTimeZone(timezone);
    const QDateTime startTime = _startTime.toUTC();
    if (!startTime.isValid()) {
        printFailed();
        return inputError(qtTrId("gikctl-event-add-err-empty-start"));
    }

    const QDateTime endTime = ([end, startTime, timezone]() -> QDateTime {
        if (end.isEmpty()) {
            return startTime;
        } else {
            auto _endTime = QDateTime::fromString(end, Qt::ISODate);
            _endTime.setTimeZone(timezone);
            return _endTime.toUTC();
        }
    }());
    if (!endTime.isValid()) {
        printFailed();
        //% "Can not add a new event with invalid end time"
        return inputError(qtTrId("gikctl-event-add-err-invalid-end"));
    }

    if (audience == Event::InvalidAudience) {
        printFailed();
        QLocale locale;
        //: CLI error message
        //% "Can not add a new event with invalid audience type “%1”. Currently supported types are: %2"
        return inputError(qtTrId("gikctl-event-add-err-invalid-audience").arg(parser->value(QStringLiteral("audience")).trimmed(), locale.createSeparatedList(Event::supportedAudiences())));
    }

    if (participation == Event::InvalidParticipation) {
        printFailed();
        QLocale locale;
        //: CLI error message
        //% "Can not add a new event with invalid participation type “%1”. Currently supported types are: %2"
        return inputError(qtTrId("gikctl-event-add-err-invalid-participation").arg(parser->value(QStringLiteral("participation")).trimmed(), locale.createSeparatedList(Event::supportedParticipations())));
    }

    if (salutation == GuestGroup::SalutationInvalid || salutation == GuestGroup::SalutationDefault) {
        printFailed();
        QLocale locale;
        //: CLI error message
        //% "Can not add a new event with invalid default group salutation form “%1”. Currently supported forms are: %2"
        return inputError(qtTrId("gikctl-event-add-err-invalid-salutation").arg(parser->value(QStringLiteral("salutation")).trimmed(), locale.createSeparatedList(GuestGroup::salutationKeys(false))));
    }

    const QDateTime now = QDateTime::currentDateTimeUtc();

    QSqlDatabase db = QSqlDatabase::database(QStringLiteral(DBCONNAME));

    if (Q_UNLIKELY(!db.transaction())) {
        printFailed();
        return dbError(db);
    }

    QSqlQuery q(db);

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
            //% "Can not find user “%1”"
            return inputError(qtTrId("gikctl-err-add-event-user-not-found").arg(user));
        }
        userId = q.value(0).toUInt();
    }

    if (Q_UNLIKELY(!q.prepare(QStringLiteral("INSERT INTO events (user_id, title, slug, start_time, end_time, timezone, audience, participation, all_day, description, created_at, updated_at) "
                                             "VALUES (:user_id, :title, :slug, :start_time, :end_time, :timezone, :audience, :participation, :all_day, :description, :created_at, :updated_at)")))) {
        printFailed();
        return dbError(q);
    }
    q.bindValue(QStringLiteral(":user_id"), userId);
    q.bindValue(QStringLiteral(":title"), title);
    q.bindValue(QStringLiteral(":slug"), slug);
    q.bindValue(QStringLiteral(":start_time"), startTime);
    q.bindValue(QStringLiteral(":end_time"), endTime);
    q.bindValue(QStringLiteral(":timezone"), timezone.id());
    q.bindValue(QStringLiteral(":audience"), static_cast<qint8>(audience));
    q.bindValue(QStringLiteral(":participation"), static_cast<qint8>(participation));
    q.bindValue(QStringLiteral(":all_day"), allDay);
    q.bindValue(QStringLiteral(":description"), description);
    q.bindValue(QStringLiteral(":created_at"), now);
    q.bindValue(QStringLiteral(":updated_at"), now);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        return dbError(q);
    }

    const dbid_t eventId = q.lastInsertId().toUInt();

    if (Q_UNLIKELY(!q.prepare(QStringLiteral("INSERT INTO guestgroups (event_id, name, slug, salutation, created_at, updated_at) VALUES (:event_id, :name, :slug, :salutation, :created_at, :updated_at)")))) {
        printFailed();
        return dbError(q);
    }
    q.bindValue(QStringLiteral(":event_id"), eventId);
    q.bindValue(QStringLiteral(":name"), group);
    q.bindValue(QStringLiteral(":slug"), Utils::createSlug(group));
    q.bindValue(QStringLiteral(":salutation"), static_cast<int>(salutation));
    q.bindValue(QStringLiteral(":created_at"), now);
    q.bindValue(QStringLiteral(":updated_at"), now);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        return dbError(q);
    }

    const dbid_t groupId = q.lastInsertId().toUInt();

    if (Q_UNLIKELY(!q.prepare(QStringLiteral("UPDATE events SET default_group = :default_group WHERE id = :id")))) {
        printFailed();
        return dbError(q);
    }
    q.bindValue(QStringLiteral(":default_group"), groupId);
    q.bindValue(QStringLiteral(":id"), eventId);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        return dbError(q);
    }

    if (Q_UNLIKELY(!db.commit())) {
        printFailed();
        return dbError(db);
    }

    printDone();

    return rc;
}

QString EventAddCommand::summary() const
{
    //: CLI command summary
    //% "Add a new event"
    return qtTrId("gikctl-command-event-add-summary");
}

QString EventAddCommand::description() const
{
    //: CLI command description
    //% "Add a new event for a specific user."
    return qtTrId("gikctl-command-event-add-description");
}

#include "moc_eventaddcommand.cpp"
