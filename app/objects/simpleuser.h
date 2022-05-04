/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_SIMPLEUSER_H
#define GIKWIMI_SIMPLEUSER_H

#include "global.h"

#include <QObject>
#include <QSharedDataPointer>

class SimpleUserData;

class SimpleUser
{
    Q_GADGET
    Q_PROPERTY(dbid_t id READ id CONSTANT)
    Q_PROPERTY(QString username READ username CONSTANT)
public:
    SimpleUser();

    SimpleUser(dbid_t id, const QString &username);

    SimpleUser(const SimpleUser &other);

    SimpleUser(SimpleUser &&other) noexcept;

    SimpleUser &operator=(const SimpleUser &other);

    SimpleUser &operator=(SimpleUser &&other) noexcept;

    ~SimpleUser();

    void swap(SimpleUser &other);

    dbid_t id() const;

    QString username() const;

    bool isValid() const;

    bool isNull() const;

private:
    QSharedDataPointer<SimpleUserData> d;

    friend QDataStream &operator<<(QDataStream &stream, const SimpleUser &user);
    friend QDataStream &operator>>(QDataStream &stream, SimpleUser &user);
};

QDebug operator<<(QDebug dbg, const SimpleUser &user);

QDataStream &operator<<(QDataStream &stream, const SimpleUser &user);

QDataStream &operator>>(QDataStream &stream, SimpleUser &user);

#endif // GIKWIMI_SIMPLEUSER_H
