/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_EVENT_P_H
#define GIKWIMI_EVENT_P_H

#include "event.h"

#include <QSharedData>
#include <QTimeZone>

class EventData : public QSharedData
{
public:
    User user;
    QString title;
    QString slug;
    QString description;
    QDateTime start;
    QDateTime end;
    QDateTime created;
    QDateTime updated;
    QDateTime lockedAt;
    QVariantHash settings;
    SimpleUser lockedBy;
    QTimeZone timezone;
    dbid_t id;
    Event::Audience audience = Event::InvalidAudience;
    Event::Participation participation = Event::InvalidParticipation;
    bool allDay = false;
    bool startOnly = false;
};

#endif // GIKWIMI_EVENT_P_H
