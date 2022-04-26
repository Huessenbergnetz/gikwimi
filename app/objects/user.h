/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_USER_H
#define GIKWIMI_USER_H

#include "global.h"

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
    Q_PROPERTY(qint64 lockedAt READ lockedAt CONSTANT)
    Q_PROPERTY(dbid_t lockedBy READ lockedBy CONSTANT)
    Q_PROPERTY(QVariantMap settings READ settings CONSTANT)
public:
    enum Type : quint8 {
        Disabled = 0,
        Registered = 64,
        Administrator = 128,
        SuperUser = 255
    };

    User();

    User(dbid_t id, User::Type type, const QString &username, const QString &email, const QDateTime &created, const QDateTime &updated, qint64 lockedAt, dbid_t lockedBy);

    User(dbid_t id, User::Type type, const QString &username, const QString &email, const QDateTime &created, const QDateTime &updated, qint64 lockedAt, dbid_t lockedBy, const QVariantMap &settings);

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

    qint64 lockedAt() const;

    dbid_t lockedBy() const;

    QVariantMap settings() const;

    bool isValid() const;

    void toStash(Cutelyst::Context *c);

    static User fromStash(Cutelyst::Context *c);

    static std::vector<User> list(Cutelyst::Context *c, Error &e);

    static User get(Cutelyst::Context *c, Error &e, dbid_t id);

private:
    QSharedDataPointer<UserData> d;
};

#endif // USER_H
