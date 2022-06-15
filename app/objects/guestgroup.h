/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_GUESTGROUP_H
#define GIKWIMI_GUESTGROUP_H

#include "global.h"
#include "event.h"
#include "simpleuser.h"
#include "optionitem.h"

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
    Q_PROPERTY(GuestGroup::Salutation salutation READ salutation CONSTANT)
    Q_PROPERTY(QVariantList guests READ guests CONSTANT)
    Q_PROPERTY(uint adultsInvited READ adultsInvited CONSTANT)
    Q_PROPERTY(uint adultsAccepted READ adultsAccepted CONSTANT)
    Q_PROPERTY(uint childrenInvited READ childrenInvited CONSTANT)
    Q_PROPERTY(uint childrenAccepted READ childrenAccepted CONSTANT)
    Q_PROPERTY(uint totalInvited READ totalInvited CONSTANT)
    Q_PROPERTY(uint totalAccepted READ totalAccepted CONSTANT)
    Q_PROPERTY(QDateTime created READ created CONSTANT)
    Q_PROPERTY(QDateTime updated READ updated CONSTANT)
    Q_PROPERTY(QDateTime lockedAt READ lockedAt CONSTANT)
    Q_PROPERTY(SimpleUser lockedBy READ lockedBy CONSTANT)
public:
    enum Salutation : int {
        SalutationInvalid = -1,
        SalutationDefault = 0,
        SalutationVeryFormal = 15,
        SalutationFormal = 31,
        SalutationNeutral = 47,
        SalutationInformal = 63,
        SalutationVeryInformal = 79
    };
    Q_ENUM(Salutation)

    GuestGroup();

    GuestGroup(dbid_t id, dbid_t eventId, const QString &name, const QString &slug, const QString &note, GuestGroup::Salutation salutation, uint adultsInvited, uint adultsAccepted, uint childrenInvited, uint childrenAccepted, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy);

    GuestGroup(dbid_t id, const Event &event, const QString &name, const QString &slug, const QString &note, GuestGroup::Salutation salutation, uint adultsInvited, uint adultsAccepted, uint childrenInvited, uint childrenAccepted, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy);

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

    Salutation salutation() const;

    QVariantList guests() const;

    uint adultsInvited() const;

    uint adultsAccepted() const;

    uint childrenInvited() const;

    uint childrenAccepted() const;

    uint totalInvited() const;

    uint totalAccepted() const;

    QDateTime created() const;

    QDateTime updated() const;

    QDateTime lockedAt() const;

    SimpleUser lockedBy() const;

    bool isValid() const;

    bool isNull() const;

    QJsonObject toJson() const;

    bool remove(Cutelyst::Context *c, Error *e);

    bool toStash(Cutelyst::Context *c) const;

    static bool toStash(Cutelyst::Context *c, dbid_t groupId);

    static GuestGroup fromStash(Cutelyst::Context *c);

    static GuestGroup::Salutation salutationStringToEnum(const QString &str);

    static QString salutationEnumToString(GuestGroup::Salutation salutation);

    static std::vector<OptionItem> salutationOptionList(Cutelyst::Context *c, GuestGroup::Salutation selected = GuestGroup::SalutationInvalid);

    static QStringList salutationValues(bool withDefault = true);

    static QStringList salutationKeys(bool withDefault = true);

    static std::vector<GuestGroup> list(Cutelyst::Context *c, Error *e, dbid_t eventId);

    static std::vector<GuestGroup> list(Cutelyst::Context *c, Error *e, const Event &event);

    static GuestGroup get(Cutelyst::Context *c, Error *e, dbid_t id);

    static GuestGroup create(Cutelyst::Context *c, Error *e, const Event &event, const QVariantHash &p);

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
