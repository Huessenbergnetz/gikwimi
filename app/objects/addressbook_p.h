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

    AddressBookData(dbid_t _id, dbid_t _userId, AddressBook::Type _type, const QString &_name, const QVariant &_data)
        : QSharedData(),
          name{_name},
          data{_data},
          id{_id},
          userId{_userId},
          type{_type}
    {}

    ~AddressBookData() = default;

    QString name;
    QVariant data;
    dbid_t id = 0;
    dbid_t userId = 0;
    AddressBook::Type type = AddressBook::Local;
};

#endif // GIKWIMI_ADDRESSBOOK_P_H
