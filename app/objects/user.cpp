/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "user.h"
#include "error.h"
#include "logging.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Authentication/authenticationuser.h>
#include <Cutelyst/Plugins/Utils/Sql>

#include <QSqlQuery>
#include <QSqlError>
#include <QMetaObject>
#include <QMetaEnum>
#include <QDebug>

#define USER_STASH_KEY "user"

class UserData : public QSharedData
{
public:
    QString username;
    QString email;
    QDateTime created;
    QDateTime updated;
    QVariantMap settings;
    qint64 lockedAt = 0;
    dbid_t lockedBy = 0;
    dbid_t id = 0;
    User::Type type = User::Disabled;
};

User::User()
{

}

User::User(dbid_t id, User::Type type, const QString &username, const QString &email, const QDateTime &created, const QDateTime &updated, qint64 lockedAt, dbid_t lockedBy)
    : d(new UserData)
{
    d->username = username;
    d->email    = email;
    d->created  = created;
    d->created.setTimeSpec(Qt::UTC);
    d->updated  = updated;
    d->updated.setTimeSpec(Qt::UTC);
    d->lockedAt = lockedAt;
    d->lockedBy = lockedBy;
    d->id       = id;
    d->type     = type;
}

User::User(dbid_t id, User::Type type, const QString &username, const QString &email, const QDateTime &created, const QDateTime &updated, qint64 lockedAt, dbid_t lockedBy, const QVariantMap &settings)
    : d(new UserData)
{
    d->username = username;
    d->email    = email;
    d->created  = created;
    d->created.setTimeSpec(Qt::UTC);
    d->updated  = updated;
    d->updated.setTimeSpec(Qt::UTC);
    d->settings = settings;
    d->lockedAt = lockedAt;
    d->lockedBy = lockedBy;
    d->id       = id;
    d->type     = type;
}

User::User(const Cutelyst::AuthenticationUser &user)
    : d(new UserData)
{
    d->username = user.value(QStringLiteral("username")).toString();
    d->email    = user.value(QStringLiteral("email")).toString();
    d->created  = user.value(QStringLiteral("created_at")).toDateTime();
    d->created.setTimeSpec(Qt::UTC);
    d->updated  = user.value(QStringLiteral("updated_at")).toDateTime();
    d->updated.setTimeSpec(Qt::UTC);
    d->settings = user.value(QStringLiteral("settings")).toMap();
    d->lockedAt = user.value(QStringLiteral("locked_at")).toLongLong();
    d->lockedBy = user.value(QStringLiteral("locked_by")).toUInt();
    d->id       = user.id().toUInt();
    d->type     = static_cast<User::Type>(user.value(QStringLiteral("type")).toUInt());
}

User::User(const User &other) = default;

User::User(User &&other) noexcept = default;

User &User::operator=(const User &other) = default;

User &User::operator=(User &&other) noexcept = default;

User::~User() = default;

void User::swap(User &other) noexcept
{
    std::swap(d, other.d);
}

dbid_t User::id() const
{
    return d ? d->id : 0;
}

User::Type User::type() const
{
    return d ? d->type : User::Invalid;
}

QString User::username() const
{
    return d ? d->username : QString();
}

QString User::email() const
{
    return d ? d->email : QString();
}

QDateTime User::created() const
{
    return d ? d->created : QDateTime();
}

QDateTime User::updated() const
{
    return d ? d->updated : QDateTime();
}

qint64 User::lockedAt() const
{
    return d ? d->lockedAt : 0;
}

dbid_t User::lockedBy() const
{
    return d ? d->lockedBy : 0;
}

QVariantMap User::settings() const
{
    return d ? d->settings : QVariantMap();
}

bool User::isAdmin() const
{
    return d && d->type >= User::Administrator;
}

bool User::isValid() const
{
    return d && d->id > 0;
}

bool User::isNull() const
{
    return d ? false : true;
}

void User::toStash(Cutelyst::Context *c)
{
    Q_ASSERT(c);
    c->setStash(QStringLiteral(USER_STASH_KEY), QVariant::fromValue<User>(*this));
}

User User::fromStash(Cutelyst::Context *c)
{
    Q_ASSERT(c);
    return c->stash(QStringLiteral(USER_STASH_KEY)).value<User>();
}

std::vector<User> User::list(Cutelyst::Context *c, Error &e)
{
    std::vector<User> users;

    Q_ASSERT(c);

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT id, type, username, email, created_at, updated_at, locked_at, locked_by FROM users"));

    if (Q_LIKELY(q.exec())) {
        if (q.size() > 0) {
            users.reserve(q.size());
        }
        while (q.next()) {
            users.emplace_back(q.value(0).toUInt(),
                               static_cast<User::Type>(q.value(1).value<quint8>()),
                               q.value(2).toString(),
                               q.value(3).toString(),
                               q.value(4).toDateTime(),
                               q.value(5).toDateTime(),
                               q.value(6).toLongLong(),
                               q.value(7).toUInt());
        }
    } else {
        e = Error(q.lastError(), c->translate("User", "Failed to query user list from the database."));
        qCCritical(GIK_CORE) << "Failed to query list of users from the database:" << q.lastError().text();
    }

    return users;
}

User User::get(Cutelyst::Context *c, Error &e, dbid_t id)
{
    User user;

    Q_ASSERT(c);

    QSqlQuery q1 = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT id, type, username, email, created_at, updated_at, locked_at, locked_by FROM users WHERE id = :id"));
    q1.bindValue(QStringLiteral(":id"), id);

    if (Q_LIKELY(q1.exec())) {
        if (Q_LIKELY(q1.next())) {

            QVariantMap settings;

            QSqlQuery q2 = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT name, value FROM user_settings WHERE user_id = :user_id"));
            if (Q_LIKELY(q2.exec())) {
                while (q2.next()) {
                    settings.insert(q2.value(0).toString(), q2.value(1));
                }
            } else {
                qCWarning(GIK_CORE) << "Failed to execute database query to get user settings for user ID" << id << "from the database:" << q2.lastError().text();
            }

            user = User(q1.value(0).toUInt(),
                        static_cast<User::Type>(q1.value(1).value<quint8>()),
                        q1.value(2).toString(),
                        q1.value(3).toString(),
                        q1.value(4).toDateTime(),
                        q1.value(5).toDateTime(),
                        q1.value(6).toLongLong(),
                        q1.value(7).toUInt(),
                        settings);

        } else {
            e = Error(Error::NotFound, c->translate("User", "Can not find user with ID %1 in the database.").arg(id));
            qCWarning(GIK_CORE) << "Can not find user with ID" << id << "in the database";
        }
    } else {
        e = Error(q1.lastError(), c->translate("User", "Failed to get user with ID %1 from the database.").arg(id));
        qCCritical(GIK_CORE) << "Failed to execute database query to get user with ID" << id << "from the database:" << q1.lastError().text();
    }

    return user;
}

User::Type User::typeStringToEnum(const QString &str)
{
    if (str.compare(QLatin1String("disabled"), Qt::CaseInsensitive) == 0) {
        return User::Disabled;
    } else if (str.compare(QLatin1String("registered"), Qt::CaseInsensitive) == 0) {
        return User::Registered;
    } else if (str.compare(QLatin1String("administrator"), Qt::CaseInsensitive) == 0) {
        return User::Administrator;
    } else if (str.compare(QLatin1String("superuser"), Qt::CaseInsensitive) == 0) {
        return User::SuperUser;
    } else {
        return User::Invalid;
    }
}

QString User::typeEnumToString(User::Type type)
{
    QString str;

    if (type != User::Invalid) {
        const QMetaObject mo = User::staticMetaObject;
        const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Type"));

        str = QString::fromLatin1(me.valueToKey(static_cast<int>(type)));
    }

    return str;
}

QStringList User::supportedTypes()
{
    QStringList lst;

    const QMetaObject mo = User::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Type"));

    lst.reserve(me.keyCount() - 1);

    for (int i = 1; i < me.keyCount(); ++i) {
        lst.append(QString::fromLatin1(me.key(i)));
    }

    return lst;
}

QDebug operator<<(QDebug dbg, const User &user)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "User(";
    dbg << "ID: " << user.id();
    dbg << " , Username: " << user.username();
    dbg << " , Type: " << user.type();
    dbg << " , Email: " << user.email();
    dbg << " , Created: " << user.created();
    dbg << " , Updated: " << user.updated();
    dbg << ')';
    return dbg.maybeSpace();
}

#include "moc_user.cpp"
