/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "simpleuser.h"
#include <QDebug>

class SimpleUserData : public QSharedData
{
public:
    QString username;
    dbid_t id = 0;
};

SimpleUser::SimpleUser()
{

}

SimpleUser::SimpleUser(dbid_t id, const QString &username)
    : d(new SimpleUserData)
{
    d->username = username;
    d->id = id;
}

SimpleUser::SimpleUser(const SimpleUser &other) = default;

SimpleUser::SimpleUser(SimpleUser &&other) noexcept = default;

SimpleUser &SimpleUser::operator=(const SimpleUser &other) = default;

SimpleUser &SimpleUser::operator=(SimpleUser &&other) noexcept = default;

SimpleUser::~SimpleUser() = default;

void SimpleUser::swap(SimpleUser &other)
{
    std::swap(d, other.d);
}

dbid_t SimpleUser::id() const
{
    return d ? d->id : 0;
}

QString SimpleUser::username() const
{
    return d ? d->username : QString();
}

bool SimpleUser::isValid() const
{
    return d && d->id > 0;
}

bool SimpleUser::isNull() const
{
    return d ? false : true;
}

QDebug operator<<(QDebug dbg, const SimpleUser &user)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "SimpleUser(";
    dbg << ", ID:" << user.id();
    dbg << ", Username:" << user.username();
    dbg << ')';
    return dbg.maybeSpace();
}

#include "moc_simpleuser.cpp"
