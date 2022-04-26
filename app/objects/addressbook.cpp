/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "addressbook_p.h"
#include "logging.h"

#include <Cutelyst/Plugins/Utils/Sql>
#include <QSqlQuery>
#include <QSqlError>

AddressBook::AddressBook()
    : d(new AddressBookData)
{

}

AddressBook::AddressBook(dbid_t id, dbid_t userId, AddressBook::Type type, const QString &name, const QVariant &data)
    : d(new AddressBookData(id, userId, type, name, data))
{

}

AddressBook::AddressBook(const AddressBook &other) = default;

AddressBook::AddressBook(AddressBook &&other) noexcept = default;

AddressBook &AddressBook::operator=(const AddressBook &other) = default;

AddressBook &AddressBook::operator=(AddressBook &&ohter) noexcept = default;

AddressBook::~AddressBook() = default;

void AddressBook::swap(AddressBook &other) noexcept
{
    std::swap(d, other.d);
}

dbid_t AddressBook::id() const
{
    return d->id;
}

dbid_t AddressBook::userId() const
{
    return d->userId;
}

AddressBook::Type AddressBook::type() const
{
    return d->type;
}

QString AddressBook::name() const
{
    return d->name;
}

QVariant AddressBook::data() const
{
    return d->data;
}

bool AddressBook::isValid() const
{
    return d->id > 0;
}

AddressBook AddressBook::create(dbid_t userId, AddressBook::Type type, const QString &name, const QVariant &data)
{
    AddressBook addressBook;

    QSqlQuery q = CPreparedSqlQueryThread(QStringLiteral("INSERT INTO addressbooks (user_id, type, name, data) VALUES (:user_id, :type, :name, :data)"));
    q.bindValue(QStringLiteral(":user_id"), userId);
    q.bindValue(QStringLiteral(":type"), static_cast<quint8>(type));
    q.bindValue(QStringLiteral(":name"), name);
    q.bindValue(QStringLiteral(":data"), data);

    if (Q_LIKELY(q.exec())) {
        addressBook = AddressBook(q.lastInsertId().toUInt(), userId, type, name, data);
    } else {
        qCCritical(GIK_CORE) << "Failed to create new addressbook for user id" << userId << "with name" << name << "in database:" << q.lastError().text();
    }

    return addressBook;
}

std::vector<AddressBook> AddressBook::list(dbid_t userId)
{
    std::vector<AddressBook> addressBooks;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT id, type, name, data FROM addressbooks WHERE user_id = :user_id"));
    q.bindValue(QStringLiteral(":user_id"), userId);

    if (Q_LIKELY(q.exec())) {
        if (q.size() > 0) {
            addressBooks.reserve(q.size());
        }
        while (q.next()) {
            addressBooks.emplace_back(q.value(0).toUInt(), userId, static_cast<AddressBook::Type>(q.value(1).value<quint8>()), q.value(2).toString(), q.value(3));
        }
    } else {
        qCCritical(GIK_CORE) << "Failed to query list of addressbooks for user id" << userId << "from the database:" << q.lastError().text();
    }

    return addressBooks;
}

AddressBook AddressBook::get(dbid_t id)
{
    AddressBook addressBook;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT user_id, type, name, data FROM addressbooks WHERE id = :id"));
    q.bindValue(QStringLiteral(":id"), id);

    if (Q_LIKELY(q.exec())) {
        if (Q_LIKELY(q.next())) {
            addressBook = AddressBook(id, q.value(0).toUInt(), static_cast<AddressBook::Type>(q.value(1).value<quint8>()), q.value(2).toString(), q.value(3));
        } else {

        }
    } else {
        qCCritical(GIK_CORE) << "Failed to get addressbook with id" << id << "from the database:" << q.lastError().text();
    }

    return addressBook;
}
