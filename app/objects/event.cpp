/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "event_p.h"
#include "logging.h"
#include "error.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Utils/Sql>

#include <QSqlQuery>
#include <QSqlError>
#include <QMetaType>
#include <QMetaEnum>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDataStream>

#define EVENT_STASH_KEY "event"

Event::Event()
{

}

Event::Event(dbid_t id, const User &user, const QString &title, const QString &slug, const QDateTime &start, const QDateTime &end, const QTimeZone &tz, Audience audience, Participation participation, const QString &description, const QVariantHash settings, bool allDay, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy)
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
    return d ? d->user : User();
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

bool Event::isValid() const
{
    return d && d->id > 0;
}

bool Event::isNull() const
{
    return d ? false : true;
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
    if (Q_LIKELY(error.type() != Error::NoError)) {
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

    QSqlQuery q;
    if (user.isNull()) {
        q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT "
                                                     "e.id, e.title, e.slug, e.start_time, e.end_time, e.timezone, e.audience, e.participation, e.description, e.settings, e.all_day, e.created_at, e.updated_at, e.locked_at, "
                                                     "l.id AS locked_by_id, l.username AS locked_by_username, "
                                                     "u.id AS user_id, u.type, u.username, u.email, u.created_at AS user_created, u.updated_at AS user_updated "
                                                     "FROM events e LEFT JOIN users l ON l.id = e.locked_by JOIN users u ON u.id = e.user_id"));
    } else {
        q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT "
                                                     "e.id, e.title, e.slug, e.start_time, e.end_time, e.timezone, e.audience, e.participation, e.description, e.settings, e.all_day, e.created_at, e.updated_at, e.locked_at,  "
                                                     "u.id AS locked_by_id, u.username AS locked_by_username "
                                                     "FROM events e LEFT JOIN users u ON u.id = e.locked_by WHERE e.user_id = :user_id"));
        q.bindValue(QStringLiteral(":user_id"), user.id());
    }

    if (Q_LIKELY(q.exec())) {
        if (q.size() > 0) {
            events.reserve(q.size());
        }

        while (q.next()) {
            const User _user = user.isNull()
                    ? User(q.value(16).toUInt(),
                           static_cast<User::Type>(q.value(17).value<qint8>()),
                           q.value(18).toString(),
                           q.value(19).toString(),
                           q.value(20).toDateTime(),
                           q.value(21).toDateTime())
                    : user;

            QVariantHash settings;
            const QString settingsString = q.value(9).toString();
            if (!settingsString.isEmpty()) {
                const QJsonDocument settingsJson = QJsonDocument::fromJson(settingsString.toUtf8());
                const QJsonObject settingsObject = settingsJson.object();
                settings = settingsObject.toVariantHash();
            }

            const qlonglong lat         = q.value(13).toLongLong();
            const QDateTime lockedAt    = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy   = lat > 0 ? SimpleUser(q.value(14).toUInt(), q.value(15).toString()) : SimpleUser();

            events.emplace_back(q.value(0).toUInt(),
                                _user,
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
                                lockedBy);
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
                                                           "e.id, e.title, e.slug, e.start_time, e.end_time, e.timezone, e.audience, e.participation, e.description, e.settings, e.all_day, e.created_at, e.updated_at, e.locked_at, "
                                                           "l.id AS locked_by_id, l.username AS locked_by_username, "
                                                           "u.id AS user_id, u.type, u.username, u.email, u.created_at AS user_created, u.updated_at AS user_updated "
                                                           "FROM events e LEFT JOIN users l ON l.id = e.locked_by JOIN users u ON u.id = e.user_id WHERE e.id = :id"));
    q.bindValue(QStringLiteral(":id"), eventId);

    if (Q_LIKELY(q.exec())) {
        if (Q_LIKELY(q.next())) {

            QVariantHash settings;
            const QString settingsString = q.value(9).toString();
            if (!settingsString.isEmpty()) {
                const QJsonDocument settingsJson = QJsonDocument::fromJson(settingsString.toUtf8());
                const QJsonObject settingsObject = settingsJson.object();
                settings = settingsObject.toVariantHash();
            }

            const qlonglong lat         = q.value(13).toLongLong();
            const QDateTime lockedAt    = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy   = lat > 0 ? SimpleUser(q.value(14).toUInt(), q.value(15).toString()) : SimpleUser();

            const User user(q.value(16).toUInt(),
                            static_cast<User::Type>(q.value(17).value<qint8>()),
                            q.value(18).toString(),
                            q.value(19).toString(),
                            q.value(20).toDateTime(),
                            q.value(21).toDateTime());

            event = Event(q.value(0).toUInt(),
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
                          lockedBy);

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
           << event.d->audience
           << event.d->participation
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
    stream >> event.d->audience;
    stream >> event.d->participation;
    stream >> event.d->allDay;
    stream >> event.d->startOnly;
    return stream;

}

#include "moc_event.cpp"
