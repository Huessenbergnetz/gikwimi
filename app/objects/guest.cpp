/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guest.h"
#include "error.h"
#include "event.h"
#include "../logging.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Utils/Sql>

#include <cutelee/engine.h>
#include <cutelee/context.h>
#include <cutelee/template.h>

#include <KContacts/VCardConverter>
#include <KContacts/Addressee>

#include <QSqlQuery>
#include <QSqlError>
#include <QRandomGenerator>
#include <QMetaObject>
#include <QMetaEnum>
#include <QJsonValue>
#include <QJsonArray>

#include <limits>
#include <algorithm>

class GuestData : public QSharedData
{
public:
    Contact contact;
    GuestGroup group;
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
    dbid_t groupId = 0;
    GuestGroup::Salutation salutation = GuestGroup::SalutationInvalid;
    Guest::Type type = Guest::InvalidType;
    quint8 adults = 0;
    quint8 adultsAccepted = 0;
    quint8 children = 0;
    quint8 childrenAccepted = 0;
    Guest::Status status = Guest::InvalidStatus;
    Guest::Notifications notifications = Guest::NotNotified;
};

Guest::Guest()
{

}

Guest::Guest(dbid_t id, const QString &uid, dbid_t groupId, const Contact &contact, const QString &pgName, const QString &pfName, uint adults, uint adultsAccepted, uint children, uint childrenAccepted, Guest::Status status, Guest::Notifications notifications, const QString &note, const QString &comment, GuestGroup::Salutation salutation, Type type, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new GuestData)
{
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
    d->groupId = groupId;
    d->salutation = salutation;
    d->type = type;
    d->adults = static_cast<quint8>(std::min(adults, static_cast<uint>(std::numeric_limits<quint8>::max())));
    d->adultsAccepted = static_cast<quint8>(std::min(adultsAccepted, static_cast<uint>(std::numeric_limits<quint8>::max())));
    d->children = static_cast<quint8>(std::min(children, static_cast<uint>(std::numeric_limits<quint8>::max())));
    d->childrenAccepted = static_cast<quint8>(std::min(childrenAccepted, static_cast<uint>(std::numeric_limits<quint8>::max())));
    d->status = status;
    d->notifications = notifications;
}

Guest::Guest(dbid_t id, const QString &uid, const GuestGroup &group, const Contact &contact, const QString &pgName, const QString &pfName, uint adults, uint adultsAccepted, uint children, uint childrenAccepted, Guest::Status status, Guest::Notifications notifications, const QString &note, const QString &comment, GuestGroup::Salutation salutation, Type type, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new GuestData)
{
    d->contact = contact;
    d->group = group;
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
    d->salutation = salutation;
    d->type = type;
    d->adults = static_cast<quint8>(std::min(adults, static_cast<uint>(std::numeric_limits<quint8>::max())));
    d->adultsAccepted = static_cast<quint8>(std::min(adultsAccepted, static_cast<uint>(std::numeric_limits<quint8>::max())));
    d->children = static_cast<quint8>(std::min(children, static_cast<uint>(std::numeric_limits<quint8>::max())));
    d->childrenAccepted = static_cast<quint8>(std::min(childrenAccepted, static_cast<uint>(std::numeric_limits<quint8>::max())));
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

GuestGroup Guest::group() const
{
    if (d) {
        if (d->group.isNull() && d->groupId > 0) {
            return GuestGroup::get(nullptr, nullptr, d->groupId);
        } else {
            return d->group;
        }
    } else {
        return GuestGroup();
    }
}

Contact Guest::contact() const
{
    return d ? d->contact : Contact();
}

QString Guest::givenName() const
{
    return d ? d->contact.addressee().givenName() : QString();
}

QString Guest::familyName() const
{
    return d ? d->contact.addressee().familyName() : QString();
}

QString Guest::partnerGivenName() const
{
    return d ? d->partnerGivenName : QString();
}

QString Guest::partnerFamilyName() const
{
    return d ? d->partnerFamilyName : QString();
}

uint Guest::adults() const
{
    return d ? d->adults : 0;
}

uint Guest::adultsAccepted() const
{
    return d ? d->adultsAccepted : 0;
}

uint Guest::children() const
{
    return d ? d->children : 0;
}

uint Guest::childrenAccepted() const
{
    return d ? d->childrenAccepted : 0;
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

GuestGroup::Salutation Guest::salutation() const
{
    return d ? d->salutation : GuestGroup::SalutationInvalid;
}

Guest::Type Guest::type() const
{
    return d ? d->type : Guest::InvalidType;
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

bool Guest::notifiedByEmail() const
{
    return d ? d->notifications.testFlag(Email) : false;
}

bool Guest::notifiedBySMS() const
{
    return d ? d->notifications.testFlag(SMS) : false;
}

bool Guest::notifiedByMessenger() const
{
    return d ? d->notifications.testFlag(Messenger) : false;
}

bool Guest::notifiedByPostal() const
{
    return d ? d->notifications.testFlag(Postal) : false;
}

bool Guest::notifiedByPhone() const
{
    return d ? d->notifications.testFlag(Phone) : false;
}

bool Guest::isValid() const
{
    return d && d->id > 0;
}

bool Guest::isNull() const
{
    return d ? false : true;
}

QJsonObject Guest::toJson() const
{
    QJsonObject o;

    if (isNull() || !isValid()) {
        return o;
    }

    o.insert(QStringLiteral("id"), static_cast<qint64>(d->id));
    o.insert(QStringLiteral("uid"), d->uid);
    o.insert(QStringLiteral("group"), group().toJson());
    o.insert(QStringLiteral("contact"), contact().toJson());
    o.insert(QStringLiteral("partnerGivenName"), d->partnerGivenName);
    o.insert(QStringLiteral("partnerFamilyName"), d->partnerFamilyName);
    o.insert(QStringLiteral("adults"), static_cast<qint64>(d->adults));
    o.insert(QStringLiteral("adultsAccepted"), static_cast<qint64>(d->adultsAccepted));
    o.insert(QStringLiteral("children"), static_cast<qint64>(d->children));
    o.insert(QStringLiteral("childrenAccepted"), static_cast<qint64>(d->childrenAccepted));
    o.insert(QStringLiteral("status"), Guest::statusEnumToString(d->status));

    QJsonArray notificationList;
    {
        const QMetaObject mo = Guest::staticMetaObject;
        const QMetaEnum me = mo.enumerator(mo.indexOfEnumerator("Notification"));
        for (int i = 0; i < me.keyCount(); ++i) {
            if (d->notifications.testFlag(static_cast<Notification>(me.value(i)))) {
                notificationList.append(QString::fromLatin1(me.key(i)));
            }
        }
    }
    o.insert(QStringLiteral("notifications"), notificationList);
    o.insert(QStringLiteral("note"), d->note);
    o.insert(QStringLiteral("comment"), d->comment);
    o.insert(QStringLiteral("salutation"), GuestGroup::salutationEnumToString(d->salutation));
    o.insert(QStringLiteral("type"), Guest::typeEnumToString(d->type));
    o.insert(QStringLiteral("created"), d->created.toString(Qt::ISODate));
    o.insert(QStringLiteral("updated"), d->updated.toString(Qt::ISODate));
    o.insert(QStringLiteral("lockedAt"), d->lockedAt.toString(Qt::ISODate));
    o.insert(QStringLiteral("lockedBy"), d->lockedBy.toJson());

    return o;
}

bool Guest::invite(Cutelyst::Context *c, Error *e, Notification notificaton)
{
    return false;
}

QString Guest::invitationText(Cutelyst::Context *c, Error *e, Guest::Notification notification) const
{
    QString text;

    const GuestGroup gg = group();
    const Event event = gg.event();

    GuestGroup::Salutation sal = salutation();
    if (sal == GuestGroup::SalutationDefault) {
        sal = gg.salutation();
    }

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT text, name FROM templates WHERE event_id = :event_id AND salutation = :salutation AND notification = :notification"));
    q.bindValue(QStringLiteral(":event_id"), event.id());
    q.bindValue(QStringLiteral(":salutation"), static_cast<int>(sal));
    q.bindValue(QStringLiteral(":notification"), static_cast<int>(notification));

    if (Q_LIKELY(q.exec())) {
        if (Q_LIKELY(q.next())) {
            const QString templ = q.value(0).toString();
            const QString name  = q.value(1).toString();

            Cutelee::Engine engine;
            auto t = engine.newTemplate(templ, name);
            Cutelee::Context cc;
            cc.insert(QStringLiteral("guest"), QVariant::fromValue<Guest>(*this));
            cc.insert(QStringLiteral("invitation_url"), c->uriFor(c->getAction(QStringLiteral("index"), QStringLiteral("i")), QStringList(), QStringList(uid())));
            text = t->render(&cc);

        } else {
            if (c && e) *e = Error(Error::NotFound, c->translate("Guest", "Can not find a matching template in the database."));
            qCWarning(GIK_CORE) << "Can not find a matching template in the database. Guest ID:" << id() << "Event ID:" << event.id() << "Salutation:" << sal << "Notification:" << notification;
        }
    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("Guest", "Failed to execute database query to get invitation text template."));
        qCCritical(GIK_CORE) << "Failed to execute query to get invitation text template for guest ID" << id() << "from the database:" << q.lastError().text();
    }

    return text;
}

QString Guest::invitationSubject(Cutelyst::Context *c, Error *e, Guest::Notification notification) const
{
    QString subject;

    const GuestGroup gg = group();
    const Event event = gg.event();

    GuestGroup::Salutation sal = salutation();
    if (sal == GuestGroup::SalutationDefault) {
        sal = gg.salutation();
    }

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT subject, name FROM templates WHERE event_id = :event_id AND salutation = :salutation AND notification = :notification"));
    q.bindValue(QStringLiteral(":event_id"), event.id());
    q.bindValue(QStringLiteral(":salutation"), static_cast<int>(sal));
    q.bindValue(QStringLiteral(":notification"), static_cast<int>(notification));

    if (Q_LIKELY(q.exec())) {
        if (Q_LIKELY(q.next())) {
            const QString templ = q.value(0).toString();
            const QString name  = q.value(1).toString();

            Cutelee::Engine engine;
            auto t = engine.newTemplate(templ, name);
            Cutelee::Context cc;
            cc.insert(QStringLiteral("guest"), QVariant::fromValue<Guest>(*this));
            subject = t->render(&cc);

        } else {
            if (c && e) *e = Error(Error::NotFound, c->translate("Guest", "Can not find a matching subject template in the database."));
            qCWarning(GIK_CORE) << "Can not find a matching subject template in the database. Guest ID:" << id() << "Event ID:" << event.id() << "Salutation:" << sal << "Notification:" << notification;
        }
    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("Guest", "Failed to execute database query to get invitation subject template."));
        qCCritical(GIK_CORE) << "Failed to execute query to get invitation subject template for guest ID" << id() << "from the database:" << q.lastError().text();
    }

    return subject;
}

bool Guest::markAsInvited(Cutelyst::Context *c, Error *e, Notification notification)
{
    Guest::Notifications nots = notifications();

    nots |= notification;

    QSqlQuery q = CPreparedSqlQueryThread(QStringLiteral("UPDATE guests SET notified = :notified WHERE id = :id"));
    q.bindValue(QStringLiteral(":notified"), static_cast<int>(nots));
    q.bindValue(QStringLiteral(":id"), id());

    if (Q_LIKELY(q.exec())) {
        d->notifications = nots;
    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("Guest", "Failed to execute database query to update notification status."));
        qCCritical(GIK_CORE) << "Failed to execute query to update notification status of guest ID" << id() << "in the database:" << q.lastError().text();
        return false;
    }

    return true;
}

QString Guest::generateUid(int length)
{
    QString uid;

    const int _length = length >= 6 ? length : 6;
    static QString allowedChars = QStringLiteral("abcdefghijklmnopqrstuvwxyz0123456789");

    auto rand = QRandomGenerator::global();
    uid.reserve(_length);

    for (int i = 0; i < _length; ++i) {
        uid.append(allowedChars.at(rand->bounded(0, allowedChars.size() - 1)));
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
        str = QString::fromLatin1(me.valueToKey(static_cast<int>(status))).toLower();
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

Guest::Type Guest::typeStringToEnum(const QString &str)
{
    bool ok = false;
    const int type = str.toInt(&ok);
    if (ok) {
        return static_cast<Guest::Type>(type);
    } else {
        if (str.compare(QLatin1String("personorcouple"), Qt::CaseInsensitive) == 0) {
            return Guest::PersonOrCouple;
        } else if (str.compare(QLatin1String("family"), Qt::CaseInsensitive) == 0) {
            return Guest::Family;
        } else if (str.compare(QLatin1String("organization"), Qt::CaseInsensitive) == 0) {
            return Guest::Organization;
        } else {
            qCWarning(GIK_CORE) << "Can not convert string to Guest::Type:" << str << "is neither a valid integer value nor a valid enum key.";
            return Guest::InvalidType;
        }
    }
}

QString Guest::typeEnumToString(Type type)
{
    QString str;

    if (type != Guest::InvalidType) {
        const QMetaObject mo = Guest::staticMetaObject;
        const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Type"));
        str = QString::fromLatin1(me.valueToKey(static_cast<int>(type))).toLower();
    }

    return str;
}

QStringList Guest::supportedTypes()
{
    QStringList lst;

    const QMetaObject mo = Guest::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Type"));

    lst.reserve(me.keyCount() - 1);

    for (int i = 1; i < me.keyCount(); ++i) {
        lst.append(QString::fromLatin1(me.key(i)));
    }

    return lst;
}

std::vector<OptionItem> Guest::typeOptionList(Cutelyst::Context *c, Guest::Type selected)
{
    std::vector<OptionItem> options;

    options.emplace_back(c->translate("Guest", "Select guest type"), static_cast<int>(Guest::InvalidType), selected == Guest::InvalidType);
    options.emplace_back(c->translate("Guest", "Person/Couple"), static_cast<int>(Guest::PersonOrCouple), selected == Guest::PersonOrCouple);
    options.emplace_back(c->translate("Guest", "Family"), static_cast<int>(Guest::Family), selected == Guest::Family);
    options.emplace_back(c->translate("Guest", "Organization"), static_cast<int>(Guest::Organization), selected == Guest::Organization);

    return options;
}

QStringList Guest::typeValues()
{
    QStringList list;

    const QMetaObject mo = Guest::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Type"));
    list.reserve(me.keyCount() - 1);

    const int startIndex = 1;
    for (int i = startIndex; i < me.keyCount(); ++i) {
        list << QString::number(me.value(i));
    }

    return list;
}

Guest::Notification Guest::notificationStringToEnum(const QString &str)
{
    bool ok = false;
    const int notification = str.toInt(&ok);
    if (ok) {
        return static_cast<Guest::Notification>(notification);
    } else {
        if (str.compare(QLatin1String("email"), Qt::CaseInsensitive) == 0) {
            return Guest::Email;
        } else if (str.compare(QLatin1String("sms"), Qt::CaseInsensitive) == 0) {
            return Guest::SMS;
        } else if (str.compare(QLatin1String("messenger"), Qt::CaseInsensitive) == 0) {
            return Guest::Messenger;
        } else if (str.compare(QLatin1String("postal"), Qt::CaseInsensitive) == 0) {
            return Guest::Postal;
        } else if (str.compare(QLatin1String("phone"), Qt::CaseInsensitive) == 0) {
            return Guest::Phone;
        } else {
            return Guest::NotNotified;
        }
    }
}

QString Guest::notificationEnumToString(Notification notification)
{
    QString str;

    if (notification != NotNotified) {
        const QMetaObject mo = Guest::staticMetaObject;
        const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Notification"));
        str = QString::fromLatin1(me.valueToKey(static_cast<int>(notification))).toLower();
    }

    return str;
}

std::vector<OptionItem> Guest::notificationOptionList(Cutelyst::Context *c, Notification selected)
{
    std::vector<OptionItem>options;

    options.emplace_back(c->translate("Guest", "Select notification type"), static_cast<int>(Guest::NotNotified), selected == Guest::NotNotified);
    options.emplace_back(c->translate("Guest", "Email"), static_cast<int>(Guest::Email), selected == Guest::Email);
    options.emplace_back(c->translate("Guest", "SMS"), static_cast<int>(Guest::SMS), selected == Guest::SMS);
    options.emplace_back(c->translate("Guest", "Messenger"), static_cast<int>(Guest::Messenger), selected == Guest::Messenger);
    options.emplace_back(c->translate("Guest", "Postal"), static_cast<int>(Guest::Postal), selected == Guest::Postal);
    options.emplace_back(c->translate("Guest", "Phone"), static_cast<int>(Guest::Phone), selected == Guest::Phone);

    return options;
}

QStringList Guest::notificationValues(bool withNotNotified)
{
    QStringList list;

    const QMetaObject mo = Guest::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Notification"));

    const int startIndex = withNotNotified ? 0 : 1;

    list.reserve(me.keyCount());

    for (int i = startIndex; i < me.keyCount(); ++i) {
        list.push_back(QString::number(me.value(i)));
    }

    return list;
}

std::vector<Guest> Guest::list(Cutelyst::Context *c, Error *e, dbid_t groupId)
{
    std::vector<Guest> guests;

    GuestGroup group = GuestGroup::fromStash(c);
    if (group.id() != groupId) {
        group = GuestGroup::get(c, e, groupId);
        if (e && e->type() != Error::NoError) {
            return guests;
        }
    }
    guests = Guest::list(c, e, group);

    return guests;
}

std::vector<Guest> Guest::list(Cutelyst::Context *c, Error *e, const GuestGroup &group)
{
    std::vector<Guest> guests;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT "
                                                           "g.id, g.uid, g.partner_given_name, g.partner_family_name, g.adults, g.adults_accepted, g.children, g.children_accepted, g.status, g.notified, g.note, g.comment, g.salutation, g.type, g.created_at, g.updated_at, g.locked_at, "
                                                           "u.id AS locked_by_id, u.username AS locked_by_username, "
                                                           "c.id AS contact_id, c.addressbook_id, c.vcard, c.created_at contact_created, c.updated_at AS contact_updated "
                                                           "FROM guests g "
                                                           "LEFT JOIN users u ON u.id = g.locked_by "
                                                           "JOIN contacts c ON c.id = g.contact_id "
                                                           "WHERE g.group_id = :group_id "
                                                           "ORDER BY c.family_name, c.given_name"));
    q.bindValue(QStringLiteral(":group_id"), group.id());

    if (Q_LIKELY(q.exec())) {
        if (q.size() > 0) {
            guests.reserve(q.size());
        }

        while (q.next()) {

            const qlonglong  lat      = q.value(16).toLongLong();
            const QDateTime  lockedAt = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy = lat > 0 ? SimpleUser(q.value(17).toUInt(), q.value(18).toString()) : SimpleUser();

            const KContacts::VCardConverter converter;
            const KContacts::Addressee addressee = converter.parseVCard(q.value(21).toString().toUtf8());
            if (addressee.isEmpty()) {
                qCWarning(GIK_CORE) << "Failed to parse vCard for contact id" << q.value(19).toUInt();
            }

            const Contact contact(q.value(19).toUInt(),
                                  q.value(20).toUInt(),
                                  addressee,
                                  q.value(22).toDateTime(),
                                  q.value(23).toDateTime(),
                                  QDateTime(),
                                  SimpleUser());

            guests.emplace_back(q.value(0).toUInt(),
                                q.value(1).toString(),
                                group,
                                contact,
                                q.value(2).toString(),
                                q.value(3).toString(),
                                static_cast<quint8>(q.value(4).toUInt()),
                                static_cast<quint8>(q.value(5).toUInt()),
                                static_cast<quint8>(q.value(6).toUInt()),
                                static_cast<quint8>(q.value(7).toUInt()),
                                static_cast<Guest::Status>(static_cast<qint8>(q.value(8).toInt())),
                                static_cast<Guest::Notifications>(q.value(9).toInt()),
                                q.value(10).toString(),
                                q.value(11).toString(),
                                static_cast<GuestGroup::Salutation>(q.value(12).toInt()),
                                static_cast<Guest::Type>(q.value(13).toInt()),
                                q.value(14).toDateTime(),
                                q.value(15).toDateTime(),
                                lockedAt,
                                lockedBy);
        }

    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("Guest", "Failed to query guests from the database."));
        qCCritical(GIK_CORE) << "Failed to query list of guests for guest group ID" << group.id() << "from the database:" << q.lastError().text();
    }

    return guests;
}

std::vector<Guest> Guest::listByEvent(Cutelyst::Context *c, Error *e, const Event &event)
{
    std::vector<Guest> guests;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT "
                                                           "g.id, g.uid, g.group_id, g.partner_given_name, g.partner_family_name, g.adults, g.adults_accepted, g.children, g.children_accepted, g.status, g.notified, g.note, g.comment, g.salutation, g.type, g.created_at, g.updated_at, g.locked_at, "
                                                           "u.id AS locked_by_id, u.username AS locked_by_username, "
                                                           "c.id AS contact_id, c.addressbook_id, c.vcard, c.created_at contact_created, c.updated_at AS contact_updated "
                                                           "FROM guests g "
                                                           "LEFT JOIN users u ON u.id = g.locked_by "
                                                           "JOIN contacts c ON c.id = g.contact_id "
                                                           "JOIN guestgroups gg ON g.group_id = gg.id "
                                                           "WHERE gg.event_id = :event_id "
                                                           "ORDER BY c.family_name, c.given_name"));
    q.bindValue(QStringLiteral(":event_id"), event.id());

    if (Q_LIKELY(q.exec())) {

        if (q.size() > 0) {
            guests.reserve(q.size());
        }

        while (q.next()) {
            const qlonglong lat = q.value(17).toLongLong();
            const QDateTime lockedAt = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy = lat > 0 ? SimpleUser(q.value(18).toUInt(), q.value(19).toString()) : SimpleUser();

            const KContacts::VCardConverter converter;
            const KContacts::Addressee addressee = converter.parseVCard(q.value(22).toString().toUtf8());
            if (addressee.isEmpty()) {
                qCWarning(GIK_CORE) << "Failed to parse vCard for contact id" << q.value(20).toUInt();
            }

            const Contact contact(q.value(20).toUInt(),
                                  q.value(21).toUInt(),
                                  addressee,
                                  q.value(23).toDateTime(),
                                  q.value(24).toDateTime(),
                                  QDateTime(),
                                  SimpleUser());

            guests.emplace_back(q.value(0).toUInt(),
                                q.value(1).toString(),
                                q.value(2).toUInt(),
                                contact,
                                q.value(3).toString(),
                                q.value(4).toString(),
                                q.value(5).toUInt(),
                                q.value(6).toUInt(),
                                q.value(7).toUInt(),
                                q.value(8).toUInt(),
                                static_cast<Guest::Status>(q.value(9).toInt()),
                                static_cast<Guest::Notifications>(q.value(10).toInt()),
                                q.value(11).toString(),
                                q.value(12).toString(),
                                static_cast<GuestGroup::Salutation>(q.value(13).toInt()),
                                static_cast<Guest::Type>(q.value(14).toInt()),
                                q.value(15).toDateTime(),
                                q.value(16).toDateTime(),
                                lockedAt,
                                lockedBy);
        }

    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("Guest", "Failed to query guests from the database."));
        qCCritical(GIK_CORE) << "Failed to query lists fo guetse for event ID" << event.id() << "from the database:" << q.lastError().text();
    }

    return guests;
}

Guest Guest::create(Cutelyst::Context *c, Error *e, const Event &event, const QVariantHash &p)
{
    Guest guest;

    Q_ASSERT(event.isValid());

    const dbid_t groupId = p.value(QStringLiteral("group")).toUInt();
    const GuestGroup group = GuestGroup::get(c, e, groupId);
    if (!group.isValid()) {
        return guest;
    }

    const dbid_t contactId = p.value(QStringLiteral("contact")).toUInt();
    const Contact contact = Contact::get(c, e, contactId);
    if (!contact.isValid()) {
        return guest;
    }

    if (group.event().id() != event.id()) {
        if (c && e) *e = Error(Error::ApplicationError, c->translate("Guest", "Can not create new guest for group “%1” (ID: %2) that does not belong to event “%3” (ID: %4).").arg(group.name(), QString::number(group.id()), event.title(), QString::number(event.id())));
        qCCritical(GIK_CORE) << "Failed to create new guest for group" << group.name() << "ID:" << group.id() << "that does not belong to event" << event.title() << "ID:" << event.id();
        return guest;
    }

    const QString pgName = p.value(QStringLiteral("partnerGivenName")).toString().trimmed();
    QString pfName = p.value(QStringLiteral("partnerFamilyName")).toString().trimmed();
    if (pfName.isEmpty() && !pgName.isEmpty()) {
        pfName = contact.addressee().familyName();
    }

    const uint expectedAdults = p.value(QStringLiteral("expectedAdults")).toUInt();
    const uint acceptedAdults = event.participation() == Event::Refusal ? expectedAdults : 0;
    const uint expectedChildren = p.value(QStringLiteral("expectedChildren")).toUInt();
    const uint acceptedChildren = event.participation() == Event::Refusal ? expectedChildren : 0;
    const QString note = p.value(QStringLiteral("note")).toString();
    const GuestGroup::Salutation salutation = static_cast<GuestGroup::Salutation>(p.value(QStringLiteral("salutation"), GuestGroup::SalutationDefault).toInt());
    const Guest::Type type = static_cast<Guest::Type>(p.value(QStringLiteral("type")).toInt());
    const QDateTime now = QDateTime::currentDateTimeUtc();
    const QString uid = Guest::generateUid(8);

    QSqlQuery q = CPreparedSqlQueryThread(QStringLiteral("INSERT INTO guests (uid, group_id, contact_id, partner_given_name, partner_family_name, adults, adults_accepted, children, children_accepted, status, note, salutation, type, created_at, updated_at) "
                                                         "VALUES (:uid, :group_id, :contact_id, :partner_given_name, :partner_family_name, :adults, :adults_accepted, :children, :children_accepted, :status, :note, :salutation, :type, :created_at, :updated_at)"));
    q.bindValue(QStringLiteral(":uid"), uid);
    q.bindValue(QStringLiteral(":group_id"), group.id());
    q.bindValue(QStringLiteral(":contact_id"), contact.id());
    q.bindValue(QStringLiteral(":partner_given_name"), pgName);
    q.bindValue(QStringLiteral(":partner_family_name"), pfName);
    q.bindValue(QStringLiteral(":adults"), expectedAdults);
    q.bindValue(QStringLiteral(":adults_accepted"), acceptedAdults);
    q.bindValue(QStringLiteral(":children"), expectedChildren);
    q.bindValue(QStringLiteral(":children_accepted"), acceptedChildren);
    q.bindValue(QStringLiteral(":status"), static_cast<int>(Guest::DefaultStaus));
    q.bindValue(QStringLiteral(":note"), note);
    q.bindValue(QStringLiteral(":salutation"), static_cast<int>(salutation));
    q.bindValue(QStringLiteral(":type"), static_cast<int>(type));
    q.bindValue(QStringLiteral(":created_at"), now);
    q.bindValue(QStringLiteral(":updated_at"), now);

    if (Q_LIKELY(q.exec())) {
        const dbid_t id = q.lastInsertId().toUInt();
        guest = Guest(id, uid, group, contact, pgName, pfName, expectedAdults, acceptedAdults, expectedChildren, acceptedChildren, Guest::DefaultStaus, Guest::NotNotified, note, QString(), salutation, type, now, now, QDateTime(), SimpleUser());
    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("Guest", "Failed to create new guest “%1” for group “%2” in event “%3” in the database.").arg(contact.addressee().formattedName(), group.name(), event.title()));
        qCCritical(GIK_CORE) << "Failed to execute database query to create new guest for group id" << groupId << "in event id" << event.id() << "in the database:" << q.lastError().text();
    }

    return guest;
}

Guest Guest::get(Cutelyst::Context *c, Error *e, dbid_t guestId)
{
    Guest guest;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT "
                                                           "g.uid, g.group_id, g.partner_given_name, g.partner_family_name, g.adults, g.adults_accepted, g.children, g.children_accepted, g.status, g.notified, g.note, g.comment, g.salutation, g.type, g.created_at, g.updated_at, g.locked_at, "
                                                           "u.id AS locked_by_id, u.username AS locked_by_username, "
                                                           "c.id AS contact_id, c.addressbook_id, c.vcard, c.created_at contact_created, c.updated_at AS contact_updated "
                                                           "FROM guests g "
                                                           "LEFT JOIN users u ON u.id = g.locked_by "
                                                           "JOIN contacts c ON c.id = g.contact_id "
                                                           "WHERE g.id = :id"));
    q.bindValue(QStringLiteral(":id"), guestId);

    if (Q_LIKELY(q.exec())) {
        if (Q_LIKELY(q.next())) {

            const qlonglong  lat      = q.value(16).toLongLong();
            const QDateTime  lockedAt = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy = lat > 0 ? SimpleUser(q.value(17).toUInt(), q.value(18).toString()) : SimpleUser();

            const KContacts::VCardConverter converter;
            const KContacts::Addressee addressee = converter.parseVCard(q.value(21).toString().toUtf8());
            if (addressee.isEmpty()) {
                qCWarning(GIK_CORE) << "Failed to parse vCard for contact id" << q.value(19).toUInt();
            }

            const Contact cont(q.value(19).toUInt(),
                               q.value(20).toUInt(),
                               addressee,
                               q.value(22).toDateTime(),
                               q.value(23).toDateTime(),
                               QDateTime(),
                               SimpleUser());

            guest = Guest(guestId,
                          q.value(0).toString(),
                          q.value(1).toUInt(),
                          cont,
                          q.value(2).toString(),
                          q.value(3).toString(),
                          q.value(4).toUInt(),
                          q.value(5).toUInt(),
                          q.value(6).toUInt(),
                          q.value(7).toUInt(),
                          static_cast<Guest::Status>(q.value(8).toInt()),
                          static_cast<Guest::Notifications>(q.value(9).toInt()),
                          q.value(10).toString(),
                          q.value(11).toString(),
                          static_cast<GuestGroup::Salutation>(q.value(12).toInt()),
                          static_cast<Guest::Type>(q.value(13).toInt()),
                          q.value(14).toDateTime(),
                          q.value(15).toDateTime(),
                          lockedAt,
                          lockedBy);

        } else {
            if (c && e) *e = Error(Error::NotFound, c->translate("Guest", "Can not find guest with ID %1 in the database.").arg(guestId));
            qCWarning(GIK_CORE) << "Can not find guest with ID" << guestId << "in the database";
        }
    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("Guest", "Failed to execute query to get guest with ID %1 from the database.").arg(guestId));
        qCCritical(GIK_CORE) << "Failed to execute query to get guest ID" << guestId << "from the database:" << q.lastError().text();
    }

    return guest;
}

QDataStream &operator<<(QDataStream &stream, const Guest &guest)
{
    stream << guest.d->group
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
           << guest.d->groupId
           << static_cast<qint32>(guest.d->salutation)
           << static_cast<qint32>(guest.d->type)
           << guest.d->adults
           << guest.d->adultsAccepted
           << guest.d->children
           << guest.d->childrenAccepted
           << static_cast<qint32>(guest.d->status)
           << static_cast<qint32>(guest.d->notifications);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Guest &guest)
{
    if (guest.d == nullptr) {
        guest.d = new GuestData;
    }

    stream >> guest.d->group;
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
    stream >> guest.d->groupId;
    qint32 salInt;
    stream >> salInt;
    guest.d->salutation = static_cast<GuestGroup::Salutation>(salInt);
    qint32 typeInt;
    stream >> typeInt;
    guest.d->adults = static_cast<Guest::Type>(typeInt);
    stream >> guest.d->adultsAccepted;
    stream >> guest.d->children;
    stream >> guest.d->childrenAccepted;
    qint32 statusInt;
    stream >> statusInt;
    guest.d->status = static_cast<Guest::Status>(statusInt);
    qint32 notificInt;
    stream >> notificInt;
    guest.d->notifications = static_cast<Guest::Notifications>(notificInt);
    return stream;
}

#include "moc_guest.cpp"
