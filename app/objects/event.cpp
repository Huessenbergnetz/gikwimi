/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "event_p.h"

#include <QMetaType>
#include <QMetaEnum>

Event::Event()
{

}

Event::Event(dbid_t id, const User &user, const QString &title, const QString &slug, const QDateTime &start, const QDateTime &end, Audience audience, Participation participation, const QString &description, const QVariantHash settings, bool allDay)
    : d(new EventData)
{
    d->id = id;
    d->user = user;
    d->title = title;
    d->slug = slug;
    d->start = start;
    d->end = end;
    d->audience = audience;
    d->participation = participation;
    d->description = description;
    d->settings = settings;
    d->allDay = allDay;
    d->startOnly = start == end;
}

Event::Event(const Event &other) = default;

Event::Event(Event &&other) noexcept = default;

Event &Event::operator=(const Event &other) = default;

Event &Event::operator=(Event &&other) noexcept = default;

Event::~Event() = default;

void Event::swap(Event &other) noexcept
{
    std::swap(d, other.d);
}

dbid_t Event::id() const
{
    return d ? d->id : 0;
}

User Event::user() const
{
    return d ? d->user : User();
}

QString Event::title() const
{
    return d ? d->title : QString();
}

QString Event::slug() const
{
    return d ? d->slug : QString();
}

QDateTime Event::start() const
{
    return d ? d->start : QDateTime();
}

QDateTime Event::end() const
{
    return d ? d->end : QDateTime();
}

Event::Audience Event::audience() const
{
    return d ? d->audience : InvalidAudience;
}

Event::Participation Event::participation() const
{
    return d ? d->participation : InvalidParticipation;
}

QString Event::description() const
{
    return d ? d->description : QString();
}

QVariantHash Event::settings() const
{
    return d ? d->settings : QVariantHash();
}

bool Event::isAllDay() const
{
    return d ? d->allDay : false;
}

bool Event::startTimeOnly() const
{
    return d ? d->startOnly : false;
}

bool Event::isValid() const
{
    return d && d->id > 0;
}

bool Event::isNull() const
{
    return d ? false : true;
}

Event::Audience Event::audienceStringToEnum(const QString &str)
{
    if (str.compare(QLatin1String("private"), Qt::CaseInsensitive) == 0) {
        return Event::Private;
    } else if (str.compare(QLatin1String("public"), Qt::CaseInsensitive) == 0) {
        return Event::Public;
    } else {
        return Event::InvalidAudience;
    }
}

QString Event::audienceEnumToString(Event::Audience audience)
{
    QString str;

    if (audience != Event::InvalidAudience) {
        const QMetaObject mo = Event::staticMetaObject;
        const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Audience"));

        str = QString::fromLatin1(me.valueToKey(static_cast<int>(audience)));
    }

    return str;
}

QStringList Event::supportedAudiences()
{
    QStringList lst;

    const QMetaObject mo = User::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Audience"));

    lst.reserve(me.keyCount() - 1);

    for (int i = 1; i < me.keyCount(); ++i) {
        lst.append(QString::fromLatin1(me.key(i)));
    }

    return lst;
}

Event::Participation Event::participationStringToEnum(const QString &str)
{
    if (str.compare(QLatin1String("refusal"), Qt::CaseInsensitive) == 0) {
        return Event::Refusal;
    } else if (str.compare(QLatin1String("acceptance"), Qt::CaseInsensitive) == 0) {
        return Event::Acceptance;
    } else {
        return Event::InvalidParticipation;
    }
}

QString Event::participationEnumToString(Event::Participation participation)
{
    QString str;

    if (participation != Event::InvalidParticipation) {
        const QMetaObject mo = Event::staticMetaObject;
        const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Participation"));

        str = QString::fromLatin1(me.valueToKey(static_cast<int>(participation)));
    }

    return str;
}

QStringList Event::supportedParticipations()
{
    QStringList lst;

    const QMetaObject mo = User::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Participation"));

    lst.reserve(me.keyCount() - 1);

    for (int i = 1; i < me.keyCount(); ++i) {
        lst.append(QString::fromLatin1(me.key(i)));
    }

    return lst;
}

#include "moc_event.cpp"
