/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcenteraddressbooks.h"
#include "global.h"

#include <Cutelyst/Plugins/Authentication/authenticationuser.h>

ControlCenterAddressBooks::ControlCenterAddressBooks(QObject *parent)
    : Controller{parent}
{

}

ControlCenterAddressBooks::~ControlCenterAddressBooks() = default;

void ControlCenterAddressBooks::index(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterAddressBook", "Addressbook")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/addressbook/index.tmpl")}
             });
}

void ControlCenterAddressBooks::add(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterAddressBook", "Add addressbook")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/addressbook/add.tmpl")}
             });
}

void ControlCenterAddressBooks::edit(Context *c, const QString &id)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterAddressBook", "Edit addressbook")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/addressbook/edit.tmpl")}
             });
}
