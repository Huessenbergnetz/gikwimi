/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_EVENT_H
#define GIKWIMI_EVENT_H

#include "global.h"
#include "user.h"
#include "simpleuser.h"

#include <QObject>
#include <QSharedDataPointer>
#include <QDateTime>
#include <QVariantHash>
#include <QJsonObject>

#include <vector>

namespace Cutelyst {
class Context;
}

class QTimeZone;

class Error;
class EventData;
class Event
{
    Q_GADGET
    Q_PROPERTY(dbid_t id READ id CONSTANT)
    Q_PROPERTY(User user READ user CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString slug READ slug CONSTANT)
    Q_PROPERTY(QDateTime start READ start CONSTANT)
    Q_PROPERTY(QDateTime startUtc READ startUtc CONSTANT)
    Q_PROPERTY(QDateTime end READ end CONSTANT)
    Q_PROPERTY(QDateTime endUtc READ endUtc CONSTANT)
    Q_PROPERTY(QString timezone READ ianaTimezone CONSTANT)
    Q_PROPERTY(Event::Audience audience READ audience CONSTANT)
    Q_PROPERTY(Event::Participation participation READ participation CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QVariantHash settings READ settings CONSTANT)
    Q_PROPERTY(bool isAllDay READ isAllDay CONSTANT)
    Q_PROPERTY(bool startTimeOnly READ startTimeOnly CONSTANT)
    Q_PROPERTY(QDateTime created READ created CONSTANT)
    Q_PROPERTY(QDateTime updated READ updated CONSTANT)
    Q_PROPERTY(QDateTime lockedAt READ lockedAt CONSTANT)
    Q_PROPERTY(SimpleUser lockedBy READ lockedBy CONSTANT)
    Q_PROPERTY(uint adultsInvited READ adultsInvited CONSTANT)
    Q_PROPERTY(uint adultsAccepted READ adultsAccepted CONSTANT)
    Q_PROPERTY(uint childrenInvited READ childrenInvited CONSTANT)
    Q_PROPERTY(uint childrenAccepted READ childrenAccepted CONSTANT)
    Q_PROPERTY(uint totalInvited READ totalInvited CONSTANT)
    Q_PROPERTY(uint totalAccepted READ totalAccepted CONSTANT)
public:
    /*!
     * \brief Audience for this event.
     */
    enum Audience : int {
        InvalidAudience = 0,
        Private = 1,
        Public = 2
    };
    Q_ENUM(Audience)

    /*!
     * \brief Participation strategy for the event.
     */
    enum Participation : int {
        InvalidParticipation = 0,   /**< Invalid participation strategy */
        Refusal = 1,                /**< Guests have to refuse explicitely */
        Acceptance = 2              /**< Guests have to accept explicitely */
    };
    Q_ENUM(Participation)

    Event();

    Event(dbid_t id, dbid_t userId, const QString &title, const QString &slug, const QDateTime &start, const QDateTime &end, const QTimeZone &tz, Audience audience, Participation participation, const QString &description, const QVariantHash &settings, bool allDay, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy, uint adultsInvited, uint adultsAccepted, uint childrenInvited, uint childrenAccepted);

    Event(dbid_t id, const User &user, const QString &title, const QString &slug, const QDateTime &start, const QDateTime &end, const QTimeZone &tz, Audience audience, Participation participation, const QString &description, const QVariantHash &settings, bool allDay, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy, uint adultsInvited, uint adultsAccepted, uint childrenInvited, uint childrenAccepted);

    Event(const Event &other);

    Event(Event &&other) noexcept;

    Event &operator=(const Event &other);

    Event &operator=(Event &&other) noexcept;

    ~Event();

    void swap(Event &other) noexcept;

    dbid_t id() const;

    User user() const;

    QString title() const;

    QString slug() const;

    QDateTime start() const;

    QDateTime startUtc() const;

    QDateTime end() const;

    QDateTime endUtc() const;

    QString ianaTimezone() const;

    Event::Audience audience() const;

    Event::Participation participation() const;

    QString description() const;

    QVariantHash settings() const;

    bool isAllDay() const;

    bool startTimeOnly() const;

    QDateTime created() const;

    QDateTime updated() const;

    QDateTime lockedAt() const;

    SimpleUser lockedBy() const;

    uint adultsInvited() const;

    uint adultsAccepted() const;

    uint childrenInvited() const;

    uint childrenAccepted() const;

    uint totalInvited() const;

    uint totalAccepted() const;

    bool isValid() const;

    bool isNull() const;

    QJsonObject toJson() const;

    bool toStash(Cutelyst::Context *c) const;

    static bool toStash(Cutelyst::Context *c, dbid_t eventId);

    static Event fromStash(Cutelyst::Context *c);

    static Event::Audience audienceStringToEnum(const QString &str);

    static QString audienceEnumToString(Event::Audience audience);

    static QStringList supportedAudiences();

    static Event::Participation participationStringToEnum(const QString &str);

    static QString participationEnumToString(Event::Participation participation);

    static QStringList supportedParticipations();

    static std::vector<Event> list(Cutelyst::Context *c, Error *e, dbid_t userId);

    static std::vector<Event> list(Cutelyst::Context *c, Error *e, const User &user);

    static Event get(Cutelyst::Context *c, Error *e, dbid_t eventId);

private:
    QSharedDataPointer<EventData> d;

    friend QDataStream &operator<<(QDataStream &stream, const Event &event);
    friend QDataStream &operator>>(QDataStream &stream, Event &event);
};

Q_DECLARE_METATYPE(Event)
Q_DECLARE_TYPEINFO(Event, Q_MOVABLE_TYPE);

QDataStream &operator<<(QDataStream &stream, const Event &event);

QDataStream &operator>>(QDataStream &stream, Event &event);

#endif // GIKWIMI_EVENT_H
