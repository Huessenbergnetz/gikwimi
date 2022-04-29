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

    AddressBookData(dbid_t _id, AddressBook::Type _type, const QString &_name, const QVariant &_data, const User &_user)
        : QSharedData(),
          name{_name},
          user{_user},
          data{_data},
          id{_id},
          type{_type}
    {}

    ~AddressBookData() = default;

    QString name;
    User user;
    QVariant data;
    dbid_t id = 0;
    AddressBook::Type type = AddressBook::Local;
};

#endif // GIKWIMI_ADDRESSBOOK_P_H
