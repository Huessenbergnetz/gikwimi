/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_GUESTGROUP_H
#define GIKWIMI_GUESTGROUP_H

#include "global.h"
#include "event.h"
#include "simpleuser.h"

#include <QObject>
#include <QSharedDataPointer>
#include <QJsonObject>

#include <vector>

namespace Cutelyst {
class Context;
}
class Error;
class GuestGroupData;

class GuestGroup
{
    Q_GADGET
    Q_PROPERTY(dbid_t id READ id CONSTANT)
    Q_PROPERTY(Event event READ event CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString slug READ slug CONSTANT)
    Q_PROPERTY(QString note READ note CONSTANT)
    Q_PROPERTY(QVariantList guests READ guests CONSTANT)
    Q_PROPERTY(QDateTime created READ created CONSTANT)
    Q_PROPERTY(QDateTime updated READ updated CONSTANT)
    Q_PROPERTY(QDateTime lockedAt READ lockedAt CONSTANT)
    Q_PROPERTY(SimpleUser lockedBy READ lockedBy CONSTANT)
public:
    GuestGroup();

    GuestGroup(dbid_t id, dbid_t eventId, const QString &name, const QString &slug, const QString &note, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy);

    GuestGroup(dbid_t id, const Event &event, const QString &name, const QString &slug, const QString &note, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy);

    GuestGroup(const GuestGroup &other);

    GuestGroup(GuestGroup &&other) noexcept;

    GuestGroup &operator=(const GuestGroup &other);

    GuestGroup &operator=(GuestGroup &&other) noexcept;

    ~GuestGroup();

    void swap(GuestGroup &other) noexcept;

    dbid_t id() const;

    Event event() const;

    QString name() const;

    QString slug() const;

    QString note() const;

    QVariantList guests() const;

    QDateTime created() const;

    QDateTime updated() const;

    QDateTime lockedAt() const;

    SimpleUser lockedBy() const;

    bool isValid() const;

    bool isNull() const;

    QJsonObject toJson() const;

    bool toStash(Cutelyst::Context *c) const;

    static bool toStash(Cutelyst::Context *c, dbid_t groupId);

    static GuestGroup fromStash(Cutelyst::Context *c);

    static std::vector<GuestGroup> list(Cutelyst::Context *c, Error *e, dbid_t eventId);

    static std::vector<GuestGroup> list(Cutelyst::Context *c, Error *e, const Event &event);

    static GuestGroup get(Cutelyst::Context *c, Error *e, dbid_t id);

private:
    QSharedDataPointer<GuestGroupData> d;

    friend QDataStream &operator<<(QDataStream &stream, const GuestGroup &gg);
    friend QDataStream &operator>>(QDataStream &stream, GuestGroup &gg);
};

Q_DECLARE_METATYPE(GuestGroup)
Q_DECLARE_TYPEINFO(GuestGroup, Q_MOVABLE_TYPE);

QDataStream &operator<<(QDataStream &stream, const GuestGroup &gg);

QDataStream &operator>>(QDataStream &stream, GuestGroup &gg);

#endif // GIKWIMI_GUESTGROUP_H
