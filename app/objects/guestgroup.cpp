/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guestgroup.h"
#include "../logging.h"
#include "error.h"
#include "guest.h"
#include "../utils.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Utils/Sql>

#include <QDataStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonValue>
#include <QMetaEnum>

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
    uint adultsInvited = 0;
    uint adultsAccepted = 0;
    uint childrenInvited = 0;
    uint childrenAccepted = 0;
    GuestGroup::Salutation salutation = GuestGroup::SalutationInvalid;
};

GuestGroup::GuestGroup()
{

}

GuestGroup::GuestGroup(dbid_t id, dbid_t eventId, const QString &name, const QString &slug, const QString &note, Salutation salutation, uint adultsInvited, uint adultsAccepted, uint childrenInvited, uint childrenAccepted, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new GuestGroupData)
{
    d->id = id;
    d->eventId = eventId;
    d->name = name;
    d->slug = slug;
    d->note = note;
    d->salutation = salutation;
    d->adultsInvited = adultsInvited;
    d->adultsAccepted = adultsAccepted;
    d->childrenInvited = childrenInvited;
    d->childrenAccepted = childrenAccepted;
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

GuestGroup::GuestGroup(dbid_t id, const Event &event, const QString &name, const QString &slug, const QString &note, Salutation salutation, uint adultsInvited, uint adultsAccepted, uint childrenInvited, uint childrenAccepted, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new GuestGroupData)
{
    d->id = id;
    d->event = event;
    d->name = name;
    d->slug = slug;
    d->note = note;
    d->salutation = salutation;
    d->adultsInvited = adultsInvited;
    d->adultsAccepted = adultsAccepted;
    d->childrenInvited = childrenInvited;
    d->childrenAccepted = childrenAccepted;
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

GuestGroup::Salutation GuestGroup::salutation() const
{
    return d ? d->salutation : GuestGroup::SalutationInvalid;
}

QVariantList GuestGroup::guests() const
{
    QVariantList list;
    if (d) {
        const auto guests = Guest::list(nullptr, nullptr, *this);
        list.reserve(guests.size());
        for (const Guest &guest : guests) {
            list << QVariant::fromValue<Guest>(guest);
        }
    }
    return list;
}

uint GuestGroup::adultsInvited() const
{
    return d ? d->adultsInvited : 0;
}

uint GuestGroup::adultsAccepted() const
{
    return d ? d->adultsAccepted : 0;
}

uint GuestGroup::childrenInvited() const
{
    return d ? d->childrenInvited : 0;
}

uint GuestGroup::childrenAccepted() const
{
    return d ? d->childrenAccepted : 0;
}

uint GuestGroup::totalInvited() const
{
    if (d) {
        return d->adultsInvited + d->childrenInvited;
    } else {
        return 0;
    }
}

uint GuestGroup::totalAccepted() const
{
    if (d) {
        return d->adultsAccepted + d->childrenAccepted;
    } else {
        return 0;
    }
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

QJsonObject GuestGroup::toJson() const
{
    QJsonObject o;

    if (isNull() || !isValid()) {
        return o;
    }

    o.insert(QStringLiteral("id"), static_cast<qint64>(d->id));
    o.insert(QStringLiteral("event"), event().toJson());
    o.insert(QStringLiteral("name"), d->name);
    o.insert(QStringLiteral("slug"), d->slug);
    o.insert(QStringLiteral("note"), d->note);
    o.insert(QStringLiteral("salutation"), GuestGroup::salutationEnumToString(d->salutation));
    o.insert(QStringLiteral("created"), d->created.toString(Qt::ISODate));
    o.insert(QStringLiteral("updated"), d->updated.toString(Qt::ISODate));
    o.insert(QStringLiteral("lockedAt"), d->lockedAt.toString(Qt::ISODate));
    o.insert(QStringLiteral("lockedBy"), d->lockedBy.toJson());

    return o;
}

bool GuestGroup::remove(Cutelyst::Context *c, Error *e)
{
    QSqlQuery q = CPreparedSqlQueryThread(QStringLiteral("DELETE FROM guestgroups WHERE id = :id"));
    q.bindValue(QStringLiteral(":id"), d->id);

    if (Q_LIKELY(q.exec())) {
        return true;
    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("GuestGroup", "Failed to delete guest group from the database."));
        qCCritical(GIK_CORE) << "Failed to delete guest group ID" << d->id << "from the database:" << q.lastError().text();
        return false;
    }
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

GuestGroup::Salutation GuestGroup::salutationStringToEnum(const QString &str)
{
    bool ok = false;
    const int salutation = str.toInt(&ok);
    if (ok) {
        return static_cast<GuestGroup::Salutation>(salutation);
    } else {
        if (str.compare(QLatin1String("default"), Qt::CaseInsensitive) == 0) {
            return GuestGroup::SalutationDefault;
        } else if (str.compare(QLatin1String("veryformal"), Qt::CaseInsensitive) == 0) {
            return GuestGroup::SalutationVeryFormal;
        } else if (str.compare(QLatin1String("formal"), Qt::CaseInsensitive) == 0) {
            return GuestGroup::SalutationFormal;
        } else if (str.compare(QLatin1String("neutral"), Qt::CaseInsensitive) == 0) {
            return GuestGroup::SalutationNeutral;
        } else if (str.compare(QLatin1String("informal"), Qt::CaseInsensitive) == 0) {
            return GuestGroup::SalutationInformal;
        } else if (str.compare(QLatin1String("veryinformal"), Qt::CaseInsensitive) == 0) {
            return GuestGroup::SalutationVeryInformal;
        } else {
            qCWarning(GIK_CORE) << "Can not convert string to GuestGroup::Salutation:" << str << "is neither a valid integer value nor a valid enum key.";
            return GuestGroup::SalutationInvalid;
        }
    }
}

QString GuestGroup::salutationEnumToString(GuestGroup::Salutation salutation)
{
    QString str;

    if (salutation != SalutationInvalid) {

        const QMetaObject mo = GuestGroup::staticMetaObject;
        const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Salutation"));
        str = QString::fromLatin1(me.valueToKey(static_cast<int>(salutation)));
        str = str.toLower();
        str.remove(QLatin1String("salutation"));
    }

    return str;
}

std::vector<OptionItem> GuestGroup::salutationOptionList(Cutelyst::Context *c, Salutation selected)
{
    std::vector<OptionItem> options;

    options.emplace_back(c->translate("GuestGroup", "Select salutation form"), static_cast<int>(SalutationInvalid), selected == SalutationInvalid);
    options.emplace_back(c->translate("GuestGroup", "default"), static_cast<int>(SalutationDefault), selected == SalutationDefault);
    options.emplace_back(c->translate("GuestGroup", "very formal"), static_cast<int>(SalutationVeryFormal), selected == SalutationVeryFormal);
    options.emplace_back(c->translate("GuestGroup", "formal"), static_cast<int>(SalutationFormal), selected == SalutationFormal);
    options.emplace_back(c->translate("GuestGroup", "neutral"), static_cast<int>(SalutationNeutral), selected == SalutationNeutral);
    options.emplace_back(c->translate("GuestGroup", "informal"), static_cast<int>(SalutationInformal), selected == SalutationInformal);
    options.emplace_back(c->translate("GuestGroup", "very informal"), static_cast<int>(SalutationVeryInformal), selected == SalutationVeryInformal);

    return options;
}

QStringList GuestGroup::salutationValues(bool withDefault)
{
    QStringList list;

    const QMetaObject mo = GuestGroup::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Salutation"));
    const int startIndex = withDefault ? 1 : 2;
    list.reserve(me.keyCount());
    for (int i = startIndex; i < me.keyCount(); ++i) {
        list << QString::number(me.value(i));
    }

    return list;
}

QStringList GuestGroup::salutationKeys(bool withDefault)
{
    QStringList list;

    const QMetaObject mo = GuestGroup::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Salutation"));
    const int startIndex = withDefault ? 1 : 2;
    list.reserve(me.keyCount());
    for (int i = startIndex; i < me.keyCount(); ++i) {
        QString k = QString::fromLatin1(me.key(i));
        k.remove(QLatin1String("Salutation"));
        list << k.toLower();
    }

    return list;
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

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT g.id, g.name, g.slug, g.note, g.salutation, g.created_at, g.updated_at, g.locked_at, "
                                                           "u.id AS locked_by_id, u.username AS locked_by_username "
                                                           "FROM guestgroups g LEFT JOIN users u ON u.id = g.locked_by WHERE g.event_id = :event_id"));
    q.bindValue(QStringLiteral(":event_id"), event.id());

    if (Q_LIKELY(q.exec())) {
        if (q.size() > 0) {
            groups.reserve(q.size());
        }

        while (q.next()) {

            const dbid_t id = q.value(0).toUInt();

            QSqlQuery q2 = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT SUM(g.adults), SUM(g.adults_accepted), SUM(g.children), SUM(g.children_accepted) FROM guests g WHERE g.group_id = :id"));
            q2.bindValue(QStringLiteral(":id"), id);

            uint adults, adultsAccepted, children, childrenAccepted;
            if (q2.exec() && q2.next()) {
                adults              = q2.value(0).toUInt();
                adultsAccepted      = q2.value(1).toUInt();
                children            = q2.value(2).toUInt();
                childrenAccepted    = q2.value(3).toUInt();
            } else {
                qCWarning(GIK_CORE) << "Failed to query guest counts for guest group ID" << id << "from the database:" << q2.lastError().text();
            }

            const qlonglong  lat      = q.value(7).toLongLong();
            const QDateTime  lockedAt = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy = lat > 0 ? SimpleUser(q.value(8).toUInt(), q.value(9).toString()) : SimpleUser();

            groups.emplace_back(id,
                                event,
                                q.value(1).toString(),
                                q.value(2).toString(),
                                q.value(3).toString(),
                                static_cast<GuestGroup::Salutation>(q.value(4).toInt()),
                                adults,
                                adultsAccepted,
                                children,
                                childrenAccepted,
                                q.value(5).toDateTime(),
                                q.value(6).toDateTime(),
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

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT g.event_id, g.name, g.slug, g.note, g.salutation, g.created_at, g.updated_at, g.locked_at, "
                                                           "u.id AS locked_by_id, u.username AS locked_by_username "
                                                           "FROM guestgroups g LEFT JOIN users u ON u.id = g.locked_by WHERE g.id = :id"));
    q.bindValue(QStringLiteral(":id"), id);

    if (Q_LIKELY(q.exec())) {
        if (Q_LIKELY(q.next())) {

            QSqlQuery q2 = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT SUM(g.adults), SUM(g.adults_accepted), SUM(g.children), SUM(g.children_accepted) FROM guests g WHERE g.group_id = :id"));
            q2.bindValue(QStringLiteral(":id"), id);

            uint adults = 0, adultsAccepted = 0, children = 0, childrenAccepted = 0;
            if (q2.exec() && q2.next()) {
                adults              = q2.value(0).toUInt();
                adultsAccepted      = q2.value(1).toUInt();
                children            = q2.value(2).toUInt();
                childrenAccepted    = q2.value(3).toUInt();
            } else {
                qCWarning(GIK_CORE) << "Failed to query guest counts for guest group ID" << id << "from the database:" << q2.lastError().text();
            }

            const qlonglong lat = q.value(7).toLongLong();
            const QDateTime lockedAt = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy = lat > 0 ? SimpleUser(q.value(8).toUInt(), q.value(9).toString()) : SimpleUser();

            group = GuestGroup(id,
                               q.value(0).toUInt(),
                               q.value(1).toString(),
                               q.value(2).toString(),
                               q.value(3).toString(),
                               static_cast<GuestGroup::Salutation>(q.value(4).toInt()),
                               adults,
                               adultsAccepted,
                               children,
                               childrenAccepted,
                               q.value(5).toDateTime(),
                               q.value(6).toDateTime(),
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

GuestGroup GuestGroup::create(Cutelyst::Context *c, Error *e, const Event &event, const QVariantHash &p)
{
    GuestGroup group;

    Q_ASSERT(event.isValid());

    const QString name = p.value(QStringLiteral("name")).toString();
    const QString note = p.value(QStringLiteral("note")).toString();
    const Salutation salutation = static_cast<Salutation>(p.value(QStringLiteral("salutation"), SalutationNeutral).toInt());
    const QDateTime now = QDateTime::currentDateTimeUtc();

    const QString slug = [p,name]() -> QString {
        QString _slug = p.value(QStringLiteral("slug")).toString();
        if (_slug.isEmpty()) {
            _slug = Utils::createSlug(name);
        }
        return _slug;
    }();

    QSqlQuery q = CPreparedSqlQueryThread(QStringLiteral("INSERT INTO guestgroups (event_id, name, slug, note, salutation, created_at, updated_at) "
                                                         "VALUES (:event_id, :name, :slug, :note, :salutation, :created_at, :updated_at)"));
    q.bindValue(QStringLiteral(":event_id"), event.id());
    q.bindValue(QStringLiteral(":name"), name);
    q.bindValue(QStringLiteral(":slug"), slug);
    q.bindValue(QStringLiteral(":note"), note);
    q.bindValue(QStringLiteral(":salutation"), static_cast<int>(salutation));
    q.bindValue(QStringLiteral(":created_at"), now);
    q.bindValue(QStringLiteral(":updated_at"), now);

    if (Q_LIKELY(q.exec())) {
        const dbid_t id = q.lastInsertId().toUInt();
        group = GuestGroup(id, event, name, slug, note, salutation, 0, 0, 0, 0, now, now, QDateTime(), SimpleUser());
    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("GuestGroup", "Failed to create new guest group “%1” in the database.").arg(name));
        qCCritical(GIK_CORE) << "Failed to execute database query to create new guest group" << name << "in the database:" << q.lastError().text();
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
           << static_cast<qint32>(gg.d->salutation)
           << gg.d->adultsInvited
           << gg.d->adultsAccepted
           << gg.d->childrenInvited
           << gg.d->childrenAccepted
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
    qint32 salutInt;
    stream >> salutInt;
    gg.d->salutation = static_cast<GuestGroup::Salutation>(salutInt);
    stream >> gg.d->adultsInvited;
    stream >> gg.d->adultsAccepted;
    stream >> gg.d->childrenInvited;
    stream >> gg.d->childrenAccepted;
    stream >> gg.d->created;
    stream >> gg.d->updated;
    stream >> gg.d->lockedAt;
    stream >> gg.d->lockedBy;
    return stream;
}

#include "moc_guestgroup.cpp"
