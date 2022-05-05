/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "contact_p.h"
#include "logging.h"
#include "error.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Utils/Sql>

#include <KContacts/VCardConverter>

#include <QSqlQuery>
#include <QSqlError>
#include <QDataStream>

#define CONTACT_STASH_KEY "contact"

Contact::Contact()
{

}

Contact::Contact(dbid_t id, const AddressBook &addressbook, const KContacts::Addressee &addressee, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new ContactData)
{
    d->id = id;
    d->addressbook = addressbook;
    d->addressee = addressee;
    d->created = created;
    d->created.setTimeSpec(Qt::UTC);
    d->updated = updated;
    d->updated.setTimeSpec(Qt::UTC);
    d->lockedAt = lockedAt;
    if (lockedAt.isValid()) {
        d->lockedAt.setTimeSpec(Qt::UTC);
    }
    d->lockedBy = lockedBy;
}

Contact::Contact(const Contact &other) = default;

Contact::Contact(Contact &&other) noexcept = default;

Contact &Contact::operator=(const Contact &other) = default;

Contact &Contact::operator=(Contact &&other) noexcept = default;

Contact::~Contact() = default;

void Contact::swap(Contact &other) noexcept
{
    std::swap(d, other.d);
}

dbid_t Contact::id() const
{
    return d ? d->id : 0;
}

AddressBook Contact::addressbook() const
{
    return d ? d->addressbook : AddressBook();
}

KContacts::Addressee Contact::addressee() const
{
    return d ? d->addressee : KContacts::Addressee();
}

QDateTime Contact::created() const
{
    return d ? d->created : QDateTime();
}

QDateTime Contact::updated() const
{
    return d ? d->updated : QDateTime();
}

QDateTime Contact::lockedAt() const
{
    return d ? d->lockedAt : QDateTime();
}

SimpleUser Contact::lockedBy() const
{
    return d ? d->lockedBy : SimpleUser();
}

bool Contact::isValid() const
{
    return d && d->id > 0;
}

bool Contact::isNull() const
{
    return d ? false : true;
}

std::vector<Contact> Contact::list(Cutelyst::Context *c, Error &e, const AddressBook &addressbook)
{
    std::vector<Contact> contacts;

    Q_ASSERT(c);

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT c.id, c.vcard, c.created_at, c.updated_at, c.locked_at, u.id AS locked_by_id, u.username AS locked_by_username "
                                                           "FROM contacts c LEFT JOIN users u ON u.id = c.locked_by WHERE addressbook_id = :addressbook_id"));
    q.bindValue(QStringLiteral(":addressbook_id"), addressbook.id());

    if (Q_UNLIKELY(!q.exec())) {
        e = Error(q.lastError(), c->translate("Contact", "Failed to query contacts from database."));
        qCCritical(GIK_CORE) << "Failed to query contacts in" << addressbook << "from database:" << q.lastError().text();
        return contacts;
    }

    if (q.size() > -1) {
        contacts.reserve(q.size());
    }

    KContacts::VCardConverter converter;
    while (q.next()) {
        const dbid_t id                         = q.value(0).toUInt();
        const QByteArray vcard                  = q.value(1).toString().toUtf8();
        const KContacts::Addressee addressee    = converter.parseVCard(vcard);
        const QDateTime created                 = q.value(2).toDateTime();
        const QDateTime updated                 = q.value(3).toDateTime();
        const qlonglong lockedAt                = q.value(4).toLongLong();
        const SimpleUser lockedBy               = lockedAt > 0 ? SimpleUser(q.value(5).toUInt(), q.value(6).toString()) : SimpleUser();
        if (Q_UNLIKELY(addressee.isEmpty())) {
            qCWarning(GIK_CORE) << "Failed to parse vCard data for contact ID" << id;
        } else {
            contacts.emplace_back(id, addressbook, addressee, created, updated, QDateTime::fromMSecsSinceEpoch(lockedAt), lockedBy);
        }
    }

    return contacts;
}

QDataStream &operator<<(QDataStream &stream, const Contact &contact)
{
    stream << contact.d->addressee
           << contact.d->addressbook
           << contact.d->user
           << contact.d->created
           << contact.d->updated
           << contact.d->lockedAt
           << contact.d->lockedBy
           << contact.d->id;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Contact &contact)
{
    if (contact.d == nullptr) {
        contact.d = new ContactData;
    }

    stream >> contact.d->addressee;
    stream >> contact.d->addressbook;
    stream >> contact.d->user;
    stream >> contact.d->created;
    stream >> contact.d->updated;
    stream >> contact.d->lockedAt;
    stream >> contact.d->lockedBy;
    stream >> contact.d->id;

    return stream;
}

#include "moc_contact.cpp"
