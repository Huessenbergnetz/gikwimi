/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcenterusers.h"
#include "objects/menuitem.h"
#include "logging.h"

ControlCenterUsers::ControlCenterUsers(QObject *parent)
    : Controller{parent}
{

}

ControlCenterUsers::~ControlCenterUsers() = default;

void ControlCenterUsers::index(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterUsers", "Users")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/users/index.tmpl")}
             });
}

void ControlCenterUsers::add(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterUsers", "Add User")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/users/add.tmpl")}
             });
}

#include "moc_controlcenterusers.cpp"
