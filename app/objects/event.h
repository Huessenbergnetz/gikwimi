/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_EVENT_H
#define GIKWIMI_EVENT_H

#include "global.h"
#include "user.h"

#include <QObject>
#include <QSharedDataPointer>
#include <QDateTime>
#include <QVariantHash>

namespace Cutelyst {
class Context;
}

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
    Q_PROPERTY(QDateTime end READ end CONSTANT)
    Q_PROPERTY(Event::Audience audience READ audience CONSTANT)
    Q_PROPERTY(Event::Participation participation READ participation CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QVariantHash settings READ settings CONSTANT)
    Q_PROPERTY(bool isAllDay READ isAllDay CONSTANT)
    Q_PROPERTY(bool startTimeOnly READ startTimeOnly CONSTANT)
public:
    /*!
     * \brief Audience for this event.
     */
    enum Audience : qint8 {
        InvalidAudience = 0,
        Private = 1,
        Public = 2
    };
    Q_ENUM(Audience)

    /*!
     * \brief Participation strategy for the event.
     */
    enum Participation : qint8 {
        InvalidParticipation = 0,   /**< Invalid participation strategy */
        Refusal = 1,                /**< Guests have to refuse explicitely */
        Acceptance = 2              /**< Guests have to accept explicitely */
    };
    Q_ENUM(Participation)

    Event();

    Event(dbid_t id, const User &user, const QString &title, const QString &slug, const QDateTime &start, const QDateTime &end, Audience audience, Participation participation, const QString &description, const QVariantHash settings, bool allDay);

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

    QDateTime end() const;

    Event::Audience audience() const;

    Event::Participation participation() const;

    QString description() const;

    QVariantHash settings() const;

    bool isAllDay() const;

    bool startTimeOnly() const;

    bool isValid() const;

    bool isNull() const;

    static Event::Audience audienceStringToEnum(const QString &str);

    static QString audienceEnumToString(Event::Audience audience);

    static QStringList supportedAudiences();

    static Event::Participation participationStringToEnum(const QString &str);

    static QString participationEnumToString(Event::Participation participation);

    static QStringList supportedParticipations();

private:
    QSharedDataPointer<EventData> d;
};

#endif // GIKWIMI_EVENT_H
