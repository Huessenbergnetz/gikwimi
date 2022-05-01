/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "addressbook_p.h"
#include "logging.h"
#include "error.h"
#include "user.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Utils/Sql>
#include <QSqlQuery>
#include <QSqlError>
#include <QMetaObject>
#include <QMetaEnum>

using namespace Cutelyst;

AddressBook::AddressBook()
    : d(new AddressBookData)
{

}

AddressBook::AddressBook(dbid_t id, AddressBook::Type type, const QString &name, const QVariant &data, const User &user)
    : d(new AddressBookData(id, type, name, data, user))
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

User AddressBook::user() const
{
    return d->user;
}

bool AddressBook::isValid() const
{
    return d->id > 0;
}

AddressBook AddressBook::create(Cutelyst::Context *c, Error &e, dbid_t userId, AddressBook::Type type, const QString &name, const QVariant &data)
{
    AddressBook addressBook;

    User user = User::fromStash(c);
    if (user.id() != userId) {
        user = User::get(c, e, userId);
        if (e.type() != Error::NoError) {
            return addressBook;
        }
    }
    addressBook = AddressBook::create(c, e, user, type, name, data);

    return addressBook;
}

AddressBook AddressBook::create(Cutelyst::Context *c, Error &e, const User &user, AddressBook::Type type, const QString &name, const QVariant &data)
{
    AddressBook addressBook;

    Q_ASSERT(c);

    QSqlQuery q = CPreparedSqlQueryThread(QStringLiteral("INSERT INTO addressbooks (user_id, type, name, data) VALUES (:user_id, :type, :name, :data)"));
    q.bindValue(QStringLiteral(":user_id"), user.id());
    q.bindValue(QStringLiteral(":type"), static_cast<quint8>(type));
    q.bindValue(QStringLiteral(":name"), name);
    q.bindValue(QStringLiteral(":data"), data);

    if (Q_LIKELY(q.exec())) {
        const dbid_t id = q.lastInsertId().toUInt();
        addressBook = AddressBook(id, type, name, data, user);
    } else {
        e = Error(q.lastError(), c->translate("AddressBook", "Failed to create new addressbook."));
        qCCritical(GIK_CORE) << "Failed to create new addressbook for user id" << user.id() << "with name" << name << "in database:" << q.lastError().text();
    }

    return addressBook;
}

std::vector<AddressBook> AddressBook::list(Cutelyst::Context *c, Error &e, dbid_t userId)
{
    std::vector<AddressBook> addressBooks;

    User user = User::fromStash(c);
    if (user.id() != userId) {
        user = User::get(c, e, userId);
        if (e.type() != Error::NoError) {
            return addressBooks;
        }
    }
    addressBooks = AddressBook::list(c, e, user);

    return addressBooks;
}

std::vector<AddressBook> AddressBook::list(Cutelyst::Context *c, Error &e, const User &user)
{
    std::vector<AddressBook> addressBooks;

    Q_ASSERT(c);

    QSqlQuery q;
    if (!user.isNull()) {
        q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT id, type, name, data FROM addressbooks WHERE user_id = :user_id"));
        q.bindValue(QStringLiteral(":user_id"), user.id());
    } else {
        q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT a.id, a.type, a.name, a.data, u.id AS user_id, u.type AS user_type, u.username, u.email, u.created_at, u.updated_at, u.locked_at, u.locked_by FROM addressbooks a JOIN users u ON u.id = a.user_id"));
    }

    if (Q_LIKELY(q.exec())) {
        if (q.size() > 0) {
            addressBooks.reserve(q.size());
        }
        if (!user.isNull()) {
            while (q.next()) {
                addressBooks.emplace_back(q.value(0).toUInt(), static_cast<AddressBook::Type>(q.value(1).value<qint8>()), q.value(2).toString(), q.value(3), user);
            }
        } else {
            while (q.next()) {
                User u(q.value(4).toUInt(), static_cast<User::Type>(q.value(5).value<quint8>()), q.value(6).toString(), q.value(7).toString(), q.value(8).toDateTime(), q.value(9).toDateTime(), q.value(10).toLongLong(), q.value(11).toUInt());
                addressBooks.emplace_back(q.value(0).toUInt(), static_cast<AddressBook::Type>(q.value(1).value<qint8>()), q.value(2).toString(), q.value(3), u);
            }
        }
    } else {
        e = Error(q.lastError(), c->translate("AddressBook", "Failed to query addressbooks from the database."));
        qCCritical(GIK_CORE) << "Failed to query list of addressbooks for user id" << user.id() << "from the database:" << q.lastError().text();
    }

    return addressBooks;
}

AddressBook AddressBook::get(Cutelyst::Context *c, Error &e, dbid_t id)
{
    AddressBook addressBook;

    Q_ASSERT(c);

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT a.id, a.type, a.name, a.data, u.id AS user_id, u.type AS user_type, u.username, u.email, u.created_at, u.updated_at, u.locked_at, u.locked_by FROM addressbooks a JOIN users u ON u.id = a.user_id WHERE a.id = :id"));
    q.bindValue(QStringLiteral(":id"), id);

    if (Q_LIKELY(q.exec())) {
        if (Q_LIKELY(q.next())) {
            User user(q.value(4).toUInt(), static_cast<User::Type>(q.value(5).value<quint8>()), q.value(6).toString(), q.value(7).toString(), q.value(8).toDateTime(), q.value(9).toDateTime(), q.value(10).toLongLong(), q.value(11).toUInt());
            addressBook = AddressBook(q.value(0).toUInt(), static_cast<AddressBook::Type>(q.value(1).value<qint8>()), q.value(2).toString(), q.value(3), user);
        } else {
            e = Error(Error::NotFound, c->translate("AddressBook", "Can not find addressbook with ID %1.").arg(id));
            qCWarning(GIK_CORE) << "Can not find addressbook with ID" << id << "in the database";
        }
    } else {
        e = Error(q.lastError().text(), c->translate("AddressBook", "Failed to get addressbook with ID %1 from the database.").arg(id));
        qCCritical(GIK_CORE) << "Failed to get addressbook with id" << id << "from the database:" << q.lastError().text();
    }

    return addressBook;
}

AddressBook::Type AddressBook::typeStringToEnum(const QString &str)
{
    if (str.compare(QStringLiteral("local"), Qt::CaseInsensitive) == 0) {
        return AddressBook::Local;
    } else {
        return AddressBook::Invalid;
    }
}

QString AddressBook::typeEnumToString(AddressBook::Type type)
{
    QString str;

    if (type != AddressBook::Invalid) {
        const QMetaObject mo = AddressBook::staticMetaObject;
        const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Type"));

        str = QString::fromLatin1(me.valueToKey(static_cast<int>(type)));
    }

    return str;
}

QStringList AddressBook::supportedTypes()
{
    QStringList lst;

    const QMetaObject mo = AddressBook::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Type"));

    lst.reserve(me.keyCount() - 1);

    for (int i = 1; i < me.keyCount(); ++i) {
        lst.append(QString::fromLatin1(me.key(i)));
    }

    return lst;
}

#include "moc_addressbook.cpp"
