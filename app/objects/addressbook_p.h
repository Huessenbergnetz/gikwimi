/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_ADDRESSBOOK_P_H
#define GIKWIMI_ADDRESSBOOK_P_H

#include "addressbook.h"
#include <QSharedData>

class AddressBookData : public QSharedData
{
public:
    AddressBookData() = default;

    AddressBookData(dbid_t _id, AddressBook::Type _type, const QString &_name, const QVariantHash &_settings, const QDateTime &_created, const QDateTime &_updated, const User &_user)
        : QSharedData(),
          name{_name},
          user{_user},
          settings{_settings},
          created{_created},
          updated{_updated},
          id{_id},
          type{_type}
    {
        created.setTimeSpec(Qt::UTC);
        updated.setTimeSpec(Qt::UTC);
    }

    ~AddressBookData() = default;

    QString name;
    User user;
    QVariantHash settings;
    QDateTime created;
    QDateTime updated;
    dbid_t id = 0;
    AddressBook::Type type = AddressBook::Local;
};

#endif // GIKWIMI_ADDRESSBOOK_P_H
