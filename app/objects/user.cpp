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
    : d(new UserData)
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
    return d->id;
}

User::Type User::type() const
{
    return d->type;
}

QString User::username() const
{
    return d->username;
}

QString User::email() const
{
    return d->username;
}

QDateTime User::created() const
{
    return d->created;
}

QDateTime User::updated() const
{
    return d->updated;
}

qint64 User::lockedAt() const
{
    return d->lockedAt;
}

dbid_t User::lockedBy() const
{
    return d->lockedBy;
}

QVariantMap User::settings() const
{
    return d->settings;
}

bool User::isValid() const
{
    return d->id > 0;
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

#include "moc_user.cpp"
