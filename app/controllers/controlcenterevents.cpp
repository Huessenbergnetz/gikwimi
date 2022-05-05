/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcenterevents.h"
#include "../objects/user.h"
#include "../objects/event.h"
#include "../objects/error.h"

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

void ControlCenterEvents::add(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterEvents", "Add Event")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/add.tmpl")}
             });
}
