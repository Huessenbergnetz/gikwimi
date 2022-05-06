/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "addressbook_p.h"
#include "logging.h"
#include "error.h"
#include "user.h"
#include "../utils.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Utils/Sql>
#include <QSqlQuery>
#include <QSqlError>
#include <QMetaObject>
#include <QMetaEnum>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDataStream>

#define ADDRESSBOOK_STASH_KEY "addressbook"

using namespace Cutelyst;

AddressBook::AddressBook()
    : d(new AddressBookData)
{

}

AddressBook::AddressBook(dbid_t id, AddressBook::Type type, const QString &name, const QVariantHash &settings, const QDateTime &created, const QDateTime &updated, dbid_t userId, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new AddressBookData(id, type, name, settings, created, updated, userId, lockedAt, lockedBy))
{

}

AddressBook::AddressBook(dbid_t id, AddressBook::Type type, const QString &name, const QVariantHash &settings, const QDateTime &created, const QDateTime &updated, const User &user, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new AddressBookData(id, type, name, settings, created, updated, user, lockedAt, lockedBy))
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

QVariantHash AddressBook::settings() const
{
    return d->settings;
}

User AddressBook::user() const
{
    if (d) {
        if (d->user.isNull() && d->userId > 0) {
            return User::get(nullptr, nullptr, d->userId);
        } else {
            return d->user;
        }
    } else {
        return User();
    }
}

QDateTime AddressBook::created() const
{
    return d->created;
}

QDateTime AddressBook::updated() const
{
    return d->updated;
}

QDateTime AddressBook::lockedAt() const
{
    return d->lockedAt;
}

SimpleUser AddressBook::lockedBy() const
{
    return d->lockedBy;
}

bool AddressBook::isValid() const
{
    return d->id > 0;
}

bool AddressBook::toStash(Cutelyst::Context *c) const
{
    Q_ASSERT_X(c, "addressbook to stash", "invalid context pointer");

    if (Q_LIKELY(isValid())) {
        c->stash({
                     {QStringLiteral(ADDRESSBOOK_STASH_KEY), QVariant::fromValue<AddressBook>(*this)},
                     {QStringLiteral("site_title"), name()}
                 });
        return true;
    } else {
        c->res()->setStatus(404);
        c->detach(c->getAction(QStringLiteral("error")));
        return false;
    }
}

bool AddressBook::toStash(Cutelyst::Context *c, dbid_t id)
{
    Q_ASSERT_X(c, "addressbook to stash", "invalid context pointer");

    Error e;
    const auto ab = AddressBook::get(c, &e, id);
    if (Q_LIKELY(e.type() == Error::NoError)) {
        return ab.toStash(c);
    } else {
        e.toStash(c, true);
        return false;
    }
}

AddressBook AddressBook::fromStash(Cutelyst::Context *c)
{
    return c->stash(QStringLiteral(ADDRESSBOOK_STASH_KEY)).value<AddressBook>();
}

AddressBook AddressBook::create(Cutelyst::Context *c, Error *e, dbid_t userId, AddressBook::Type type, const QString &name, const QVariantHash &settings)
{
    AddressBook addressBook;

    User user = User::fromStash(c);
    if (user.id() != userId) {
        user = User::get(c, e, userId);
        if (e->type() != Error::NoError) {
            return addressBook;
        }
    }
    addressBook = AddressBook::create(c, e, user, type, name, settings);

    return addressBook;
}

AddressBook AddressBook::create(Cutelyst::Context *c, Error *e, const User &user, AddressBook::Type type, const QString &name, const QVariantHash &settings)
{
    AddressBook addressBook;

    const QDateTime now = QDateTime::currentDateTimeUtc();
    const QJsonObject settingsObj = QJsonObject::fromVariantHash(settings);
    const QJsonDocument settingsDoc = QJsonDocument(settingsObj);
    const QString settingsStr = QString::fromUtf8(settingsDoc.toJson(QJsonDocument::Compact));

    QSqlQuery q = CPreparedSqlQueryThread(QStringLiteral("INSERT INTO addressbooks (user_id, type, name, data, created_at, updated_at) "
                                                         "VALUES (:user_id, :type, :name, :data, :created_at, :updated_at)"));
    q.bindValue(QStringLiteral(":user_id"), user.id());
    q.bindValue(QStringLiteral(":type"), static_cast<quint8>(type));
    q.bindValue(QStringLiteral(":name"), name);
    q.bindValue(QStringLiteral(":data"), settingsStr);
    q.bindValue(QStringLiteral(":created_at"), now);
    q.bindValue(QStringLiteral(":updated at"), now);

    if (Q_LIKELY(q.exec())) {
        const dbid_t id = q.lastInsertId().toUInt();
        addressBook = AddressBook(id, type, name, settings, now, now, user, QDateTime(), SimpleUser());
    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("AddressBook", "Failed to create new addressbook."));
        qCCritical(GIK_CORE) << "Failed to create new addressbook for user id" << user.id() << "with name" << name << "in database:" << q.lastError().text();
    }

    return addressBook;
}

std::vector<AddressBook> AddressBook::list(Cutelyst::Context *c, Error *e, dbid_t userId)
{
    std::vector<AddressBook> addressBooks;

    User user = User::fromStash(c);
    if (user.id() != userId) {
        user = User::get(c, e, userId);
        if (e && e->type() != Error::NoError) {
            return addressBooks;
        }
    }
    addressBooks = AddressBook::list(c, e, user);

    return addressBooks;
}

std::vector<AddressBook> AddressBook::list(Cutelyst::Context *c, Error *e, const User &user)
{
    std::vector<AddressBook> addressBooks;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT a.id, a.type, a.name, a.settings, a.created_at, a.updated_at, a.locked_at, u.id AS locked_by_id, u.username AS locked_by_username "
                                                           "FROM addressbooks a LEFT JOIN users u ON u.id = a.locked_by WHERE a.user_id = :user_id"));
    q.bindValue(QStringLiteral(":user_id"), user.id());

    if (Q_LIKELY(q.exec())) {
        if (q.size() > 0) {
            addressBooks.reserve(q.size());
        }

        while (q.next()) {

            const QVariantHash settings = Utils::settingsHashFromString(q.value(3).toString());

            const qlonglong  lat      = q.value(6).toLongLong();
            const QDateTime  lockedAt = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy = lat > 0 ? SimpleUser(q.value(7).toUInt(), q.value(8).toString()) : SimpleUser();

            addressBooks.emplace_back(q.value(0).toUInt(),
                                      static_cast<AddressBook::Type>(q.value(1).value<qint8>()),
                                      q.value(2).toString(),
                                      settings,
                                      q.value(4).toDateTime(),
                                      q.value(5).toDateTime(),
                                      user,
                                      lockedAt,
                                      lockedBy);
        }

    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("AddressBook", "Failed to query addressbooks from the database."));
        qCCritical(GIK_CORE) << "Failed to query list of addressbooks for user id" << user.id() << "from the database:" << q.lastError().text();
    }

    return addressBooks;
}

AddressBook AddressBook::get(Cutelyst::Context *c, Error *e, dbid_t id)
{
    AddressBook addressBook;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT "
                                                           "a.id, a.type, a.name, a.settings, a.created_at, a.updated_at, a.user_id, a.locked_at, "
                                                           "u.id AS locked_by_id, u.username AS locked_by_username, "
                                                           "FROM addressbooks a LEFT JOIN users u ON u.id = a.locked_by WHERE a.id = :id"));
    q.bindValue(QStringLiteral(":id"), id);

    if (Q_LIKELY(q.exec())) {
        if (Q_LIKELY(q.next())) {
            const QVariantHash settings = Utils::settingsHashFromString(q.value(3).toString());

            const qlonglong  lat      = q.value(7).toLongLong();
            const QDateTime  lockedAt = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy = lat > 0 ? SimpleUser(q.value(8).toUInt(), q.value(9).toString()) : SimpleUser();

            addressBook = AddressBook(q.value(0).toUInt(),
                                      static_cast<AddressBook::Type>(q.value(1).value<qint8>()),
                                      q.value(2).toString(),
                                      settings,
                                      q.value(4).toDateTime(),
                                      q.value(5).toDateTime(),
                                      q.value(6).toUInt(),
                                      lockedAt,
                                      lockedBy);
        } else {
            if (c && e) *e = Error(Error::NotFound, c->translate("AddressBook", "Can not find addressbook with ID %1.").arg(id));
            qCWarning(GIK_CORE) << "Can not find addressbook with ID" << id << "in the database";
        }
    } else {
        if (c && e) *e = Error(q.lastError().text(), c->translate("AddressBook", "Failed to get addressbook with ID %1 from the database.").arg(id));
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

QDebug operator<<(QDebug dbg, const AddressBook &addressbook)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "AddressBook(";
    dbg << "ID: " << addressbook.id();
    dbg << ", Type: " << addressbook.type();
    dbg << ", Name: " << addressbook.name();
    dbg << ", Settings: " << addressbook.settings();
    dbg << ", UserID: " << addressbook.user().id();
    return dbg.maybeSpace();
}

QDataStream &operator<<(QDataStream &stream, const AddressBook &ab)
{
    stream << ab.d->name
           << ab.d->user
           << ab.d->settings
           << ab.d->created
           << ab.d->updated
           << ab.d->lockedAt
           << ab.d->lockedBy
           << ab.d->id
           << ab.d->type;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, AddressBook &ab)
{
    if (ab.d == nullptr) {
        ab.d = new AddressBookData;
    }

    stream >> ab.d->name;
    stream >> ab.d->user;
    stream >> ab.d->settings;
    stream >> ab.d->created;
    stream >> ab.d->updated;
    stream >> ab.d->lockedAt;
    stream >> ab.d->lockedBy;
    stream >> ab.d->id;
    stream >> ab.d->type;
    return stream;
}

#include "moc_addressbook.cpp"
