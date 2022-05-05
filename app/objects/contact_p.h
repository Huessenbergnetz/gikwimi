/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef CONTACT_P_H
#define CONTACT_P_H

#include "contact.h"
#include <QSharedData>

class ContactData : public QSharedData
{
public:

    KContacts::Addressee addressee;
    AddressBook addressbook;
    User user;
    QDateTime created;
    QDateTime updated;
    QDateTime lockedAt;
    SimpleUser lockedBy;
    dbid_t id = 0;
};

#endif // CONTACT_P_H
