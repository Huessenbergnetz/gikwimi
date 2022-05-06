/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guestgroup.h"
#include "../logging.h"
#include "error.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Utils/Sql>

#include <QDataStream>
#include <QSqlQuery>
#include <QSqlError>

#define GUESTGROUP_STASH_KEY "guestgroup"

class GuestGroupData : public QSharedData
{
public:
    Event event;
    QString name;
    QString slug;
    QString note;
    QDateTime created;
    QDateTime updated;
    QDateTime lockedAt;
    SimpleUser lockedBy;
    dbid_t id = 0;
    dbid_t eventId = 0;
};

GuestGroup::GuestGroup()
{

}

GuestGroup::GuestGroup(dbid_t id, dbid_t eventId, const QString &name, const QString &slug, const QString &note, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new GuestGroupData)
{
    d->id = id;
    d->eventId = eventId;
    d->name = name;
    d->slug = slug;
    d->note = note;
    d->created = created;
    d->created.setTimeSpec(Qt::UTC);
    d->updated = updated;
    d->updated.setTimeSpec(Qt::UTC);
    d->lockedAt = lockedAt;
    if (d->lockedAt.isValid()) {
        d->lockedAt.setTimeSpec(Qt::UTC);
    }
    d->lockedBy = lockedBy;
}

GuestGroup::GuestGroup(dbid_t id, const Event &event, const QString &name, const QString &slug, const QString &note, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new GuestGroupData)
{
    d->id = id;
    d->event = event;
    d->name = name;
    d->slug = slug;
    d->note = note;
    d->created = created;
    d->created.setTimeSpec(Qt::UTC);
    d->updated = updated;
    d->updated.setTimeSpec(Qt::UTC);
    d->lockedAt = lockedAt;
    if (d->lockedAt.isValid()) {
        d->lockedAt.setTimeSpec(Qt::UTC);
    }
    d->lockedBy = lockedBy;
}

GuestGroup::GuestGroup(const GuestGroup &other) = default;

GuestGroup::GuestGroup(GuestGroup &&other) noexcept = default;

GuestGroup &GuestGroup::operator=(const GuestGroup &other) = default;

GuestGroup &GuestGroup::operator=(GuestGroup &&other) noexcept = default;

GuestGroup::~GuestGroup() = default;

void GuestGroup::swap(GuestGroup &other) noexcept
{
    std::swap(d, other.d);
}

dbid_t GuestGroup::id() const
{
    return d ? d->id : 0;
}

Event GuestGroup::event() const
{
    if (d) {
        if (d->event.isNull() && d->eventId > 0) {
            return Event::get(nullptr, nullptr, d->eventId);
        } else {
            return d->event;
        }
    } else {
        return Event();
    }
}

QString GuestGroup::name() const
{
    return d ? d->name : QString();
}

QString GuestGroup::slug() const
{
    return d ? d->slug : QString();
}

QString GuestGroup::note() const
{
    return d ? d->note : QString();
}

QDateTime GuestGroup::created() const
{
    return d ? d->created : QDateTime();
}

QDateTime GuestGroup::updated() const
{
    return d ? d->updated : QDateTime();
}

QDateTime GuestGroup::lockedAt() const
{
    return d ? d->lockedAt : QDateTime();
}

SimpleUser GuestGroup::lockedBy() const
{
    return d ? d->lockedBy : SimpleUser();
}

bool GuestGroup::isValid() const
{
    return d && d->id > 0;
}

bool GuestGroup::isNull() const
{
    return d ? false : true;
}

bool GuestGroup::toStash(Cutelyst::Context *c) const
{
    Q_ASSERT_X(c, "guestgroup to stash", "invalid context pointer");

    if (Q_LIKELY(isValid())) {
        c->stash({
                     {QStringLiteral(GUESTGROUP_STASH_KEY), QVariant::fromValue<GuestGroup>(*this)},
                     {QStringLiteral("site_title"), name()}
                 });
        return true;
    } else {
        c->res()->setStatus(404);
        c->detach(c->getAction(QStringLiteral("error")));
        return false;
    }
}

bool GuestGroup::toStash(Cutelyst::Context *c, dbid_t groupId)
{
    Q_ASSERT_X(c, "guestgroup to stash", "invalid context pointer");

    Error e;
    const auto group = GuestGroup::get(c, &e, groupId);
    if (Q_LIKELY(e.type() == Error::NoError)) {
        return group.toStash(c);
    } else {
        e.toStash(c, true);
        return false;
    }
}

GuestGroup GuestGroup::fromStash(Cutelyst::Context *c)
{
    return c->stash(QStringLiteral(GUESTGROUP_STASH_KEY)).value<GuestGroup>();
}

std::vector<GuestGroup> GuestGroup::list(Cutelyst::Context *c, Error *e, dbid_t eventId)
{
    std::vector<GuestGroup> groups;

    Event event = Event::fromStash(c);
    if (event.id() != eventId) {
        event = Event::get(c, e, eventId);
        if (e && e->type() != Error::NoError) {
            return groups;
        }
    }
    groups = GuestGroup::list(c, e, event);

    return groups;
}

std::vector<GuestGroup> GuestGroup::list(Cutelyst::Context *c, Error *e, const Event &event)
{
    std::vector<GuestGroup> groups;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT g.id, g.name, g.slug, g.note, g.created_at, g.updated_at, g.locked_at, "
                                                           "u.id AS locked_by_id, u.username AS locked_by_username "
                                                           "FROM guestgroups g LEFT JOIN users u ON u.id = g.locked_by WHERE g.event_id = :event_id"));
    q.bindValue(QStringLiteral(":event_id"), event.id());

    if (Q_LIKELY(q.exec())) {
        if (q.size() > 0) {
            groups.reserve(q.size());
        }

        while (q.next()) {
            const qlonglong  lat      = q.value(6).toLongLong();
            const QDateTime  lockedAt = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy = lat > 0 ? SimpleUser(q.value(7).toUInt(), q.value(8).toString()) : SimpleUser();

            groups.emplace_back(q.value(0).toUInt(),
                                event,
                                q.value(1).toString(),
                                q.value(2).toString(),
                                q.value(3).toString(),
                                q.value(4).toDateTime(),
                                q.value(5).toDateTime(),
                                lockedAt,
                                lockedBy);
        }

    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("GuestGroup", "Failed to query guest groups from the database."));
        qCCritical(GIK_CORE) << "Failed to query list of guest groups for event ID" << event.id() << "from the database:" << q.lastError().text();
    }

    return groups;
}

GuestGroup GuestGroup::get(Cutelyst::Context *c, Error *e, dbid_t id)
{
    GuestGroup group;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT g.event_id, g.name, g.slug, g.note, g.created_at, g.updated_at, g.locked_at, "
                                                           "u.id AS locked_by_id, u.username AS locked_by_username "
                                                           "FROM guestgroups g LEFT JOIN users u ON u.id = g.locked_by WHERE g.id = :id"));
    q.bindValue(QStringLiteral(":id"), id);

    if (Q_LIKELY(q.exec())) {
        if (Q_LIKELY(q.next())) {

            const qlonglong lat = q.value(6).toLongLong();
            const QDateTime lockedAt = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy = lat > 0 ? SimpleUser(q.value(7).toUInt(), q.value(8).toString()) : SimpleUser();

            group = GuestGroup(id,
                               q.value(0).toUInt(),
                               q.value(1).toString(),
                               q.value(2).toString(),
                               q.value(3).toString(),
                               q.value(4).toDateTime(),
                               q.value(5).toDateTime(),
                               lockedAt,
                               lockedBy);

        } else {
            if (c && e) *e = Error(Error::NotFound, c->translate("GuestGroup", "Can not find guest group with ID %1 in the database.").arg(id));
            qCWarning(GIK_CORE) << "Can not find guest group with ID" << id << "in the database";
        }
    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("GuestGroup", "Failed to get guest group with ID %1 from the database.").arg(id));
        qCCritical(GIK_CORE) << "Failed to execute database query to get guest group with ID" << id << "from the database:" << q.lastError().text();
    }

    return group;
}

QDataStream &operator<<(QDataStream &stream, const GuestGroup &gg)
{
    stream << gg.d->id
           << gg.d->eventId
           << gg.d->event
           << gg.d->name
           << gg.d->slug
           << gg.d->note
           << gg.d->created
           << gg.d->updated
           << gg.d->lockedAt
           << gg.d->lockedBy;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, GuestGroup &gg)
{
    stream >> gg.d->id;
    stream >> gg.d->eventId;
    stream >> gg.d->event;
    stream >> gg.d->name;
    stream >> gg.d->slug;
    stream >> gg.d->note;
    stream >> gg.d->created;
    stream >> gg.d->updated;
    stream >> gg.d->lockedAt;
    stream >> gg.d->lockedBy;
    return stream;
}

#include "moc_guestgroup.cpp"
