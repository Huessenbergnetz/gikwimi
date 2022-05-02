/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef CONTACT_H
#define CONTACT_H

#include "addressbook.h"

#include <KContacts/Addressee>

#include <QSharedDataPointer>

#include <vector>

namespace Cutelyst {
class Context;
}
class Error;
class ContactData;

class Contact
{
    Q_GADGET
    Q_PROPERTY(dbid_t id READ id CONSTANT)
    Q_PROPERTY(AddressBook addressbook READ addressbook CONSTANT)
    Q_PROPERTY(KContacts::Addressee addressee READ addressee CONSTANT)
public:
    Contact();

    Contact(dbid_t id, const AddressBook &addressbook, const KContacts::Addressee &addressee);

    Contact(const Contact &other);

    Contact(Contact &&other) noexcept;

    Contact &operator=(const Contact &other);

    Contact &operator=(Contact &&other) noexcept;

    ~Contact();

    void swap(Contact &other) noexcept;

    dbid_t id() const;

    AddressBook addressbook() const;

    KContacts::Addressee addressee() const;

    bool isValid() const;

    bool isNull() const;

    static std::vector<Contact> list(Cutelyst::Context *c, Error &e, const AddressBook &addressbook);

protected:
    QSharedDataPointer<ContactData> d;
};

Q_DECLARE_METATYPE(Contact)
Q_DECLARE_TYPEINFO(Contact, Q_MOVABLE_TYPE);

#endif // CONTACT_H
