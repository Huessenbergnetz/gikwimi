/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_GUEST_H
#define GIKWIMI_GUEST_H

#include "guestgroup.h"
#include "simpleuser.h"
#include "global.h"
#include "contact.h"

#include <QObject>
#include <QSharedDataPointer>

namespace Cutelyst {
class Context;
}
class Error;
class Event;
class GuestData;

class Guest
{
    Q_GADGET
    Q_PROPERTY(dbid_t id READ id CONSTANT)
    Q_PROPERTY(QString uid READ uid CONSTANT)
    Q_PROPERTY(GuestGroup group READ group CONSTANT)
    Q_PROPERTY(Contact contact READ contact CONSTANT)
    Q_PROPERTY(QString partnerGivenName READ partnerGivenName CONSTANT)
    Q_PROPERTY(QString partnerFamilyName READ partnerFamilyName CONSTANT)
    Q_PROPERTY(quint8 adults READ adults CONSTANT)
    Q_PROPERTY(quint8 children READ children CONSTANT)
    Q_PROPERTY(Guest::Status status READ status CONSTANT)
    Q_PROPERTY(Guest::Notifications notifications READ notifications CONSTANT)
    Q_PROPERTY(QString note READ note CONSTANT)
    Q_PROPERTY(QString comment READ comment CONSTANT)
    Q_PROPERTY(QDateTime created READ created CONSTANT)
    Q_PROPERTY(QDateTime updated READ updated CONSTANT)
    Q_PROPERTY(QDateTime lockedAt READ lockedAt CONSTANT)
    Q_PROPERTY(SimpleUser lockedBy READ lockedBy CONSTANT)
public:
    enum Status : qint8 {
        InvalidStatus   = -1,
        DefaultStaus    =  0,
        Agreed          =  1,
        Canceled        =  2
    };
    Q_ENUM(Status)

    enum Notification : int {
        NotNotified     = 0x0,
        Email           = 0x1,
        SMS             = 0x2,
        Messenger       = 0x4,
        Postal          = 0x8
    };
    Q_DECLARE_FLAGS(Notifications, Notification)
    Q_FLAG(Notifications)

    Guest();

    Guest(dbid_t id, const QString &uid, dbid_t groupId, const Contact &contact, const QString &pgName, const QString &pfName, quint8 adults, quint8 children, Guest::Status status, Guest::Notifications notifications, const QString &note, const QString &comment, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy);

    Guest(dbid_t id, const QString &uid, const GuestGroup &group, const Contact &contact, const QString &pgName, const QString &pfName, quint8 adults, quint8 children, Guest::Status status, Guest::Notifications notifications, const QString &note, const QString &comment, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy);

    Guest(const Guest &other);

    Guest(Guest &&other) noexcept;

    Guest &operator=(const Guest &other);

    Guest &operator=(Guest &&other) noexcept;

    ~Guest();

    void swap(Guest &other) noexcept;

    dbid_t id() const;

    QString uid() const;

    GuestGroup group() const;

    Contact contact() const;

    QString partnerGivenName() const;

    QString partnerFamilyName() const;

    quint8 adults() const;

    quint8 children() const;

    Guest::Status status() const;

    Guest::Notifications notifications() const;

    QString note() const;

    QString comment() const;

    QDateTime created() const;

    QDateTime updated() const;

    QDateTime lockedAt() const;

    SimpleUser lockedBy() const;

    bool isValid() const;

    bool isNull() const;

    static QString generateUid();

    static Guest::Status statusStringToEnum(const QString &str);

    static QString statusEnumToString(Guest::Status status);

    static QStringList supportedStatus();

    static std::vector<Guest> list(Cutelyst::Context *c, Error *e, dbid_t groupId);

    static std::vector<Guest> list(Cutelyst::Context *c, Error *e, const GuestGroup &group);

    static std::vector<Guest> listByEvent(Cutelyst::Context *c, Error *e, const Event &event);

private:
    QSharedDataPointer<GuestData> d;

    friend QDataStream &operator<<(QDataStream &stream, const Guest &guest);
    friend QDataStream &operator>>(QDataStream &stream, Guest &guest);
};

Q_DECLARE_METATYPE(Guest)
Q_DECLARE_TYPEINFO(Guest, Q_MOVABLE_TYPE);
Q_DECLARE_OPERATORS_FOR_FLAGS(Guest::Notifications)

QDataStream &operator<<(QDataStream &stream, const Guest &guest);

QDataStream &operator>>(QDataStream &stream, Guest &guest);

#endif // GIKWIMI_GUEST_H
