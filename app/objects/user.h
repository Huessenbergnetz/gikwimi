/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_USER_H
#define GIKWIMI_USER_H

#include "global.h"
#include "simpleuser.h"

#include <QObject>
#include <QSharedDataPointer>
#include <QDateTime>

#include <vector>

namespace Cutelyst {
class Context;
class AuthenticationUser;
}

class Error;
class UserData;
class User
{
    Q_GADGET
    Q_PROPERTY(dbid_t id READ id CONSTANT)
    Q_PROPERTY(User::Type type READ type CONSTANT)
    Q_PROPERTY(QString username READ username CONSTANT)
    Q_PROPERTY(QString email READ email CONSTANT)
    Q_PROPERTY(QDateTime created READ created CONSTANT)
    Q_PROPERTY(QDateTime updated READ updated CONSTANT)
    Q_PROPERTY(QDateTime lockedAt READ lockedAt CONSTANT)
    Q_PROPERTY(SimpleUser lockedBy READ lockedBy CONSTANT)
    Q_PROPERTY(QVariantMap settings READ settings CONSTANT)
    Q_PROPERTY(bool isAdmin READ isAdmin CONSTANT)
public:
    enum Type : qint8 {
        Invalid = -1,
        Disabled = 0,
        Registered = 32,
        Administrator = 64,
        SuperUser = 127
    };
    Q_ENUM(Type)

    User();

    User(dbid_t id, User::Type type, const QString &username, const QString &email, const QDateTime &created, const QDateTime &updated);

    User(dbid_t id, User::Type type, const QString &username, const QString &email, const QDateTime &created, const QDateTime &updated, qint64 lockedAt, const SimpleUser &lockedBy);

    User(dbid_t id, User::Type type, const QString &username, const QString &email, const QDateTime &created, const QDateTime &updated, qint64 lockedAt, const SimpleUser &lockedBy, const QVariantMap &settings);

    User(const Cutelyst::AuthenticationUser &user);

    User(const User &other);

    User(User &&other) noexcept;

    User &operator=(const User &other);

    User &operator=(User && other) noexcept;

    ~User();

    void swap(User &other) noexcept;

    dbid_t id() const;

    User::Type type() const;

    QString username() const;

    QString email() const;

    QDateTime created() const;

    QDateTime updated() const;

    QDateTime lockedAt() const;

    SimpleUser lockedBy() const;

    QVariantMap settings() const;

    bool isAdmin() const;

    bool isValid() const;

    bool isNull() const;

    void toStash(Cutelyst::Context *c);

    static User fromStash(Cutelyst::Context *c);

    static std::vector<User> list(Cutelyst::Context *c, Error *e);

    static User get(Cutelyst::Context *c, Error *e, dbid_t id);

    static User::Type typeStringToEnum(const QString &str);

    static QString typeEnumToString(User::Type tpye);

    static QStringList supportedTypes();

private:
    QSharedDataPointer<UserData> d;

    friend QDataStream &operator<<(QDataStream &stream, const User &user);
    friend QDataStream &operator>>(QDataStream &stream, User &user);
};

Q_DECLARE_METATYPE(User)
Q_DECLARE_TYPEINFO(User, Q_MOVABLE_TYPE);

QDebug operator<<(QDebug dbg, const User &user);

QDataStream &operator<<(QDataStream &stream, const User &user);

QDataStream &operator>>(QDataStream &stream, User &user);

#endif // USER_H
