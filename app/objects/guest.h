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
#include "optionitem.h"

#include <QObject>
#include <QSharedDataPointer>
#include <QJsonObject>

#include <vector>

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
    Q_PROPERTY(QString givenName READ givenName CONSTANT)
    Q_PROPERTY(QString familyName READ familyName CONSTANT)
    Q_PROPERTY(QString partnerGivenName READ partnerGivenName CONSTANT)
    Q_PROPERTY(QString partnerFamilyName READ partnerFamilyName CONSTANT)
    Q_PROPERTY(uint adults READ adults CONSTANT)
    Q_PROPERTY(uint adultsAccepted READ adultsAccepted CONSTANT)
    Q_PROPERTY(uint children READ children CONSTANT)
    Q_PROPERTY(uint childrenAccepted READ childrenAccepted CONSTANT)
    Q_PROPERTY(Guest::Status status READ status CONSTANT)
    Q_PROPERTY(Guest::Status composedStatus READ composedStatus CONSTANT)
    Q_PROPERTY(Guest::Notifications notifications READ notifications CONSTANT)
    Q_PROPERTY(QString note READ note CONSTANT)
    Q_PROPERTY(QString comment READ comment CONSTANT)
    Q_PROPERTY(GuestGroup::Salutation salutation READ salutation CONSTANT)
    Q_PROPERTY(Guest::Type type READ type CONSTANT)
    Q_PROPERTY(QDateTime created READ created CONSTANT)
    Q_PROPERTY(QDateTime updated READ updated CONSTANT)
    Q_PROPERTY(QDateTime lockedAt READ lockedAt CONSTANT)
    Q_PROPERTY(SimpleUser lockedBy READ lockedBy CONSTANT)
    Q_PROPERTY(bool notifiedByEmail READ notifiedByEmail CONSTANT)
    Q_PROPERTY(bool notifiedBySMS READ notifiedBySMS CONSTANT)
    Q_PROPERTY(bool notifiedByMessenger READ notifiedByMessenger CONSTANT)
    Q_PROPERTY(bool notifiedByPostal READ notifiedByPostal CONSTANT)
    Q_PROPERTY(bool notifiedByPhone READ notifiedByPhone CONSTANT)
public:
    enum Type : int {
        InvalidType     = -1,
        PersonOrCouple  =  0,
        Family          =  1,
        Organization    =  2
    };
    Q_ENUM(Type)

    enum Status : int {
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
        Postal          = 0x8,
        Phone           = 0x10
    };
    Q_DECLARE_FLAGS(Notifications, Notification)
    Q_FLAG(Notifications)

    Guest();

    Guest(dbid_t id, const QString &uid, dbid_t groupId, const Contact &contact, const QString &pgName, const QString &pfName, uint adults, uint adultsAccepted, uint children, uint childrenAccepted, Guest::Status status, Guest::Notifications notifications, const QString &note, const QString &comment, GuestGroup::Salutation salutation, Guest::Type type, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy);

    Guest(dbid_t id, const QString &uid, const GuestGroup &group, const Contact &contact, const QString &pgName, const QString &pfName, uint adults, uint adultsAccepted, uint children, uint childrenAccepted, Guest::Status status, Guest::Notifications notifications, const QString &note, const QString &comment, GuestGroup::Salutation salutation, Guest::Type type, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy);

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

    QString givenName() const;

    QString familyName() const;

    QString partnerGivenName() const;

    QString partnerFamilyName() const;

    uint adults() const;

    uint adultsAccepted() const;

    uint children() const;

    uint childrenAccepted() const;

    Guest::Status status() const;

    Guest::Status composedStatus() const;

    Guest::Notifications notifications() const;

    QString note() const;

    QString comment() const;

    GuestGroup::Salutation salutation() const;

    Guest::Type type() const;

    QDateTime created() const;

    QDateTime updated() const;

    QDateTime lockedAt() const;

    SimpleUser lockedBy() const;

    bool notifiedByEmail() const;

    bool notifiedBySMS() const;

    bool notifiedByMessenger() const;

    bool notifiedByPostal() const;

    bool notifiedByPhone() const;

    bool isValid() const;

    bool isNull() const;

    QJsonObject toJson() const;

    bool invite(Cutelyst::Context *c, Error *e, Guest::Notification notificaton);

    QString invitationText(Cutelyst::Context *c, Error *e, Guest::Notification notification) const;

    QString invitationSubject(Cutelyst::Context *c, Error *e, Guest::Notification notification) const;

    bool markAsInvited(Cutelyst::Context *c, Error *e, Guest::Notification notification);

    bool updateStatus(Cutelyst::Context *c, Error *e, const QVariantHash &p);

    bool update(Cutelyst::Context *c, Error *e, const QVariantHash &p);

    bool remove(Cutelyst::Context *c, Error *e);

    static QString generateUid(int length = 8);

    static Guest::Status statusStringToEnum(const QString &str);

    static QString statusEnumToString(Guest::Status status);

    static QStringList supportedStatus();

    static std::vector<OptionItem> statusOptionList(Cutelyst::Context *c, Guest::Status selected = Guest::InvalidStatus);

    static QStringList statusValues(bool withDefault = false);

    static Guest::Type typeStringToEnum(const QString &str);

    static QString typeEnumToString(Guest::Type type);

    static QStringList supportedTypes();

    static std::vector<OptionItem> typeOptionList(Cutelyst::Context *c, Guest::Type selected = Guest::InvalidType);

    static QStringList typeValues();

    static Guest::Notification notificationStringToEnum(const QString &str);

    static QString notificationEnumToString(Guest::Notification notification);

    static std::vector<OptionItem> notificationOptionList(Cutelyst::Context *c, Guest::Notification selected = Guest::NotNotified);

    static QStringList notificationValues(bool withNotNotified = false);

    static std::vector<Guest> list(Cutelyst::Context *c, Error *e, dbid_t groupId);

    static std::vector<Guest> list(Cutelyst::Context *c, Error *e, const GuestGroup &group);

    static std::vector<Guest> listByEvent(Cutelyst::Context *c, Error *e, const Event &event);

    static Guest create(Cutelyst::Context *c, Error *e, const Event &event, const QVariantHash &p);

    static Guest get(Cutelyst::Context *c, Error *e, dbid_t guestId);

    static Guest getByUid(Cutelyst::Context *c, Error *e, const QString &uid);

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
