/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcenterevents.h"
#include "../objects/user.h"
#include "../objects/event.h"
#include "../objects/error.h"
#include "../objects/guest.h"
#include "../utils.h"

ControlCenterEvents::ControlCenterEvents(QObject *parent)
    : Controller{parent}
{

}

ControlCenterEvents::~ControlCenterEvents() = default;

void ControlCenterEvents::index(Context *c)
{
    const User currentUser = User::fromStash(c);

    Error e;
    const std::vector<Event> events = Event::list(c, &e, currentUser);

    if (e.type() != Error::NoError) {
        e.toStash(c, true);
        return;
    }

    QMap<QString,QString> eventsTableHeaders;
    //: table header
    eventsTableHeaders.insert(QStringLiteral("id"), c->translate("ControlCenterEvents", "ID"));
    //: table header
    eventsTableHeaders.insert(QStringLiteral("title"), c->translate("ControlCenterEvents", "title"));
    //: table header
    eventsTableHeaders.insert(QStringLiteral("start"), c->translate("ControlCenterEvents", "start"));
    //: table header
    eventsTableHeaders.insert(QStringLiteral("end"), c->translate("ControlCenterEvents", "end"));

    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterEvents", "Events")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/index.tmpl")},
                 {QStringLiteral("events"), QVariant::fromValue<std::vector<Event>>(events)},
                 {QStringLiteral("events_table_headers"), QVariant::fromValue<QMap<QString,QString>>(eventsTableHeaders)}
             });
}

void ControlCenterEvents::base(Context *c, const QString &id)
{
    bool ok = false;
    const dbid_t eventId = Utils::strToDbid(id, &ok, c->translate("ControlCenterEvents", "Invalid event database ID."), c);
    if (Q_LIKELY(ok)) {
        Event::toStash(c, eventId);
    }
}

void ControlCenterEvents::edit(Context *c)
{
    c->stash({
                 {QStringLiteral("site_subtitle"), c->translate("ControlCenterEvents", "Edit")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/edit.tmpl")}
             });
}

void ControlCenterEvents::guests(Context *c)
{
    const auto event = Event::fromStash(c);
    Error e;
    const std::vector<Guest> guests = Guest::list(c, &e, event);
    if (e.type() != Error::NoError) {
        e.toStash(c, true);
        return;
    }

    QMap<QString,QString> guestsTableHeaders;

    c->stash({
                 {QStringLiteral("site_subtitle"), c->translate("ControlCenterEvents", "Guests")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/guests/index.tmpl")},
                 {QStringLiteral("guests"), QVariant::fromValue<std::vector<Guest>>(guests)},
                 {QStringLiteral("guests_table_headers"), QVariant::fromValue<QMap<QString,QString>>(guestsTableHeaders)}
             });
}

void ControlCenterEvents::addGuest(Context *c)
{
    c->stash({
                 {QStringLiteral("site_subtitle"), c->translate("ControlCenterEvents", "Add guests")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/guests/add.tmpl")}
             });
}

void ControlCenterEvents::editGuest(Context *c, const QString &id)
{
    c->stash({
                 {QStringLiteral("site_subtitle"), c->translate("ControlCenterEvents", "Edit guests")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/guests/edit.tmpl")}
             });
}

void ControlCenterEvents::add(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterEvents", "Add Event")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/add.tmpl")}
             });
}
