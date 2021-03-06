/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_CONTROLCENTEREVENTS_H
#define GIKWIMI_CONTROLCENTEREVENTS_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class ControlCenterEvents : public Controller
{
    Q_OBJECT
    C_NAMESPACE("controlcenter/events")
public:
    explicit ControlCenterEvents(QObject *parent = nullptr);
    ~ControlCenterEvents() override;

    C_ATTR(index, :Path :Args(0))
    void index(Context *);

    C_ATTR(add, :Path("add") :Args(0))
    void add(Context *c);

    C_ATTR(base, :Chained("/") :PathPart("controlcenter/events") :CaptureArgs(1))
    void base(Context *c, const QString &id);

    C_ATTR(edit, :Chained("base") :PathPart("edit") :Args(0))
    void edit(Context *c);

    C_ATTR(guests, :Chained("base") :PathPart("guests") :Args(0))
    void guests(Context *c);

    C_ATTR(addGuest, :Chained("base") :PathPart("guests/add") :Args(0))
    void addGuest(Context *c);

    C_ATTR(addGuestToGroup, :Chained("base") :PathPart("guests/addtogroup") :Args(1))
    void addGuestToGroup(Context *c, const QString &groupId);

    C_ATTR(editGuest, :Chained("base") :PathPart("guests/edit") :Args(1))
    void editGuest(Context *c, const QString &id);

    C_ATTR(removeGuest, :Chained("base") :PathPart("guests/remove") :Args(1))
    void removeGuest(Context *c, const QString &id);

    C_ATTR(inviteGuest, :Chained("base") :PathPart("guests/invite") :Args(2))
    void inviteGuest(Context *c, const QString &id, const QString &notificationValue);

    C_ATTR(guestGroups, :Chained("base") :PathPart("guestgroups") :Args(0))
    void guestGroups(Context *c);

    C_ATTR(addGuestGroup, :Chained("base") :PathPart("guestgroups/add") :Args(0))
    void addGuestGroup(Context *c);

    C_ATTR(editGuestGroup, :Chained("base") :PathPart("guestgroups/edit") :Args(1))
    void editGuestGroup(Context *c, const QString &id);

    C_ATTR(removeGuestGroup, :Chained("base") :PathPart("guestgroups/remove") :Args(1))
    void removeGuestGroup(Context *c, const QString &id);

    C_ATTR(templates, :Chained("base") :PathPart("templates") :Args(0))
    void templates(Context *c);

    C_ATTR(addTemplate, :Chained("base") :PathPart("templates/add") :Args(0))
    void addTemplate(Context *c);

    C_ATTR(editTemplate, :Chained("base") :PathPart("templates/edit") :Args(1))
    void editTemplate(Context *c, const QString &id);
};

#endif // CONTROLCENTEREVENTS_H
