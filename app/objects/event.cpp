/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "event_p.h"
#include "logging.h"
#include "error.h"
#include "../utils.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Utils/Sql>

#include <QSqlQuery>
#include <QSqlError>
#include <QMetaType>
#include <QMetaEnum>
#include <QJsonDocument>
#include <QJsonValue>
#include <QDataStream>

#define EVENT_STASH_KEY "event"

Event::Event()
{

}

Event::Event(dbid_t id, dbid_t userId, const QString &title, const QString &slug, const QDateTime &start, const QDateTime &end, const QTimeZone &tz, Audience audience, Participation participation, const QString &description, const QVariantHash &settings, bool allDay, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy, uint adultsInvited, uint adultsAccepted, uint childrenInvited, uint childrenAccepted)
    : d(new EventData)
{
    d->id = id;
    d->userId = userId;
    d->title = title;
    d->slug = slug;
    d->start = start;
    d->start.setTimeSpec(Qt::UTC);
    d->end = end;
    d->end.setTimeSpec(Qt::UTC);
    d->timezone = tz;
    d->audience = audience;
    d->participation = participation;
    d->description = description;
    d->settings = settings;
    d->allDay = allDay;
    d->created = created;
    d->created.setTimeSpec(Qt::UTC);
    d->updated = updated;
    d->updated.setTimeSpec(Qt::UTC);
    d->lockedAt = lockedAt;
    if (d->lockedAt.isValid()) {
        d->lockedAt.setTimeSpec(Qt::UTC);
    }
    d->lockedBy = lockedBy;
    d->adultsInvited = adultsInvited;
    d->adultsAccepted = adultsAccepted;
    d->childrenInvited = childrenInvited;
    d->childrenAccepted = childrenAccepted;
    d->startOnly = start == end;
}

Event::Event(dbid_t id, const User &user, const QString &title, const QString &slug, const QDateTime &start, const QDateTime &end, const QTimeZone &tz, Audience audience, Participation participation, const QString &description, const QVariantHash &settings, bool allDay, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy, uint adultsInvited, uint adultsAccepted, uint childrenInvited, uint childrenAccepted)
    : d(new EventData)
{
    d->id = id;
    d->user = user;
    d->title = title;
    d->slug = slug;
    d->start = start;
    d->start.setTimeSpec(Qt::UTC);
    d->end = end;
    d->end.setTimeSpec(Qt::UTC);
    d->timezone = tz;
    d->audience = audience;
    d->participation = participation;
    d->description = description;
    d->settings = settings;
    d->allDay = allDay;
    d->created = created;
    d->created.setTimeSpec(Qt::UTC);
    d->updated = updated;
    d->updated.setTimeSpec(Qt::UTC);
    d->lockedAt = lockedAt;
    if (d->lockedAt.isValid()) {
        d->lockedAt.setTimeSpec(Qt::UTC);
    }
    d->lockedBy = lockedBy;
    d->adultsInvited = adultsInvited;
    d->adultsAccepted = adultsAccepted;
    d->childrenInvited = childrenInvited;
    d->childrenAccepted = childrenAccepted;
    d->startOnly = start == end;
}

Event::Event(const Event &other) = default;

Event::Event(Event &&other) noexcept = default;

Event &Event::operator=(const Event &other) = default;

Event &Event::operator=(Event &&other) noexcept = default;

Event::~Event() = default;

void Event::swap(Event &other) noexcept
{
    std::swap(d, other.d);
}

dbid_t Event::id() const
{
    return d ? d->id : 0;
}

User Event::user() const
{
    if (d) {
        if (d->user.isNull() && d->userId > 0) {
            return User::get(nullptr, nullptr, d->userId);
        } else {
            return d->user;
        }
    } else {
        return User();
    }
}

QString Event::title() const
{
    return d ? d->title : QString();
}

QString Event::slug() const
{
    return d ? d->slug : QString();
}

QDateTime Event::start() const
{
    if (d) {
        if (d->timezone == QTimeZone::utc()) {
            return d->start;
        } else {
            return d->start.toTimeZone(d->timezone);
        }
    } else {
        return QDateTime();
    }
}

QDateTime Event::startUtc() const
{
    return d ? d->start : QDateTime();
}

QDateTime Event::end() const
{
    if (d) {
        if (d->timezone == QTimeZone::utc()) {
            return d->end;
        } else {
            return d->end.toTimeZone(d->timezone);
        }
    } else {
        return QDateTime();
    }
}

QDateTime Event::endUtc() const
{
    return d ? d->end : QDateTime();
}

QString Event::ianaTimezone() const
{
    return d ? QString::fromUtf8(d->timezone.id()) : QString();
}

Event::Audience Event::audience() const
{
    return d ? d->audience : InvalidAudience;
}

Event::Participation Event::participation() const
{
    return d ? d->participation : InvalidParticipation;
}

QString Event::description() const
{
    return d ? d->description : QString();
}

QVariantHash Event::settings() const
{
    return d ? d->settings : QVariantHash();
}

bool Event::isAllDay() const
{
    return d ? d->allDay : false;
}

bool Event::startTimeOnly() const
{
    return d ? d->startOnly : false;
}

QDateTime Event::created() const
{
    return d ? d->created : QDateTime();
}

QDateTime Event::updated() const
{
    return d ? d->updated : QDateTime();
}

QDateTime Event::lockedAt() const
{
    return d ? d->lockedAt : QDateTime();
}

SimpleUser Event::lockedBy() const
{
    return d ? d->lockedBy : SimpleUser();
}

uint Event::adultsInvited() const
{
    return d ? d->adultsInvited : 0;
}

uint Event::adultsAccepted() const
{
    return d ? d->adultsAccepted : 0;
}

uint Event::childrenInvited() const
{
    return d ? d->childrenInvited : 0;
}

uint Event::childrenAccepted() const
{
    return d ? d->childrenAccepted : 0;
}

uint Event::totalInvited() const
{
    if (d) {
        return d->adultsInvited + d->childrenInvited;
    } else {
        return 0;
    }
}

uint Event::totalAccepted() const
{
    if (d) {
        return d->adultsAccepted + d->childrenAccepted;
    } else {
        return 0;
    }
}

bool Event::isValid() const
{
    return d && d->id > 0;
}

bool Event::isNull() const
{
    return d ? false : true;
}

QJsonObject Event::toJson() const
{
    QJsonObject o;

    if (isNull() || !isValid()) {
        return o;
    }

    o.insert(QStringLiteral("id"), static_cast<qint64>(d->id));
    o.insert(QStringLiteral("user"), d->user.toJson());
    o.insert(QStringLiteral("title"), d->title);
    o.insert(QStringLiteral("slug"), d->slug);
    o.insert(QStringLiteral("start"), start().toString(Qt::ISODate));
    o.insert(QStringLiteral("startUtc"), d->start.toString(Qt::ISODate));
    o.insert(QStringLiteral("end"), end().toString(Qt::ISODate));
    o.insert(QStringLiteral("endUtc"), d->end.toString());
    o.insert(QStringLiteral("timezone"), ianaTimezone());
    o.insert(QStringLiteral("audience"), Event::audienceEnumToString(d->audience));
    o.insert(QStringLiteral("participation"), Event::participationEnumToString(d->participation));
    o.insert(QStringLiteral("description"), d->description);
    o.insert(QStringLiteral("settings"), QJsonObject::fromVariantHash(d->settings));
    o.insert(QStringLiteral("isAllDay"), d->allDay);
    o.insert(QStringLiteral("startTimeOnly"), d->startOnly);
    o.insert(QStringLiteral("created"), d->created.toString(Qt::ISODate));
    o.insert(QStringLiteral("updated"), d->updated.toString(Qt::ISODate));
    o.insert(QStringLiteral("lockedAt"), d->lockedAt.toString(Qt::ISODate));
    o.insert(QStringLiteral("lockedBy"), d->lockedBy.toJson());
    o.insert(QStringLiteral("adultsInvited"), static_cast<qint64>(d->adultsInvited));
    o.insert(QStringLiteral("adultsAccepted"), static_cast<qint64>(d->adultsAccepted));
    o.insert(QStringLiteral("childrenInvited"), static_cast<qint64>(d->childrenInvited));
    o.insert(QStringLiteral("childrenAccepted"), static_cast<qint64>(d->childrenAccepted));
    o.insert(QStringLiteral("totalInvited"), static_cast<qint64>(totalInvited()));
    o.insert(QStringLiteral("totalAccepted"), static_cast<qint64>(totalAccepted()));

    return o;
}

bool Event::toStash(Cutelyst::Context *c) const
{
    Q_ASSERT_X(c, "event to stash", "invalid context pointer");

    if (Q_LIKELY(isValid())) {
        c->stash({
                     {QStringLiteral(EVENT_STASH_KEY), QVariant::fromValue<Event>(*this)},
                     {QStringLiteral("site_title"), title()}
                 });
        return true;
    } else {
        c->res()->setStatus(404);
        c->detach(c->getAction(QStringLiteral("error")));
        return false;
    }
}

bool Event::toStash(Cutelyst::Context *c, dbid_t eventId)
{
    Q_ASSERT_X(c, "event to stash", "invalid context pointer");

    Error error;
    const auto event = Event::get(c, &error, eventId);
    if (Q_LIKELY(error.type() == Error::NoError)) {
        return event.toStash(c);
    } else {
        error.toStash(c, true);
        return false;
    }
}

Event Event::fromStash(Cutelyst::Context *c)
{
    return c->stash(QStringLiteral(EVENT_STASH_KEY)).value<Event>();
}

Event::Audience Event::audienceStringToEnum(const QString &str)
{
    if (str.compare(QLatin1String("private"), Qt::CaseInsensitive) == 0) {
        return Event::Private;
    } else if (str.compare(QLatin1String("public"), Qt::CaseInsensitive) == 0) {
        return Event::Public;
    } else {
        return Event::InvalidAudience;
    }
}

QString Event::audienceEnumToString(Event::Audience audience)
{
    QString str;

    if (audience != Event::InvalidAudience) {
        const QMetaObject mo = Event::staticMetaObject;
        const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Audience"));

        str = QString::fromLatin1(me.valueToKey(static_cast<int>(audience)));
    }

    return str;
}

QStringList Event::supportedAudiences()
{
    QStringList lst;

    const QMetaObject mo = Event::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Audience"));

    lst.reserve(me.keyCount() - 1);

    for (int i = 1; i < me.keyCount(); ++i) {
        lst.append(QString::fromLatin1(me.key(i)));
    }

    return lst;
}

Event::Participation Event::participationStringToEnum(const QString &str)
{
    if (str.compare(QLatin1String("refusal"), Qt::CaseInsensitive) == 0) {
        return Event::Refusal;
    } else if (str.compare(QLatin1String("acceptance"), Qt::CaseInsensitive) == 0) {
        return Event::Acceptance;
    } else {
        return Event::InvalidParticipation;
    }
}

QString Event::participationEnumToString(Event::Participation participation)
{
    QString str;

    if (participation != Event::InvalidParticipation) {
        const QMetaObject mo = Event::staticMetaObject;
        const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Participation"));

        str = QString::fromLatin1(me.valueToKey(static_cast<int>(participation)));
    }

    return str;
}

QStringList Event::supportedParticipations()
{
    QStringList lst;

    const QMetaObject mo = Event::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Participation"));

    lst.reserve(me.keyCount() - 1);

    for (int i = 1; i < me.keyCount(); ++i) {
        lst.append(QString::fromLatin1(me.key(i)));
    }

    return lst;
}

std::vector<Event> Event::list(Cutelyst::Context *c, Error *e, dbid_t userId)
{
    std::vector<Event> events;

    User user = User::fromStash(c);
    if (user.id() != userId) {
        user = User::get(c, e, userId);
        if (e && e->type() != Error::NoError) {
            return events;
        }
    }
    events = Event::list(c, e, user);

    return events;
}

std::vector<Event> Event::list(Cutelyst::Context *c, Error *e, const User &user)
{
    std::vector<Event> events;

    QSqlQuery q= CPreparedSqlQueryThreadFO(QStringLiteral("SELECT "
                                                          "e.id, e.title, e.slug, e.start_time, e.end_time, e.timezone, e.audience, e.participation, e.description, e.settings, e.all_day, e.created_at, e.updated_at, e.locked_at,  "
                                                          "u.id AS locked_by_id, u.username AS locked_by_username "
                                                          "FROM events e LEFT JOIN users u ON u.id = e.locked_by WHERE e.user_id = :user_id"));
             q.bindValue(QStringLiteral(":user_id"), user.id());

    if (Q_LIKELY(q.exec())) {
        if (q.size() > 0) {
            events.reserve(q.size());
        }

        while (q.next()) {

            const dbid_t eventId = q.value(0).toUInt();

            QSqlQuery q2 = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT SUM(g.adults), SUM(g.adults_accepted), SUM(g.children), SUM(g.children_accepted) FROM guests g JOIN guestgroups gg ON g.group_id = gg.id WHERE gg.event_id = :event_id"));
            q2.bindValue(QStringLiteral(":event_id"), eventId);

            uint adults, adultsAccepted, children, childrenAccepted;
            if (q2.exec() && q2.next()) {
                adults              = q2.value(0).toUInt();
                adultsAccepted      = q2.value(1).toUInt();
                children            = q2.value(2).toUInt();
                childrenAccepted    = q2.value(3).toUInt();
            } else {
                qCWarning(GIK_CORE) << "Failed to query guest counts for event ID" << eventId << "from the database:" << q2.lastError().text();
            }

            const QVariantHash settings = Utils::settingsHashFromString(q.value(9).toString());

            const qlonglong  lat        = q.value(13).toLongLong();
            const QDateTime  lockedAt   = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy   = lat > 0 ? SimpleUser(q.value(14).toUInt(), q.value(15).toString()) : SimpleUser();

            events.emplace_back(eventId,
                                user,
                                q.value(1).toString(),
                                q.value(2).toString(),
                                q.value(3).toDateTime(),
                                q.value(4).toDateTime(),
                                QTimeZone(q.value(5).toString().toLatin1()),
                                static_cast<Event::Audience>(q.value(6).value<qint8>()),
                                static_cast<Event::Participation>(q.value(7).value<qint8>()),
                                q.value(8).toString(),
                                settings,
                                q.value(10).toBool(),
                                q.value(11).toDateTime(),
                                q.value(12).toDateTime(),
                                lockedAt,
                                lockedBy,
                                adults,
                                adultsAccepted,
                                children,
                                childrenAccepted);
        }

    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("Event", "Failed to query events from the database."));
        qCCritical(GIK_CORE) << "Failed to query list of events for user id" << user.id() << "from database:" << q.lastError().text();
    }

    return events;
}

Event Event::get(Cutelyst::Context *c, Error *e, dbid_t eventId)
{
    Event event;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT "
                                                           "e.id, e.user_id, e.title, e.slug, e.start_time, e.end_time, e.timezone, e.audience, e.participation, e.description, e.settings, e.all_day, e.created_at, e.updated_at, e.locked_at, "
                                                           "l.id AS locked_by_id, l.username AS locked_by_username "
                                                           "FROM events e LEFT JOIN users l ON l.id = e.locked_by WHERE e.id = :id"));
    q.bindValue(QStringLiteral(":id"), eventId);

    if (Q_LIKELY(q.exec())) {
        if (Q_LIKELY(q.next())) {

            QSqlQuery q2 = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT SUM(g.adults), SUM(g.adults_accepted), SUM(g.children), SUM(g.children_accepted) FROM guests g JOIN guestgroups gg ON g.group_id = gg.id WHERE gg.event_id = :event_id"));
            q2.bindValue(QStringLiteral(":event_id"), eventId);

            uint adults = 0, adultsAccepted = 0, children = 0, childrenAccepted = 0;
            if (q2.exec() && q2.next()) {
                adults              = q2.value(0).toUInt();
                adultsAccepted      = q2.value(1).toUInt();
                children            = q2.value(2).toUInt();
                childrenAccepted    = q2.value(3).toUInt();
            } else {
                qCWarning(GIK_CORE) << "Failed to query guest counts for event ID" << eventId << "from the database:" << q.lastError().text();
            }

            const QVariantHash settings = Utils::settingsHashFromString(q.value(10).toString());

            const qlonglong  lat        = q.value(14).toLongLong();
            const QDateTime  lockedAt   = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy   = lat > 0 ? SimpleUser(q.value(15).toUInt(), q.value(16).toString()) : SimpleUser();

            event = Event(q.value(0).toUInt(),
                          q.value(1).toUInt(),
                          q.value(2).toString(),
                          q.value(3).toString(),
                          q.value(4).toDateTime(),
                          q.value(5).toDateTime(),
                          QTimeZone(q.value(6).toString().toLatin1()),
                          static_cast<Event::Audience>(q.value(7).value<qint8>()),
                          static_cast<Event::Participation>(q.value(8).value<qint8>()),
                          q.value(9).toString(),
                          settings,
                          q.value(11).toBool(),
                          q.value(12).toDateTime(),
                          q.value(13).toDateTime(),
                          lockedAt,
                          lockedBy,
                          adults,
                          adultsAccepted,
                          children,
                          childrenAccepted);

        } else {
            if (c && e ) *e = Error(Error::NotFound, c->translate("Event", "Can not find event with ID %1").arg(eventId));
            qCWarning(GIK_CORE) << "Can not find event with ID" << eventId << "in the database";
        }
    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("Event", "Failed to get event with ID %1 from the database.").arg(eventId));
        qCCritical(GIK_CORE) << "Failed to get event with ID" << eventId << "from the database:" << q.lastError().text();
    }

    return event;
}

QDataStream &operator<<(QDataStream &stream, const Event &event)
{
    stream << event.d->user
           << event.d->title
           << event.d->slug
           << event.d->description
           << event.d->start
           << event.d->end
           << event.d->created
           << event.d->updated
           << event.d->lockedAt
           << event.d->lockedBy
           << event.d->settings
           << event.d->timezone
           << event.d->id
           << static_cast<qint32>(event.d->audience)
           << static_cast<qint32>(event.d->participation)
           << event.d->allDay
           << event.d->startOnly;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Event &event)
{
    if (event.d == nullptr) {
        event.d = new EventData;
    }

    stream >> event.d->user;
    stream >> event.d->title;
    stream >> event.d->slug;
    stream >> event.d->description;
    stream >> event.d->start;
    stream >> event.d->end;
    stream >> event.d->created;
    stream >> event.d->updated;
    stream >> event.d->lockedAt;
    stream >> event.d->lockedBy;
    stream >> event.d->settings;
    stream >> event.d->timezone;
    stream >> event.d->id;
    qint32 audienceInt;
    stream >> audienceInt;
    event.d->audience = static_cast<Event::Audience>(audienceInt);
    qint32 participationInt;
    stream >> participationInt;
    event.d->participation = static_cast<Event::Participation>(participationInt);
    stream >> event.d->allDay;
    stream >> event.d->startOnly;
    return stream;

}

#include "moc_event.cpp"
