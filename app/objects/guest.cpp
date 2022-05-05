/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guest.h"
#include "error.h"
#include "logging.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Utils/Sql>

#include <KContacts/VCardConverter>
#include <KContacts/Addressee>

#include <QSqlQuery>
#include <QSqlError>
#include <QRandomGenerator>
#include <QMetaObject>
#include <QMetaEnum>

class GuestData : public QSharedData
{
public:
    Event event;
    Contact contact;
    QString partnerGivenName;
    QString partnerFamilyName;
    QString note;
    QString comment;
    QString uid;
    QDateTime created;
    QDateTime updated;
    QDateTime lockedAt;
    SimpleUser lockedBy;
    dbid_t id = 0;
    quint8 adults = 1;
    quint8 children = 0;
    Guest::Status status = Guest::InvalidStatus;
    Guest::Notifications notifications = Guest::NotNotified;
};

Guest::Guest()
{

}

Guest::Guest(dbid_t id, const QString &uid, const Event &event, const Contact &contact, const QString &pgName, const QString &pfName, quint8 adults, quint8 children, Guest::Status status, Guest::Notifications notifications, const QString &note, const QString &comment, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new GuestData)
{
    d->event = event;
    d->contact = contact;
    d->partnerGivenName = pgName;
    d->partnerFamilyName = pfName;
    d->note = note;
    d->comment = comment;
    d->uid = uid;
    d->created = created;
    d->updated = updated;
    d->lockedAt = lockedAt;
    d->lockedBy = lockedBy;
    d->id = id;
    d->adults = adults;
    d->children = children;
    d->status = status;
    d->notifications = notifications;
}

Guest::Guest(const Guest &other) = default;

Guest::Guest(Guest &&other) noexcept = default;

Guest &Guest::operator=(const Guest &other) = default;

Guest &Guest::operator=(Guest &&other) noexcept = default;

Guest::~Guest()
{

}

dbid_t Guest::id() const
{
    return d ? d->id : 0;
}

QString Guest::uid() const
{
    return d ? d->uid : QString();
}

Event Guest::event() const
{
    return d ? d->event : Event();
}

Contact Guest::contact() const
{
    return d ? d->contact : Contact();
}

QString Guest::partnerGivenName() const
{
    return d ? d->partnerGivenName : QString();
}

QString Guest::partnerFamilyName() const
{
    return d ? d->partnerFamilyName : QString();
}

quint8 Guest::adults() const
{
    return d ? d->adults : 0;
}

quint8 Guest::children() const
{
    return d ? d->children : 0;
}

Guest::Status Guest::status() const
{
    return d ? d->status : Guest::InvalidStatus;
}

Guest::Notifications Guest::notifications() const
{
    return d ? d->notifications : Guest::NotNotified;
}

QString Guest::note() const
{
    return d ? d->note : QString();
}

QString Guest::comment() const
{
    return d ? d->comment : QString();
}

QDateTime Guest::created() const
{
    return d ? d->created : QDateTime();
}

QDateTime Guest::updated() const
{
    return d ? d->updated : QDateTime();
}

QDateTime Guest::lockedAt() const
{
    return d ? d->lockedAt : QDateTime();
}

SimpleUser Guest::lockedBy() const
{
    return d ? d->lockedBy : SimpleUser();
}

bool Guest::isValid() const
{
    return d && d->id > 0;
}

bool Guest::isNull() const
{
    return d ? false : true;
}

QString Guest::generateUid()
{
    QString uid;

    static int length = 6;
    static std::vector<int> ascii = ([]() -> std::vector<int> {
                                         std::vector<int> vec;
                                         vec.reserve(36);
                                         // decimal ascii codes for 0-9
                                         for (int i = 48; i < 58; ++i) {
                                             vec.push_back(i);
                                         }
                                         // decimal ascii codes for a-z
                                         for (int i = 97; i < 123; ++i) {
                                             vec.push_back(i);
                                         }
                                         return vec;
                                     }());

    auto rand = QRandomGenerator::global();
    uid.reserve(6);

    for (int i = 0; i < length; ++i) {
        uid.append(QChar(rand->bounded(0, ascii.size() - 1)));
    }

    return uid;
}

Guest::Status Guest::statusStringToEnum(const QString &str)
{
    if (str.compare(QLatin1String("defaultstatus"), Qt::CaseInsensitive) == 0) {
        return Guest::DefaultStaus;
    } else if (str.compare(QLatin1String("agreed"), Qt::CaseInsensitive) == 0) {
        return Guest::Agreed;
    } else if (str.compare(QLatin1String("canceled"), Qt::CaseInsensitive) == 0) {
        return Guest::Canceled;
    } else {
        return Guest::InvalidStatus;
    }
}

QString Guest::statusEnumToString(Guest::Status status)
{
    QString str;

    if (status != Guest::InvalidStatus) {
        const QMetaObject mo = Guest::staticMetaObject;
        const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Status"));
        str = QString::fromLatin1(me.valueToKey(static_cast<int>(status)));
    }

    return str;
}

QStringList Guest::supportedStatus()
{
    QStringList lst;

    const QMetaObject mo = Guest::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Status"));

    lst.reserve(me.keyCount() - 1);

    for (int i = 1; i < me.keyCount(); ++i) {
        lst.append(QString::fromLatin1(me.key(i)));
    }

    return lst;
}

std::vector<Guest> Guest::list(Cutelyst::Context *c, Error *e, dbid_t eventId)
{
    std::vector<Guest> guests;

    Event event = Event::fromStash(c);
    if (event.id() != eventId) {
        event = Event::get(c, e, eventId);
        if (e && e->type() != Error::NoError) {
            return guests;
        }
    }
    guests = Guest::list(c, e, event);

    return guests;
}

std::vector<Guest> Guest::list(Cutelyst::Context *c, Error *e, const Event &event)
{
    std::vector<Guest> guests;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT"
                                                           "g.id, g.uid, g.partner_given_name, g.partner_family_name, g.adults, g.children, g.status, g.notified, g.note, g.comment, g.created_at, g.updated_at, g.locked_at, "
                                                           "u.id AS locked_by_id, u.username AS locked_by_username, "
                                                           "c.id AS contact_id, c.addressbook_id, c.vcard, c.created_at contact_created, c.updated_at AS contact_updated "
                                                           "FROM guests g LEFT JOIN users u ON u.id = g.locked_by JOIN contacts c ON c.id = g.contact_id WHERE g.event_id = :event_id"));
    q.bindValue(QStringLiteral(":event_id"), event.id());

    if (Q_LIKELY(q.exec())) {
        if (q.size() > 0) {
            guests.reserve(q.size());
        }

        while (q.next()) {

            const qlonglong  lat      = q.value(12).toLongLong();
            const QDateTime  lockedAt = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy = lat > 0 ? SimpleUser(q.value(13).toUInt(), q.value(14).toString()) : SimpleUser();

            const KContacts::VCardConverter converter;
            const KContacts::Addressee addressee = converter.parseVCard(q.value(17).toString().toUtf8());
            if (addressee.isEmpty()) {
                qCWarning(GIK_CORE) << "Failed to parse vCard for contact id" << q.value(15).toUInt();
            }

            const Contact contact(q.value(15).toUInt(),
                                  q.value(16).toUInt(),
                                  addressee,
                                  q.value(18).toDateTime(),
                                  q.value(19).toDateTime(),
                                  QDateTime(),
                                  SimpleUser());

            const Guest::Notifications notifications = static_cast<Guest::Notifications>(q.value(7).toInt());

            guests.emplace_back(q.value(0).toUInt(),
                                q.value(1).toString(),
                                event,
                                contact,
                                q.value(2).toString(),
                                q.value(3).toString(),
                                static_cast<quint8>(q.value(4).toUInt()),
                                static_cast<quint8>(q.value(5).toUInt()),
                                static_cast<Guest::Status>(static_cast<qint8>(q.value(6).toInt())),
                                notifications,
                                q.value(8).toString(),
                                q.value(9).toString(),
                                q.value(10).toDateTime(),
                                q.value(11).toDateTime(),
                                lockedAt,
                                lockedBy);
        }

    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("Guest", "Failed to query guests from the database."));
        qCCritical(GIK_CORE) << "Failed to query list of guests for event ID" << event.id() << "from the database:" << q.lastError().text();
    }

    return guests;
}

QDataStream &operator<<(QDataStream &stream, const Guest &guest)
{
    stream << guest.d->event
           << guest.d->contact
           << guest.d->partnerGivenName
           << guest.d->partnerFamilyName
           << guest.d->note
           << guest.d->comment
           << guest.d->uid
           << guest.d->created
           << guest.d->updated
           << guest.d->lockedAt
           << guest.d->lockedBy
           << guest.d->id
           << guest.d->adults
           << guest.d->children
           << guest.d->status
           << guest.d->notifications;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Guest &guest)
{
    if (guest.d == nullptr) {
        guest.d = new GuestData;
    }

    stream >> guest.d->event;
    stream >> guest.d->contact;
    stream >> guest.d->partnerGivenName;
    stream >> guest.d->partnerFamilyName;
    stream >> guest.d->note;
    stream >> guest.d->comment;
    stream >> guest.d->uid;
    stream >> guest.d->created;
    stream >> guest.d->updated;
    stream >> guest.d->lockedAt;
    stream >> guest.d->lockedBy;
    stream >> guest.d->id;
    stream >> guest.d->adults;
    stream >> guest.d->children;
    stream >> guest.d->status;
    stream >> guest.d->notifications;
    return stream;
}

#include "moc_guest.cpp"
