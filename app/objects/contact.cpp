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

#define CONTACT_STASH_KEY "contact"

Contact::Contact()
{

}

Contact::Contact(dbid_t id, const AddressBook &addressbook, const KContacts::Addressee &addressee)
    : d(new ContactData)
{
    d->id = id;
    d->addressbook = addressbook;
    d->addressee = addressee;
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

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT id, vcard FROM contacts WHERE addressbook_id = :addressbook_id"));
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
        const dbid_t id = q.value(0).toUInt();
        const QByteArray vcard = q.value(1).toString().toUtf8();
        const KContacts::Addressee addressee = converter.parseVCard(vcard);
        if (Q_UNLIKELY(addressee.isEmpty())) {
            qCWarning(GIK_CORE) << "Failed to parse vCard data for contact ID" << id;
        } else {
            contacts.emplace_back(id, addressbook, addressee);
        }
    }

    return contacts;
}

#include "moc_contact.cpp"
